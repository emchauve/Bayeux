// -*- mode: c++; -*-
/* library_loader.cc
 */
// Ourselves
#include <datatools/library_loader.h>

// Standard Library
#include <cstdlib>
#include <sstream>
#include <stdexcept>
#include <vector>

// Third Party
// - Boost
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/scoped_ptr.hpp>

// This Project
#include <datatools/detail/DynamicLoader.h>

#include <datatools/utils.h>
#include <datatools/handle.h>
#include <datatools/multi_properties.h>
#include <datatools/exception.h>
#include <datatools/logger.h>

namespace datatools {


//----------------------------------------------------------------------
// library_entry struct
// ctor :
library_entry_type::library_entry_type(const std::string& lib_name,
                                       const std::string& lib_directory,
                                       const std::string& lib_filename,
                                       const std::string& lib_full_path,
                                       const std::string& lib_version,
                                       bool lib_autoload)
    : name(lib_name), directory(lib_directory), filename(lib_filename),
      full_path(lib_full_path), version(lib_version),
      autoload(lib_autoload), status(0), handle(0) {}

// dtor :
library_entry_type::~library_entry_type() {
  if (handle != 0) {
    int status = datatools::detail::DynamicLoader::CloseLibrary(handle);
    if (status != 1) {
      std::ostringstream message;
      message << "library_entry_type::dtor: The '"
              << name << "' library was not closed ! "
              << BOOST_PP_STRINGIZE(DATATOOLS_SYS_NAMESPACE) << " says: '"
              << datatools::detail::DynamicLoader::LastError()
              << "' !";
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR,message.str());
    }
  }
  handle = 0;
}

void library_entry_type::print(std::ostream& out,
                               const std::string& indent) const {
  out << indent << "|-- Name      : " << name << std::endl;
  if (!directory.empty()) {
    out << indent << "|-- Directory : '" << directory << "'" << std::endl;
  }
  if (!filename.empty()) {
    out << indent << "|-- Filename  : '" << filename << "'" << std::endl;
  }
  out << indent << "|-- Full path : '" << full_path << "'" << std::endl;
  {
    std::string fpe = full_path;
    fetch_path_with_env(fpe);
    if (fpe != full_path) {
      out << indent << "|    `-- Expanded to : '" << fpe << "'" << std::endl;
    }
  }
  if (!version.empty()) {
    out << indent << "|-- Version : '" << version << "'" << std::endl;
  }
  out << indent << "|-- Autoload  : " << (autoload ? "Yes" : "No") << std::endl;
  out << indent << "`-- Loaded    : " << (handle ? "Yes" : "No");
  if (handle != 0) {
    out << " [" << handle << "]";
  }
  out << std::endl;
}


//----------------------------------------------------------------------
// library_loader class

bool library_loader::is_debug() const {
  return flags_ & debug;
}


void library_loader::set_debug(bool allow) {
  if (allow) {
    flags_ |= debug;
  } else {
    flags_ &= debug;
  }
}


bool library_loader::is_test() const {
  return flags_ & test;
}


void library_loader::set_allow_unregistered(bool allow) {
  if (allow) {
    flags_ |= allow_unregistered;
  } else {
    flags_ &= allow_unregistered;
  }
}


bool library_loader::allowing_unregistered() const {
  return flags_ & allow_unregistered;
}


symbol_ptr library_loader::get_symbol_address(const std::string& lib_name_,
                                              const std::string& symbol_) {
  // shorter typename?
  handle_library_entry_dict_type::const_iterator found = libraries_.find(lib_name_);
  DT_THROW_IF (found == libraries_.end(),
               std::logic_error,
               "The shared library file '" << lib_name_ << "' is not registered !");
  DT_THROW_IF (found->second.get().handle == 0,
                std::logic_error,
               "The shared library file '" << lib_name_ << "' is not loaded !");
  return datatools::detail::DynamicLoader::GetSymbolAddress(found->second.get().handle, symbol_.c_str());
}


// ctor :
library_loader::library_loader(uint32_t flags,
                               const std::string& config_file) {
  flags_ = flags;
  config_filename_ = config_file;
  this->init();
}


