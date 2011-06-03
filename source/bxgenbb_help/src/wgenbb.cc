// -*- mode: c++; -*- 
// wgenbb.cc
/*
 * Copyright 2007-2011 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Publi * License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Publi * License for more details.
 * 
 * You should have received a copy of the GNU General Publi * License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 * Boston, MA 02110-1301, USA.
 *
 */
 
#include <genbb_help/wgenbb.h>

#include <stdexcept>
#include <string>
#include <sstream>
#include <fstream>

#include <genbb_help/rng.h>

#include <datatools/utils/utils.h>
#include <datatools/utils/units.h>
#include <boost/filesystem.hpp>

extern GENEVENT_DEF genevent;
extern ENRANGE_DEF  enrange;

namespace genbb {
 
  using namespace std;

  size_t wgenbb::__g_counter = 0;

  bool wgenbb::is_debug () const
  {
    return __debug;
  }
 
  void wgenbb::set_debug (bool d_)
  {
    __debug = d_;
    return;
  }

  // ctor:
  wgenbb::wgenbb () : i_genbb ()
  {
    if (wgenbb::__g_counter > 0)
      {
	throw runtime_error ("genbb::wgenbb::wgenbb: Only one instance of wgenbb is allowed !");
      }
    wgenbb::__g_counter++;
    __debug = false;
    __initialized = false;
    __event_count = 0;
    __decay_type = DECAY_TYPE_UNDEFINED;
    __decay_isotope = "";
    for (int i = 0; i < 32; i++)
      {
	__c_decay_isotope[i] = 0;
      }  
    __decay_dbd_level = 0;  
    __decay_dbd_mode = 1;
    __use_local_prng = false;
    __seed = 0;
    datatools::utils::invalidate (__energy_min);
    datatools::utils::invalidate (__energy_max);
    return;
  }
  
  // dtor:
  wgenbb::~wgenbb ()
  {
    if (__initialized)
      {
	reset ();
      }
    //clog << "DEVEL: wgenbb::~wgenbb: Exiting." << endl;
    return;
  }

  const mygsl::rng & wgenbb::get_random () const
  {
    if (! __use_local_prng)
      {
	return genbb::rng::g__ran;
      }
    return __random;
  }

  mygsl::rng & wgenbb::get_random ()
  {
    if (! __use_local_prng)
      {
	return genbb::rng::g__ran;
      }
    return __random;
  }

  void wgenbb::__clean ()
  {
    return;
  }

  void wgenbb::reset ()
  {
    if (! __initialized)
      {
	return;
      }
    __clean ();
    __event_count = 0;

    __decay_type = DECAY_TYPE_UNDEFINED;
    for (int i = 0; i < 32; i++)
      {
	__c_decay_isotope[i] = 0;
      }  
    __decay_isotope = "";  
    __decay_dbd_level = 0;  
    __decay_dbd_mode = 1;

    datatools::utils::invalidate (__energy_min);
    datatools::utils::invalidate (__energy_max);

    __seed = 0;
    if (use_local_prng ())
      {
	clog << "NOTICE: " << "genbb::wgenbb::reset: "
	     << "Reseting the local PRNG..." << endl;
	__random.reset ();
      }
    __use_local_prng = false;

    __initialized = false;
    return;
  }

  bool wgenbb::use_local_prng () const
  {
    return __use_local_prng;
  }
 
  void wgenbb::set_use_local_prng (bool u_)
  {
    if (__initialized)
      {
	throw runtime_error ("genbb::wgenbb::set_use_local_prng: Already initialized !");
      }
    __use_local_prng = u_;
    return;
  }

