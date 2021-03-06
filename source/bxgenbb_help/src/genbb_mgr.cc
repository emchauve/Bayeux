// genbb_mgr.cc
/*
 * Copyright 2007-2013 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

// Ourselves:
#include <genbb_help/genbb_mgr.h>

// Standard library:
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdlib>
#include <stdexcept>
#include <sstream>

// Third party:
// - CLHEP:
#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <CLHEP/Vector/ThreeVector.h>
// - Boost:
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/exception.h>
// - Bayeux/geomtools:
#include <geomtools/utils.h>

// This project:
#include <genbb_help/primary_event.h>

// Implementation of serialization method for the 'primary_event'
// class, implies also <genbb_help/primary_particle.ipp> :
#include <genbb_help/primary_event.ipp> // Mandatory

namespace genbb {

  using namespace std;

  GENBB_PG_REGISTRATION_IMPLEMENT(genbb_mgr,"genbb::genbb_mgr")

  // static
  const std::string & genbb_mgr::format_genbb_label()
  {
    static std::string label = "genbb";
    return label;
  }

  const std::string & genbb_mgr::format_boost_label()
  {
    static std::string label = "boost";
    return label;
  }

  bool genbb_mgr::is_initialized () const
  {
    return _initialized_;
  }

  bool genbb_mgr::is_debug () const
  {
    return _debug_;
  }

  void genbb_mgr::set_debug (bool d_)
  {
    _debug_ = d_;
  }

  int genbb_mgr::get_format () const
  {
    return _format_;
  }

  bool genbb_mgr::is_format_genbb () const
  {
    return _format_ == FORMAT_GENBB;
  }

  bool genbb_mgr::is_format_boost () const
  {
    return _format_ == FORMAT_BOOST;
  }

  // ctor:
  genbb_mgr::genbb_mgr (int format_)
  {
    _debug_ = false;
    _initialized_ = false;
    _in_ = 0;
    _format_ = FORMAT_GENBB;
    _genbb_weight_ = 1.0;
    set_format (format_);
    return;
  }

  // dtor:
  genbb_mgr::~genbb_mgr ()
  {
    reset ();
    return;
  }

  void genbb_mgr::set_format (const string & format_label_)
  {
    DT_THROW_IF(_initialized_, logic_error, "Operation not allowed ! Manager is locked !");
    string format_label = format_label_;
    if (format_label.empty()) {
      format_label = format_genbb_label();
    }
    int fmt = FORMAT_GENBB;
    DT_THROW_IF ((format_label != format_genbb_label())
                 && (format_label != format_boost_label()),
                 logic_error,
                 "Invalid format label '" << format_label << "' !");
    if (format_label == format_genbb_label()) {
      fmt = FORMAT_GENBB;
    }
    if (format_label == format_boost_label()) {
      fmt = FORMAT_BOOST;
    }
    this->set_format (fmt);
    return;
  }

  void genbb_mgr::set_format (int format_)
  {
    DT_THROW_IF(_initialized_, logic_error, "Operation not allowed ! Manager is locked !");
    DT_THROW_IF ((format_ != FORMAT_GENBB) && (format_ != FORMAT_BOOST),
                 logic_error,
                 "Invalid format !");
    _format_ = format_;
    return;
  }

  void genbb_mgr::set (const string & filename_)
  {
    DT_THROW_IF(_initialized_, logic_error, "Operation not allowed ! Manager is locked !");
    _filenames_.push_back (filename_);
    return;
  }

  bool genbb_mgr::has_next ()
  {
    // return _current_.get_number_of_particles() > 0;
    return _current_.is_valid ();
  }

  void genbb_mgr::_load_next (primary_event & event_,
                              bool compute_classification_)
  {
    // provide the preloaded current event:
    event_ = _current_;
    if (compute_classification_) {
      event_.compute_classification ();
    }
    // try to load the next event:
    _load_next_ ();
    return;
  }

  void genbb_mgr::_load_next_boost_ ()
  {
    if (! _reader_.is_initialized ()) {
      string filename;
      if (_filenames_.size () == 0) {
        if (is_debug ()) clog << "genbb::genbb_mgr::_load_next_boost_: no more filenames!" << endl;
        return;
      }
      filename = _filenames_.front ();
      _filenames_.pop_front ();
      if (filename.empty ()) {
        if (is_debug ()) clog << "DEVEL: genbb_mgr::_load_next_boost_: filename = '" << filename << "'" << endl;
        return;
      }
      datatools::fetch_path_with_env (filename);
      _reader_.init (filename,
                     datatools::using_multi_archives);
      _current_filename_ = filename;
      return;
    }
    DT_THROW_IF (! _reader_.is_initialized (),
                 logic_error,
                 "Boost reader is not initialized !");
    DT_THROW_IF (! _reader_.has_record_tag (),
                 logic_error,
                 "Boost reader has no data !");
    if (_reader_.record_tag_is (primary_event::SERIAL_TAG)) {
      _reader_.load (_current_);
    }
    if (! _reader_.has_record_tag ()) {
      _reader_.reset ();
    }
    return;
  }

  void genbb_mgr::_load_next_ ()
  {
    _current_.reset();
    if (_format_ == FORMAT_GENBB) {
      _load_next_genbb_ ();
    }
    if (_format_ == FORMAT_BOOST) {
      _load_next_boost_ ();
    }
    return;
  }

  void genbb_mgr::_load_next_genbb_ ()
  {
    if (_in_ == 0) {
      if (is_debug ()) clog << "genbb::genbb_mgr::_load_next_genbb_: No input stream!" << endl;
      _genbb_weight_ = 1.0;
      string filename;

      if (_filenames_.size () == 0) {
        if (is_debug ()) clog << "genbb::genbb_mgr::_load_next_genbb_: No more filenames!" << endl;
        return;
      }

      filename = _filenames_.front ();
      _filenames_.pop_front ();
      if (filename.empty ()) {
        if (is_debug ()) clog << "DEVEL: genbb_mgr::_load_next_genbb_: Input filename = '" << filename << "'" << endl;
        return;
      }
      datatools::fetch_path_with_env (filename);
      if (is_debug ()) clog << "DEVEL: genbb_mgr::_load_next_genbb_: Input filename = '" << filename << "'" << endl;
      _fin_.close ();
      _fin_.open (filename.c_str ());
      DT_THROW_IF (! _fin_,runtime_error,
                   "Cannot open file '" << filename << "' !");
      _in_ = &_fin_;
      _current_filename_ = filename;
    }

    std::string line;
    while (true) {
      std::string token;
      std::getline (*_in_, line);
      {
        std::istringstream line_iss (line);
        line_iss >> token;
      }
      if (token.empty ()) {
        // skip white lines :
        continue;
      }
      if (token[0] != '#') {
        break;
      }
      if (token.length () == 1) {
        // skip empty comment lines :
        continue;
      }
      // parse special comment :
      if (token[1] != '@') {
        // skip pure comment lines :
        continue;
      }
      typedef vector<string> split_vector_type;
      split_vector_type svec;
      boost::algorithm::split(svec, line,
                              boost::algorithm::is_any_of("="),
                              boost::algorithm::token_compress_on );
      DT_THROW_IF (svec.size () != 2,
                   logic_error,
                   "Invalid syntax (" << line << ") !");
      if (svec[0] == "#@toallevents") {
        std::istringstream iss (svec[1]);
        double toallevents = 1.0;
        iss >> toallevents;
        DT_THROW_IF (! iss || toallevents < 0.0,
                     logic_error,
                     "Invalid value for 'toallevents' weight (" << line << ") !");
        _genbb_weight_ = 1.0  / toallevents;
        std::clog << "NOTICE: genbb::genbb_mgr::_load_next_genbb_: "
                  << "Load GENBB event weight = " << _genbb_weight_ << std::endl;
      }
    }

    if (is_debug ()) clog << "DEVEL: genbb_mgr::_load_next_genbb_: _in_ = " << _in_ << endl;

    DT_THROW_IF (! *_in_, logic_error, "Cannot load event !");
    int evnum;
    double time;
    int npart;
    {
      std::istringstream line_iss (line);
      line_iss >> ws >> evnum >> time >> npart >> ws;
      // Only throw if stream is bad and NOT at eof. Some c++ libraries
      // (Apple clang on Mavericks) will set bad/failbit at eof. Seems
      // not to be C++11 standard compliant, but is still present!
      DT_THROW_IF (! line_iss && !line_iss.eof(),
                   logic_error,
                   "Format error !");
    }

    /*
     *_in_ >> ws >> evnum >> time >> npart;
     if (! *_in_)
     {
     ostringstream message;
     message << "genbb::genbb_mgr::_load_next_genbb_: format error!";
     th row logic_error (message.str ());
     }
    */
    _current_.set_time (time * CLHEP::second);
    _current_.set_genbb_weight (_genbb_weight_);
    double part_time = 0.0;
    for (int i = 0; i < npart; i++) {
      primary_particle pp;
      int part_type;
      double x, y ,z, time_shift;
      // 2009-07-14 FM: Vladimir Tretyak email about particles' time shifts:
      *_in_ >> ws >> part_type >> x >> y >> z >> time_shift;
      part_time += time_shift;
      pp.set_type (part_type);
      pp.set_time (part_time * CLHEP::second); // GENBB unit is s
      if (! *_in_) {
        _fin_.close ();
        _in_ = 0;
        DT_THROW_IF(true, logic_error, "Format error !");
      }
      geomtools::vector_3d p (x, y, z);
      p *= CLHEP::MeV; // GENBB unit is MeV/c
      pp.set_momentum (p);
      _current_.add_particle (pp);
    }
    *_in_ >> ws;
    if (_fin_.eof ()) {
      _fin_.close ();
      _in_ = 0;
    }
    return;
  }

  void genbb_mgr::dump (ostream & out_) const
  {
    out_ << "genbb::genbb_mgr::dump: " << endl;
    out_ << "|-- Debug : " << _debug_ << endl;
    out_ << "|-- Initialized : " << _initialized_ << endl;
    out_ << "|-- Format : " << (_format_ == FORMAT_GENBB? "GENBB": "Boost")<< endl;
    out_ << "|-- Current input file: '" << _current_filename_ << "'" << endl;
    out_ << "|-- List of remaining input files : " << endl;
    for (list<string>::const_iterator it = _filenames_.begin ();
         it != _filenames_.end ();
         it++)
      {
        list<string>::const_iterator jt = it;
        jt++;
        out_ << "|   ";
        if (jt == _filenames_.end ())
          {
            out_ << "`-- ";
          }
        else
          {
            out_ << "|-- ";
          }
        out_ << "Filename : '" << *it << "'" << endl;
      }
    if (_format_ == FORMAT_GENBB)
      {
        out_ << "|-- GENBB current file address : " << hex << _in_
             << dec << endl;
      }
    if (_format_ == FORMAT_BOOST)
      {
        out_ << "|-- Current Boost reader : "
             << (_reader_.is_initialized ()? "Yes": "No") << endl;
      }
    out_ << "`-- Current event: " << endl;
    _current_.dump (out_, "    ");
    return;
  }

  void genbb_mgr::_at_init_ ()
  {
    _load_next_ ();
    return;
  }

  void genbb_mgr::initialize (const datatools::properties & config_,
                              datatools::service_manager & /*service_manager_*/,
                              detail::pg_dict_type & /*dictionary_*/)
  {
    DT_THROW_IF (_initialized_, logic_error, "Already initialized !");
    _initialize_base(config_);

    if (config_.has_flag ("debug")) {
      set_debug (true);
    }

    if (config_.has_key ("format")) {
      string format = config_.fetch_string ("format");
      set_format (format);
    }
    // else
    //   {
    //     th row logic_error ("genbb::genbb_mgr::initialize: Missing 'format' of input files !");
    //   }

    // try to build a list of input filenames from a pattern:
    {
      string input_files_dir = ".";
      string input_files_pattern = "*.genbb";
      if (config_.has_key ("input_files.pattern")) {
        input_files_pattern = config_.fetch_string ("input_files.pattern");
        DT_THROW_IF (input_files_pattern.empty (), logic_error,
                     "Input files pattern is empty !");
        if (config_.has_key ("input_files.directory")) {
          input_files_dir = config_.fetch_string ("input_files.directory");
        }

        char tmp_buffer[256];
        for (int i = 0; i < 256; i++) {
          tmp_buffer[i] = '\0';
        }
        string tmp_dir = ".";
        sprintf (tmp_buffer,"%s/%s", tmp_dir.c_str (), "genbb_mgr_pattern.XXXXXX");
        int file_desc= mkstemp (tmp_buffer);
        DT_THROW_IF (file_desc == -1,runtime_error, "Cannot create temporary file !");
        if (is_debug ()) clog << "DEBUG: genbb_mgr::initialize: Temporary file is: '" << tmp_buffer << "'" << endl;
        close (file_desc);
        string tmp_file = tmp_buffer;
        ostringstream sys_command;
        ostringstream genbb_mgr_log_file_ss;
        genbb_mgr_log_file_ss << tmp_dir << '/' << "genbb_mgr.log";
        string genbb_mgr_log_file = genbb_mgr_log_file_ss.str ();
        sys_command << "find " << input_files_dir
                    << " -maxdepth 1"
                    << " -name " << input_files_pattern
                    << " | sort > " << tmp_file
                    << " 2> " << genbb_mgr_log_file_ss.str ();
        if (is_debug ()) clog << "DEBUG: genbb_mgr::initialize: System command: '" << sys_command.str () << "'" << endl;
        int ret = system (sys_command.str ().c_str ());
        DT_THROW_IF (ret != 0, runtime_error,
                     "System command failed ! Check log file '"
                     << genbb_mgr_log_file << "' !");
        ifstream fin (tmp_file.c_str ());
        DT_THROW_IF (! fin, runtime_error,
                  "Cannot open list of input files '" << tmp_file << "' !");
        while (fin) {
          string line;
          getline (fin, line);
          istringstream line_iss (line);
          string filename;
          line_iss >> filename;
          if (! filename.empty ()) {
            this->set (filename);
          }
          fin >> ws;
          if (fin.eof ()) break;
        }
        unlink (tmp_file.c_str ());
        unlink (genbb_mgr_log_file.c_str ());
      }
    }

    // try to get a list of input filenames:
    {
      vector<string> input_files;
      if (config_.has_key ("input_files")) {
        config_.fetch ("input_files", input_files);
        for (size_t i = 0; i < input_files.size (); i++) {
          datatools::fetch_path_with_env (input_files[i]);
          set (input_files[i]);
        }
      } else {
        DT_THROW_IF (_filenames_.empty (), logic_error, "Missing list of input files !");
      }
    }

    _at_init_ ();
    _initialized_ = true;
    return;
  }

  void genbb_mgr::reset ()
  {
    if (! _initialized_) return;
    _at_reset_ ();
    _initialized_ = false;
    return;
  }

  void genbb_mgr::_at_reset_ ()
  {
    _genbb_weight_ = 1.0;
    _current_filename_ = "";
    _filenames_.clear ();

    // "genbb"
    if (_format_ == FORMAT_GENBB) {
      if (_in_ != 0)
        {
          _in_ = 0;
          _fin_.close ();
        }
    }

    // "boost":
    if (_format_ == FORMAT_BOOST) {
      if (_reader_.is_initialized ()) {
        _reader_.reset ();
      }
    }
    _format_ = FORMAT_GENBB;

    return;
  }

} // end of namespace genbb