int library_loader::close_all() {
  while (!stacked_libraries_.empty()) {
    handle_library_entry_type& hle = stacked_libraries_.front();
    if (!hle) {
      stacked_libraries_.pop_front(); // remove top element
    } else {
      library_entry_type& le = hle.grab();
      if (le.handle != 0) {
        if (this->is_debug()) {
          DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG,"Closing library '" << le.name << "'...");
        }

        int status = datatools::detail::DynamicLoader::CloseLibrary(le.handle);
        if (status != 1) {
          std::ostringstream message;
          message << "datatools::library_loader::close_all: The '"
                  << le.name
                  << "' library was not closed ! "
                  << BOOST_PP_STRINGIZE(DATATOOLS_SYS_NAMESPACE)
                  << " says: '"
                  << datatools::detail::DynamicLoader::LastError()
                  << "' !";
          DT_LOG_ERROR(datatools::logger::PRIO_ERROR,message.str());
          //return;
        }

        // 2011/09/19: F.M & X.G: Even if the return status
        // from DATATOOLS_SYS_NAMESPACE::DynamicLoader::CloseLibrary is not 0,
        // the library is closed by DATATOOLS_SYS_NAMESPACE, so we removed the
        // library entry from the library stack. Actually
        // the return status is really not explicit and can
        // not be used.

        // else
        //   {
        le.handle = 0;
        stacked_libraries_.pop_front (); // remove top element
        // }
      }
    }
  }
  return EXIT_SUCCESS;
}


// dtor :
library_loader::~library_loader() {
  this->close_all();
  libraries_.clear();
}


bool library_loader::has(const std::string& name) const {
  return libraries_.find(name) != libraries_.end();
}


bool library_loader::is_loaded(const std::string& name) const {
  handle_library_entry_dict_type::const_iterator found = libraries_.find(name);

  if (found != libraries_.end()) {
    if (found->second.get().handle != 0) {
      return true;
    }
  }
  return false;
}


int library_loader::registration(const std::string& lib_name_,
                                 const std::string& lib_directory_,
                                 const std::string& lib_filename_,
                                 const std::string& lib_full_path_,
                                 const std::string& lib_version_,
                                 bool           autoload_) {
  if (this->has(lib_name_)) {
    return EXIT_FAILURE;
  }

  handle_library_entry_type le_handle(new library_entry_type);
  libraries_[lib_name_] = le_handle;
  library_entry_type& le = libraries_[lib_name_].grab();
  le.name         = lib_name_;
  le.filename     = lib_filename_;
  le.version      = lib_version_;
  le.directory    = lib_directory_;
  le.full_path    = lib_full_path_;

  if (le.filename.empty()) {
    std::ostringstream filename_ss;
    filename_ss << datatools::detail::DynamicLoader::LibPrefix();
    filename_ss << le.name;
    filename_ss << datatools::detail::DynamicLoader::LibExtension();

    if (!le.version.empty()) {
      filename_ss << le.version;
    }
    le.filename = filename_ss.str();
  }

  if (le.full_path.empty()) {
    std::ostringstream full_path_ss;
    if (!le.directory.empty()) {
      full_path_ss << le.directory;
      if (!boost::algorithm::ends_with(le.directory, "/")) {
        full_path_ss << '/';
      }
    }
    full_path_ss << le.filename;
    le.full_path = full_path_ss.str();
  }

  DT_THROW_IF (le.full_path.empty(),
               std::logic_error,
               "The shared library file '" << le.name << "' has no valid path !");

  if (!le.directory.empty()) {
    // Only check the existence of the DLL file if a directory is
    // explicitely given :
    std::string check = le.full_path;
    fetch_path_with_env(check);
    DT_THROW_IF (!boost::filesystem::exists(check),
                 std::logic_error,
                 "The shared library file '" << le.name << "' has no valid file '" << le.full_path << "' !");
  }
  le.handle = 0;
  le.autoload = autoload_;
  return EXIT_SUCCESS;
}