  void wgenbb::initialize (const datatools::utils::properties & config_)
  {
    if (__initialized)
      {
	throw runtime_error ("genbb::wgenbb::initialize: Already initialized !");
      }

    string decay_isotope;
    double energy_unit = CLHEP::MeV;
    double emin = 0.0;
    double emax = -1.0;
    datatools::utils::invalidate (emin);
    datatools::utils::invalidate (emax);

    if (config_.has_flag ("debug"))
      {
	__debug = true;
      }

    if (config_.has_key ("local_prng"))
      {
	set_use_local_prng (true);
      }

    if (config_.has_key ("seed"))
      {
	long seed = config_.fetch_integer ("seed");
	if (seed < 0)
	  {
	    throw runtime_error ("genbb::wgenbb::initialize: Invalid seed value (>=0) !");
	  }
	__seed = seed;
      }
    if (config_.has_key ("energy_unit"))
      {
	string unit_str = config_.fetch_string ("energy_unit");
	energy_unit = datatools::utils::units::get_energy_unit_from (unit_str);
      }

    if (config_.has_key ("energy_max"))
      {
	emax = config_.fetch_real ("energy_max");
	if (emax < 0.0)
	  {
	    throw runtime_error ("genbb::wgenbb::initialize: Invalid maximum value !");
	  }
	emax *= energy_unit;
	__energy_max = emax;
      }

    if (config_.has_key ("energy_min"))
      {
	emin = config_.fetch_real ("energy_min");
	if (emin < 0.0)
	  {
	    throw runtime_error ("genbb::wgenbb::initialize: Invalid minimum value !");
	  }
	emin *= energy_unit;
	__energy_min = emin;
      }

    if (! config_.has_key ("decay_type"))
      {
	ostringstream message;
	message << "genbb::wgenbb::initialize: Missing 'decay_type' property !";
	throw runtime_error (message.str());
      }
    else
      {
	string tmp = config_.fetch_string ("decay_type");
	if ((tmp != "DBD") && (tmp != "background"))
	  {
	    ostringstream message;
	    message << "genbb::wgenbb::initialize: Invalid decay type '"
		    << tmp << "' !";
	    throw runtime_error (message.str());
	  }
	if (tmp == "background") 
	  {
	    __decay_type = DECAY_TYPE_BACKGROUND;
	  }

	if (tmp == "DBD") 
	  {
	    __decay_type = DECAY_TYPE_DBD;

	    if (! config_.has_key ("decay_dbd_level"))
	      {
		ostringstream message;
		message << "genbb::wgenbb::initialize: Missing DBD decay level !";
		throw runtime_error (message.str());
	      }
	    __decay_dbd_level = config_.fetch_integer ("decay_dbd_level");

	    if (! config_.has_key ("decay_dbd_mode"))
	      {
		ostringstream message;
		message << "genbb::wgenbb::initialize: Missing DBD decay mode !";
		throw runtime_error (message.str());
	      }
	    __decay_dbd_mode = config_.fetch_integer ("decay_dbd_mode");
	  }

     }
 
    if (! config_.has_key ("decay_isotope"))
      {
	ostringstream message;
	message << "genbb::wgenbb::initialize: Missing 'decay_isotope' property !";
	throw runtime_error (message.str());
      }
    else
      {
	decay_isotope = config_.fetch_string ("decay_isotope");
      }

    __set_decay_isotope (decay_isotope);

    if (use_local_prng ())
      {
	clog << "NOTICE: " << "genbb::wgenbb::initialize: "
	     << "Initializing the local PRNG..." << endl;
	__random.init ("taus2", __seed);
	clog << "NOTICE: " << "genbb::wgenbb::initialize: "
	     << "Installing the local PRNG as the global PRNG for the fortran interface..." << endl;
	genbb::rng::set_genbb_external_prng (__random);
      }

    __init ();

    __initialized = true;
    return;
  }

  bool wgenbb::has_next ()
  {
    return __initialized;
  }

  void wgenbb::__set_decay_isotope (const string & di_)
  {
    __decay_isotope = di_;
    size_t sz = di_.length ();
    if (sz >= 15)
      {
	throw runtime_error ("genbb::wgenbb::__set_decay_isotope: Decay isotope string too long !");
      }
    for (int i = 0; i < sz; i++)
      {
	__c_decay_isotope[i] = di_[i];
      }
    return;
  }

  void wgenbb::_load_next (primary_event & event_, 
			 bool compute_classification_)
  {
    if (__debug)
      {
	clog << "debug: " << "genbb::wgenbb::_load_next: "
	     << "Entering..."
	     << endl;
      }
    if (! __initialized)
      {
	throw runtime_error ("genbb::wgenbb::_load_next: Not initialized !");
      }
    event_.reset ();

    int error;
    int start = 1;

    // reset Fortran common block:
    genevent.reset ();

    genbbsub (&__decay_type, 
	      __c_decay_isotope, 
	      &__decay_dbd_level, 
	      &__decay_dbd_mode, 
	      &start,
	      &error);
    if (error != 0)
      {
	throw runtime_error ("genbb::wgenbb::_load_next: genbbsub failed !");
      }

    int evnum;
    double time;
    int npart;

    evnum = 0;
    event_.time = genevent.tevst * CLHEP::second;
    double part_time = 0.0;
    npart = genevent.npfull;
    /*
    clog << "DEVEL: " << "genbb::wgenbb::_load_next: "
	 << "npart=" << npart << endl;
    */
    for (int i = 0; i < npart; i++)
      {
	primary_particle pp;
	int part_type;
	double x, y ,z, time_shift;
	part_type = genevent.npgeant[i];
	/*
	clog << "DEVEL: " << "genbb::wgenbb::_load_next: "
	     << "part_type=" << part_type << endl;
	*/
	x = genevent.pmoment[i][0];
	y = genevent.pmoment[i][1];
	z = genevent.pmoment[i][2];
	/*
	clog << "DEVEL: " << "genbb::wgenbb::_load_next: "
	     << " px=" << x 
	     << " py=" << y
	     << " pz=" << z 
	     << endl;
	*/
	time_shift = genevent.ptime[i];
	/*
	clog << "DEVEL: " << "genbb::wgenbb::_load_next: "
	     << "time_shift=" << time_shift << endl;
	*/
	part_time += time_shift; 
	pp.set_type (part_type);
	pp.set_time (part_time * CLHEP::second); // GENBB unit is s
	geomtools::vector_3d p (x, y, z);
	p *= CLHEP::MeV; // GENBB unit is MeV/c
	pp.set_momentum (p);
	event_.add_particle (pp);
      }

    if (compute_classification_)
      {
	event_.compute_classification ();
      }
    //event_.dump (clog , "GENBB event: ", "DEVEL: ");
    __event_count++;
    if (__debug)
      {
	clog << "debug: " << "genbb::wgenbb::_load_next: "
	     << "Exiting."
	     << endl;
      }
    return;
  }