int library_loader::load(const std::string& lib_name_,
                         const std::string& directory_,
                         const std::string& filename_,
                         const std::string& full_lib_path_,
                         const std::string& version_) {
  handle_library_entry_dict_type::iterator found = libraries_.find(lib_name_);
  if (found == libraries_.end()) {
    if (!this->allowing_unregistered()) {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR,"Library '" << lib_name_ << "' is not registered !");
      return EXIT_FAILURE;
    } else {
      int status = this->registration(lib_name_,
                                      directory_,
                                      filename_,
                                      full_lib_path_,
                                      version_,
                                      true);
      if (status != EXIT_SUCCESS) {
        std::ostringstream message;
        message << "datatools::library_loader::load: "
                << "Cannot register library '"
                << lib_name_ << "' !";
        DT_LOG_ERROR(datatools::logger::PRIO_ERROR,message.str());
        return EXIT_FAILURE;
      }
      found = libraries_.find(lib_name_);
    }
  }
  library_entry_type& le = found->second.grab();
  std::string check = le.full_path;
  fetch_path_with_env(check);
  le.handle = datatools::detail::DynamicLoader::OpenLibrary(check.c_str());

  if (le.handle != 0) {
    stacked_libraries_.push_front(found->second);
  } else {
    std::ostringstream message;
    message << "datatools::library_loader::load: "
            << "The '" << lib_name_ << "' library was not loaded ! "
            << BOOST_PP_STRINGIZE(DATATOOLS_SYS_NAMESPACE) << " says: '"
            << datatools::detail::DynamicLoader::LastError() << "' !";
    DT_LOG_ERROR(datatools::logger::PRIO_ERROR,message.str());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}


int library_loader::close(const std::string& lib_name_) {
  if (!this->is_loaded(lib_name_)) {
    DT_LOG_ERROR(datatools::logger::PRIO_ERROR,"No loaded library '" << lib_name_ << "' to be closed !");
    return EXIT_FAILURE;
  }

  if (stacked_libraries_.front().get().name != lib_name_) {
    DT_LOG_ERROR(datatools::logger::PRIO_ERROR,"Cannot close library '" << lib_name_ << "' !");
    return EXIT_FAILURE;
  }

  handle_library_entry_dict_type::iterator found = libraries_.find(lib_name_);
  handle_library_entry_type& hle = found->second;
  library_entry_type& le = hle.grab();

  int status = datatools::detail::DynamicLoader::CloseLibrary(le.handle);
  if (status != 1) {
    std::ostringstream message;
    message << "datatools::library_loader::close: "
            << "The '" << le.name << "' library was not closed ! "
            << BOOST_PP_STRINGIZE(DATATOOLS_SYS_NAMESPACE)
            << " says: '"
            << datatools::detail::DynamicLoader::LastError() << "' !";
    DT_LOG_ERROR(datatools::logger::PRIO_ERROR,message.str());
    return EXIT_FAILURE;
  } else {
    stacked_libraries_.front ().grab().handle = 0;
    stacked_libraries_.pop_front();
  }
  return EXIT_SUCCESS;
}


void library_loader::init() {
  if (config_filename_.empty()) {
    //DT_LOG_WARNING(datatools::logger::PRIO_WARNING, "No library loader config file is available !");
    return;
  }

  namespace du = datatools;
  fetch_path_with_env(config_filename_);
  du::multi_properties config("name", "filename");
  config.read(config_filename_);

  BOOST_FOREACH(const multi_properties::entry* ptr,
                config.ordered_entries()) {
    const multi_properties::entry& e = *ptr;
    const properties& lib_properties = e.get_properties();
    std::string lib_name       = e.get_key ();
    std::string lib_filename   = e.get_meta ();
    std::string lib_directory  = "";
    std::string lib_full_path  = "";
    std::string lib_version    = "";
    bool lib_autoload   = false;

    if (lib_properties.has_key("full_path")) {
      lib_full_path = lib_properties.fetch_string("full_path");
    } else {
      if (lib_properties.has_key("directory")) {
        lib_directory = lib_properties.fetch_string("directory");
      }
      if (lib_properties.has_key("filename")) {
        lib_filename = lib_properties.fetch_string("filename");
      }
    }

    if (lib_properties.has_flag ("autoload")) {
      lib_autoload = lib_properties.fetch_boolean("autoload");
    }

    if (lib_properties.has_key ("version")) {
      lib_version = lib_properties.fetch_string("version");
    }

    int status = this->registration(lib_name, lib_directory,
                                    lib_filename, lib_full_path,
                                    lib_version, lib_autoload);
    DT_THROW_IF (status != EXIT_SUCCESS,
                 std::logic_error,
                 "Cannot register library '" << lib_name << "' !");
    if (lib_autoload) {
      DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,"Automatic loading of library  '" << lib_name << "'...");
      int load_status = this->load(lib_name);
      DT_THROW_IF (load_status != EXIT_SUCCESS,
                   std::logic_error,
                   "Automatic loading of library='" << lib_name << "' failed !");
    }
    continue;
  } // BOOST_FOREACH

  if (this->is_debug()) this->print(std::clog);
}


void library_loader::print(std::ostream& out) const {
  out << "Library loader : " << std::endl;
  out << "Flags              : " << flags_ << std::endl;
  out << "Configuration file : '" << config_filename_ << "'" << std::endl;
  out << "List of registered shared libraries :" << std::endl;
  for (handle_library_entry_dict_type::const_iterator i
       = libraries_.begin();
       i != libraries_.end();
       ++i) {
    handle_library_entry_dict_type::const_iterator j = i;
    j++;
    const library_entry_type& le = i->second.get();
    std::string tag = "|   ";
    if (j == libraries_.end()) {
      out << "`-- ";
      tag = "    ";
    } else {
      out << "|-- ";
    }
    out << "Library : '" << i->first << "'" << std::endl;
    le.print(out, tag);
  }
  out << "List of loaded shared libraries :" << std::endl;
  for (handle_library_entry_stack_type::const_iterator i = stacked_libraries_.begin();
       i != stacked_libraries_.end();
       ++i) {
    handle_library_entry_stack_type::const_iterator j = i;
    j++;
    const library_entry_type& le = i->get();
    std::string tag = "|   ";
    if (j == stacked_libraries_.end()) {
      out << "`-- ";
      tag = "    ";
    } else {
      out << "|-- ";
    }
    out << "Library : '" << le.name << "'" << std::endl;
  }
}


} // end of namespace datatools


/***************
 * OCD support *
 ***************/

#include <datatools/ocd_macros.h>

// OCD support for class '::datatools::library_loader' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::datatools::library_loader,ocd_)
{
  ocd_.set_class_name ("datatools::library_loader");
  ocd_.set_class_description ("A shared library (DLL) loader");
  ocd_.set_class_library ("datatools");
  ocd_.set_class_documentation ("A library loader object is responsible of the dynamic loading\n"
                                "of shared libraries (DLL).                                   \n"
                                );

  ocd_.set_configuration_hints ("A library loader object may load its setup directives from a file that  \n"
                                "uses the format of the ``datatools::multi_properties`` object.\n"
                                "Each section corresponds to the loading rules of a specific   \n"
                                "shared library (DLL).                                         \n"
                                "                                                              \n"
                                "Example of configuration file: ::                             \n"
                                "                                                              \n"
                                "   #@key_label   \"name\"                                     \n"
                                "   #@meta_label  \"filename\"                                 \n"
                                "                                                              \n"
                                "   # A DLL with fully specified path :                        \n"
                                "   [name=\"z\" filename=\"\"]                                 \n"
                                "   #@config The zlib library                                  \n"
                                "   #@description The full path of the shared library file     \n"
                                "   full_path : string = \"/usr/lib/libz.so\"                  \n"
                                "                                                              \n"
                                "                                                              \n"
                                "   # Another DLL with fully specified path :                  \n"
                                "   [name=\"crypto\" filename=\"\"]                            \n"
                                "   #@config The crypto library                                \n"
                                "   #@description The full path of the shared library file     \n"
                                "   full_path : string = \"/usr/lib/libcrypto.so\"             \n"
                                "                                                              \n"
                                "                                                              \n"
                                "   # Yet another DLL with implicit path from LD_LIBRARY_PATH :\n"
                                "   [name=\"ssl\" filename=\"\"]                               \n"
                                "   #@config The libssl library                                \n"
                                "                                                              \n"
                                "                                                              \n"
                                "   # A DLL with explicit directory and filename :             \n"
                                "   [name=\"gsl\" filename=\"\"]                               \n"
                                "   #@config The GSL library                                   \n"
                                "   #@description The directory where the shared library file is stored \n"
                                "   directory : string = \"/usr/lib\"                          \n"
                                "                                                              \n"
                                "   #@description The shared library file                      \n"
                                "   filename : string = \"libgsl.so\"                          \n"
                                "                                                              \n"
                                "                                                              \n"
                                "   # A DLL with explicit filename and autoload at startup :   \n"
                                "   [name=\"genvtx\" filename=\"libgenvtx.so\"]                \n"
                                "   #@config The genvtx library                                \n"
                                "   #@description Automatic loading at startup                 \n"
                                "   autoload : boolean = 1                                     \n"
                                "                                                              \n"
                                "                                                              \n"
                                "   # A DLL with implicit path :                               \n"
                                "   [name=\"foo\" filename=\"\"]                               \n"
                                "   #@config The foo library                                   \n"
                                "                                                              \n"
                                );

  ocd_.set_validation_support(false);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()

DOCD_CLASS_SYSTEM_REGISTRATION(datatools::library_loader,"datatools::library_loader")