  size_t wgenbb::get_event_count () const
  {
    return __event_count;
  }

  double wgenbb::get_to_all_events () const
  {
    return 1.0 * enrange.toallevents;
  }

  void wgenbb::__init ()
  {
    if (__debug)
      {
	clog << "debug: " << "genbb::wgenbb::__init: "
	     << "Entering..."
	     << endl;
      }

    if (__decay_type == DECAY_TYPE_UNDEFINED)
      {
	throw runtime_error ("genbb::wgenbb::__init: Decay type is not defined !");	
      }

    enrange.reset ();
    if (datatools::utils::is_valid (__energy_min))
      {
	clog << "NOTICE: wgenbb::__init: Setting energy min to " << __energy_min / CLHEP::MeV << " MeV" << endl;
	enrange.ebb1 = (float) (__energy_min / CLHEP::MeV);	
      }
    if (datatools::utils::is_valid (__energy_max))
      {
	clog << "NOTICE: wgenbb::__init: Setting energy max to " << __energy_max / CLHEP::MeV << " MeV" << endl;
	enrange.ebb2 = (float) (__energy_max / CLHEP::MeV);	
      }
    if (enrange.ebb1 >= enrange.ebb2)
      {
	ostringstream message;
	message << "genbb::wgenbb::__init: "
		<< "Invalid energy range !";
	throw runtime_error (message.str ());
      }

    if (! __use_local_prng)
      {
	int genbb_seed = __seed;
	set_genbb_random_seed (&genbb_seed);
      }
    /*
    float test_alea[3];
    int sz = 3;
    grndm (test_alea, &sz);
    if (__debug)
      {
	clog << "DEBUG: wgenbb::__init: test_alea=" 
	     << test_alea[0] << ' ' 
	     << test_alea[2] << ' ' 
	     << test_alea[2] << endl;
      }
    */
    int error;
    int start = -1;
    genbbsub (&__decay_type, 
	      __c_decay_isotope, 
	      &__decay_dbd_level, 
	      &__decay_dbd_mode, 
	      &start,
	      &error);
    if (error != 0)
      {
	throw runtime_error ("genbb::wgenbb::__init: genbbsub failed !");
      }

    if (__debug)
      {
	clog << "debug: " << "genbb::wgenbb::__init: "
	     << "Exiting."
	     << endl;
      }
    return;
  }

  void wgenbb::dump (ostream & out_) const
  {
    const string tag = "|-- ";
    const string last_tag = "`-- ";
    out_ << "C++ GENBB wrapper: " << endl;
    out_ << tag << "debug            : " << __debug << endl;
    out_ << tag << "decay_type       : " << __decay_type << endl;
    out_ << tag << "decay_isotope    : " << __decay_isotope << endl;
    out_ << tag << "decay_dbd_level  : " << __decay_dbd_level << endl;
    out_ << tag << "decay_dbd_mode   : " << __decay_dbd_mode << endl;
    out_ << tag << "event_count      : " << __event_count << endl;
    out_ << tag << "energy_min       : " << __energy_min / CLHEP::keV << " keV" << endl;
    out_ << tag << "energy_max       : " << __energy_max / CLHEP::keV << " keV" << endl;
    out_ << tag << "use local PRNG   : " << __use_local_prng << endl;
    out_ << last_tag << "seed             : " << __seed << endl;
    return;
  }

} // end of namespace genbb

// end of wgenbb.cc
