/* service_manager.cc
 *
 * Copyright (C) 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
// Ourselves
#include <datatools/services/service_manager.h>

// Standard Library
#include <stdexcept>
#include <sstream>

// Third Party

// Datatools
#include <datatools/services/base_service.h>
#include <datatools/utils/ioutils.h>
#include <datatools/utils/utils.h>
#include <datatools/utils/properties.h>
#include <datatools/utils/multi_properties.h>

namespace datatools {
namespace service {

void service_manager::set_name(const std::string& name_) {
  _name_ = name_;
}


void service_manager::set_description(const std::string& description_) {
  _description_ = description_;
}


const std::string& service_manager::get_name() const {
  return _name_;
}


const std::string& service_manager::get_description() const {
  return _description_;
}


bool service_manager::is_debug() const {
  return _debug_;
}


void service_manager::set_debug(bool debug_) {
  _debug_ = debug_;
}


void service_manager::_set_preload(bool preload_) {
  _preload_ = preload_;
  if (_preload_) {
    this->_preload_global_dict();
  }
}


bool service_manager::is_initialized() const {
  return _initialized_;
}


void service_manager::_create_service(service_entry& service_entry_) {
  if (!(service_entry_.service_status & service_entry::STATUS_CREATED)) {
    if (this->is_debug()) {
      std::clog << datatools::utils::io::debug
                << "datatools::service::service_manager::_create_service: "
                << "Creating service named '"
                <<  service_entry_.service_name
                << "'..."
                << std::endl;
    }

    // search for the cut's label in the factory dictionary:
    if (!_factory_register_.has(service_entry_.service_id)) {
      std::ostringstream message;
      message << "datatools::service::service_manager::_create_service: "
              << "Cannot find service factory with ID '"
              << service_entry_.service_id 
              << "' for service named '"
              << service_entry_.service_name << "' !";
      throw std::logic_error(message.str());
    }

    // You don't think this might be a *little* too deeply nested?
    const datatools::service::base_service::factory_register_type::factory_type& the_factory = _factory_register_.get(service_entry_.service_id);

    base_service* ptr = the_factory();
    // 2012/04/12: XG the initialization mechanism is done 'on
    // the demand' i.e. when the service is requested
    // ptr->initialize (service_entry_.service_config,
    // _services_);

    service_entry_.service_handle.reset(ptr);
    service_entry_.service_status |= service_entry::STATUS_CREATED;

    if (this->is_debug()) {
      std::clog << datatools::utils::io::debug
                << "datatools::service::service_manager::_create_service: "
                << "Service named '"
                <<  service_entry_.service_name
                << "' has been created !"
                << std::endl;
    }
  }
}


void service_manager::_reset_service(service_entry& service_entry_) {
  // Couldn't you just use booleans?
  if (service_entry_.service_status & service_entry::STATUS_INITIALIZED) {
    base_service& the_service = service_entry_.service_handle.get();
    the_service.reset();

    service_entry_.service_status ^= service_entry::STATUS_INITIALIZED;

    for (service_dependency_dict_type::iterator i = service_entry_.service_masters.begin();
         i != service_entry_.service_masters.end();
         ++i) {
      const std::string& master_name = i->first;
      service_dict_type::iterator found = _services_.find(master_name);
      
      if (found != _services_.end()) {
        service_entry& the_master_entry = found->second;
        the_master_entry.remove_slave(service_entry_.service_name);
        
        if (this->is_debug()) {
          std::clog << datatools::utils::io::debug
                    << "datatools::service::service_manager::_create_service: "
                    << "Remove slave '"
                    << service_entry_.service_name
                    << "' from master '" 
                    << the_master_entry.service_name
                    << "' !"
                    << std::endl;
        }
      }
    }
  }
}


void service_manager::_initialize_service(service_entry& service_entry_) {
  // If not created, first do it :
  if (!(service_entry_.service_status & service_entry::STATUS_CREATED)) {
    this->_create_service(service_entry_);
  }

  // If not initialized, do it :
  if (!(service_entry_.service_status & service_entry::STATUS_INITIALIZED)) {
    if (this->is_debug()) {
      std::clog << datatools::utils::io::debug
                << "datatools::service::service_manager::_initialize_service: "
                << "Initializing service named '"
                << service_entry_.service_name
                << "'..."
                << std::endl;
    }
    
    // 2012/04/14: XG since the service handle has been set by
    // the create_service method, one has to use it. THe
    // following commented lines create another pointer ! so
    // another service is initialized ! Maybe FM has to check
    // what is going on...
    // datatools::service::base_service::factory_register_type::factory_type
    // & the_factory = _factory_register_.get
    // (service_entry_.service_id);

    // base_service * ptr = the_factory ();
    // if (ptr == 0)
    //   {
    //     ostringstream message;
    //     message << "datatools::service::service_manager::_initialize_service: "
    //             << "Cannot initialize service named '"
    //             << service_entry_.service_name << "' !";
    //     throw logic_error (message.str ());
    //   }
    base_service& the_service = service_entry_.service_handle.get();
    the_service.initialize(service_entry_.service_config, _services_);
    service_entry_.service_status |= service_entry::STATUS_INITIALIZED;
  }
}

void service_manager::load(const std::string& service_name_,
                           const std::string& service_id_,
                           const datatools::utils::properties& config_) {
  this->_load_service(service_name_, service_id_, config_);
}


void service_manager::load(const datatools::utils::multi_properties& config_) {
  bool debug = this->is_debug();
  if (debug) {
    std::clog << datatools::utils::io::debug
              << "datatools::service::service_manager::load: "
              << "Entering..."
              << std::endl;
  }

  if (this->is_initialized()) {
    std::ostringstream message;
    message << "manager::load: " 
            << "Service manager is already initialized !";
    throw std::logic_error(message.str());
  }


  namespace du = datatools::utils;
  for (du::multi_properties::entries_ordered_col_t::const_iterator i = config_.ordered_entries().begin();
       i != config_.ordered_entries().end();
       ++i) {
    du::multi_properties::entry* mpe = *i;
    const std::string& service_name = mpe->get_key();
    const std::string& service_id = mpe->get_meta();
    if (debug) {
      std::clog << datatools::utils::io::debug
                << "datatools::service::service_manager::load: "
                << "Configuration for service named '" 
                << service_name << "' "
                << " with ID '" 
                << service_id << "'..."
                << std::endl;
    }
    this->_load_service(service_name, service_id, mpe->get_properties());
  }
}


void service_manager::initialize(const datatools::utils::properties& config_) {
  if (this->is_initialized()) {
    std::ostringstream message;
    message << "datatools::service::service_manager::initialize: "
            << "Manager is already initialized !";
    throw std::logic_error(message.str());
  }

  if (!this->is_debug()) {
    if (config_.has_flag("debug")) {
      this->set_debug(true);
    }
  }

  if (_name_.empty()) {
    if (config_.has_key("name")) {
      _name_ = config_.fetch_string("name");
    }
  }

  if (_description_.empty()) {
    if (config_.has_key("description")) {
      _description_ = config_.fetch_string("description");
    }
  }

  // services :
  {
    typedef std::vector<std::string> CFList;
    typedef CFList::iterator CFListIterator;

    CFList services_configuration_files;
    if (config_.has_key("services.configuration_files")) {
      config_.fetch("services.configuration_files", services_configuration_files);
    }

    for (CFListIterator i = services_configuration_files.begin();
         i < services_configuration_files.end();
         ++i) {
      datatools::utils::fetch_path_with_env(*i);
      datatools::utils::multi_properties mconfig;
      mconfig.read(*i);
      this->load(mconfig);
    }
  }

  _initialized_ = true;
}


void service_manager::initialize() {
  datatools::utils::properties dummy_config;
  this->initialize(dummy_config);
}


void service_manager::reset() {
  if (this->is_debug()) {
    std::clog << datatools::utils::io::debug
              << "datatools::service::service_manager::reset: "
              << "Entering..."
              << std::endl;
  }

  if (!_initialized_) {
    std::ostringstream message;
    message << "datatools::service::service_manager::reset: "
            << "Manager is not initialized !";
    throw std::logic_error(message.str());
  }

  size_t count = _services_.size();
  size_t initial_size = _services_.size();
  while (_services_.size() > 0) {
    for (service_dict_type::iterator it = _services_.begin();
         it != _services_.end();
         ++it) {
      service_entry& the_service_entry = it->second;

      if (the_service_entry.can_be_dropped()) {
        if (this->is_debug()) {
          std::clog << datatools::utils::io::debug
                    << "datatools::service::service_manager::reset: "
                    << "Removing service '" 
                    << the_service_entry.service_name 
                    << "'..."
                    << std::endl;
        }
        this->_reset_service(the_service_entry);
        _services_.erase(it);
        --count;
        break;
      }
    }
    if (count == initial_size) {
      break;
    }
  }

  if (_services_.size() > 0) {
    std::clog << datatools::utils::io::warning
              << "datatools::service::service_manager::reset: "
              << "There are some left services !"
              << std::endl;
  }
  _services_.clear();
  _factory_register_.reset();
  _force_initialization_at_load_ = false;
  _preload_ = true;

  _initialized_ = false;
  if (this->is_debug()) {
    std::clog << datatools::utils::io::debug
              << "datatools::service::service_manager::reset: "
              << "Exiting."
              << std::endl;
  }
}


// Constructor :
service_manager::service_manager(const std::string& name_,
                                 const std::string& description_,
                                 uint32_t flag_)
    : _factory_register_("datatools::service::base_service/service_factory",
                         flag_ & VERBOSE ? 
                         base_service::factory_register_type::verbose : 0) {
  _initialized_ = false;
  this->set_name(name_);
  this->set_description(description_);

  this->set_debug(flag_ & DEBUG);
  _force_initialization_at_load_ = false;

  if (flag_ & FORCE_INITIALIZATION_AT_LOAD) {
    _force_initialization_at_load_ = true;
  }

  bool preload = true;
  if (flag_ & NO_PRELOAD) {
    preload = false;
  }

  this->_set_preload(preload);
}


// Destructor :
service_manager::~service_manager () {
  if (_initialized_) {
    this->reset();
  }
}


/****************
 *   SERVICES   *
 ****************/
bool service_manager::has(const std::string& service_name_) const {
  return _services_.find(service_name_) != _services_.end();
}


// Who needs this (services is an implementation detail....)
const service_dict_type& service_manager::get_services() const {
  return _services_;
}


service_dict_type& service_manager::get_services() {
  return _services_;
}


bool service_manager::can_drop(const std::string& service_name_) {
  service_dict_type::const_iterator found = _services_.find(service_name_);
  if (found == _services_.end()) {
    std::ostringstream message;
    message << "datatools::service::service_manager::can_drop: "
            << "Service '" 
            << service_name_ 
            << "' does not exist !";
    throw std::logic_error(message.str());
  }
  return found->second.can_be_dropped();
}


void service_manager::drop (const std::string& service_name_) {
  service_dict_type::iterator found = _services_.find(service_name_);
  if (found == _services_.end()) {
    std::ostringstream message;
    message << "datatools::service::service_manager::drop_service: "
            << "Service '" 
            << service_name_ 
            << "' does not exist !";
    throw std::logic_error(message.str());
  }

  service_entry& the_service_entry = found->second;
  if (!found->second.can_be_dropped()) {
    std::ostringstream message;
    message << "datatools::service::service_manager::drop_service: "
            << "Service '" 
            << service_name_
            << "' cannot be dropped because of existing dependent services !";
    throw std::logic_error(message.str());
  }

  if (this->is_debug()) {
    std::clog << datatools::utils::io::debug
              << "datatools::service::service_manager::drop: "
              << "Reset & remove service '" 
              << service_name_ 
              << "' !"
              << std::endl;
  }

  this->_reset_service(the_service_entry);
  this->_services_.erase(found);
}


void service_manager::_load_service(const std::string& service_name_,
                                    const std::string& service_id_,
                                    const datatools::utils::properties& service_config_) {
  bool debug = is_debug();
  if (debug) {
    std::clog << datatools::utils::io::debug
              << "datatools::service::service_manager::_load_service: "
              << "Entering..."
              << std::endl;
  }
  if (this->has(service_name_)) {
    std::ostringstream message;
    message << "datatools::service::service_manager::_load_service: "
            << "Service '" 
            << service_name_ 
            << "' already exists !";
    throw std::logic_error(message.str());
  }

  {
    // Add a new entry :
    service_entry dummy_service_entry;
    dummy_service_entry.service_name = service_name_;
    if (debug) {
      std::clog << datatools::utils::io::debug
                << "datatools::service::service_manager::_load_service: "
                << "Add an entry for service '" 
                << service_name_ 
                << "'..."
                << std::endl;
    }
    _services_[service_name_] = dummy_service_entry;
  }
  // fetch a reference on it and update :
  service_entry& the_service_entry = _services_.find(service_name_)->second;
  the_service_entry.service_id     = service_id_;
  the_service_entry.service_config = service_config_;
  the_service_entry.service_status = service_entry::STATUS_BLANK;

  if (debug) {
    std::clog << datatools::utils::io::debug
              << "datatools::service::service_manager::_load_service: "
              << "Fetch..."
              << std::endl;
  }

  this->_create_service(the_service_entry);
  std::vector<std::string> strict_dependencies;

  if (service_config_.has_key("dependencies.strict")) {
    service_config_.fetch("dependencies.strict", strict_dependencies);
  }

  for (size_t i = 0; i < strict_dependencies.size(); ++i) {
    // XXX the_service_entry.service_masters;
  }

  // XXX:
  //the_service_entry.service_handle.get ().fetch_dependencies (the_service_entry.service_masters);

  for (service_dependency_dict_type::const_iterator j
       = the_service_entry.service_masters.begin();
       j != the_service_entry.service_masters.end();
       ++j) {
    const std::string& master_service_name = j->first;
    if (debug) {
      std::clog << datatools::utils::io::debug
                << "datatools::service::service_manager::_load_service: "
                << "Master '" 
                << master_service_name 
                << "' "
                << std::endl;
    }
    continue;
    /*
       const dependency_info_type & master_depinfo = j->second;
    // search for missing strict dependencies from the current dictionnary of services:
    service_dict_type::iterator found = _services_.find (master_service_name);
    if (master_depinfo.level == STRICT_DEPENDENCY)
    {
    cerr << "DEVEL: "
    << "datatools::service::service_manager::_load_service: "
    << "Master '" << master_service_name << "' is a strict dependency !"
    << endl;
    if (found == _services_.end ())
    {
    ostringstream message;
    message << "datatools::service::service_manager::_load_service: "
    << "Could not find service named '" << master_service_name
    << "' on which the new service '" << service_name_ << "' depends !";
    if (is_abort_on_error ())
    {
    throw logic_error (message.str());
    }
    clog << "ERROR: " << message.str () << endl;
    the_service_entry.service_status = service_entry::STATUS_BROKEN_DEPENDENCY;
    return datatools::utils::FAILURE;
    }
    else
    {
    dependency_info_type di;
    di.name = service_name_;
    di.level = STRICT_DEPENDENCY;
    found->second.service_slaves[service_name_] = di;
    }
    }
    else if (master_depinfo.level > NO_DEPENDENCY)
    {
    dependency_info_type di;
    di.name = service_name_;
    di.level = master_depinfo.level;
    found->second.service_slaves[service_name_] = di;
    }
    */
  }
  if (_force_initialization_at_load_) {
    this->_initialize_service(the_service_entry);
  }

  if(debug) {
    std::clog << datatools::utils::io::debug
              << "datatools::service::service_manager::_load_service: "
              << "Exiting."
              << std::endl;
  }
}


void service_manager::dump_services(std::ostream& out_,
                                    const std::string& title_,
                                    const std::string& indent_) const {
  std::string indent;
  if (!indent_.empty()) indent = indent_;

  if (!title_.empty()) out_ << title_ << ":" << std::endl;

  // Services:
  {
    size_t sz = _services_.size();
    size_t count = 0;
    for (service_dict_type::const_iterator it = _services_.begin();
         it != _services_.end();
         ++it) {
      count++;
      out_ << indent;
      if (count == sz) {
        out_ << "`-- ";
      } else {
        out_ << "|-- ";
      }
      const std::string& service_name = it->first;
      const service_entry& the_service_entry = it->second;
      
      out_ << "Name : '" << service_name << "' "
           << "Type : '" << the_service_entry.service_id << "' ";
      out_ << '(';
      int count = 0;
      if (the_service_entry.service_status & service_entry::STATUS_INITIALIZED) {
        out_ << "initialized";
        count++;
      }

      if (the_service_entry.service_status & service_entry::STATUS_BROKEN_DEPENDENCY) {
        if (count>0) out_ << '/',
        out_ << "broken";
        count++;
      }
      out_ << ')';
      out_ << std::endl;
    }
  }
}


void service_manager::_preload_global_dict() {
  bool devel = false;
  if (devel) {
    std::clog << datatools::utils::io::devel
              << "datatools::service::service_manager::_preload_global_dict: "
              << "Entering..."
              << std::endl;
  }

  _factory_register_.import(
      DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(
          datatools::service::base_service));
}


bool service_manager::has_service_type(const std::string& service_id_) const {
  return _factory_register_.has(service_id_);
}


void service_manager::unregister_service_type(const std::string& service_id_) {
  _factory_register_.unregistration(service_id_);
}


void service_manager::tree_dump(std::ostream& out_ ,
                                const std::string& title_,
                                const std::string& indent_,
                                bool inherit_) const {
  namespace du = datatools::utils;
  std::string indent;
  if (!indent_.empty()) indent = indent_;

  if (!title_.empty()) out_ << indent << title_ << std::endl;

  out_ << indent << du::i_tree_dumpable::tag
       << "Name           : '" 
       << _name_ 
       << "'" << std::endl;

  out_ << indent << du::i_tree_dumpable::tag
       << "Description    : '" 
       << _description_ 
       << "'" << std::endl;

  out_ << indent << du::i_tree_dumpable::tag
       << "Debug          : " 
       << _debug_ << std::endl;

  out_ << indent << du::i_tree_dumpable::tag
       << "Preload        : " 
       << _preload_ 
       << std::endl;

  out_ << indent << du::i_tree_dumpable::tag
       << "Force initialization : " 
       << _force_initialization_at_load_ 
       << std::endl;

  out_ << indent << du::i_tree_dumpable::tag
       << "List of registered services : " << std::endl;
  {
    std::ostringstream indent_oss;
    indent_oss << indent << du::i_tree_dumpable::skip_tag;

    _factory_register_.print(out_, indent_oss.str());
  }

  {
    out_ << indent << du::i_tree_dumpable::tag
         << "Services       : ";
    size_t sz = _services_.size();
    if (sz == 0) {
      out_ << "<none>";
    }

    out_ << std::endl;
    for (service_dict_type::const_iterator i = _services_.begin();
         i != _services_.end();
         ++i) {
      const std::string& service_name = i->first;
      const service_entry& service_entry = i->second;
      out_ << indent << du::i_tree_dumpable::skip_tag;

      std::ostringstream indent_oss;
      indent_oss << indent << du::i_tree_dumpable::skip_tag;
      service_dict_type::const_iterator j = i;
      j++;
      if (j == _services_.end()) {
        out_ << du::i_tree_dumpable::last_tag;
        indent_oss << du::i_tree_dumpable::last_skip_tag;
      } else {
        out_ << du::i_tree_dumpable::tag;
        indent_oss << du::i_tree_dumpable::skip_tag;
      }
      out_ << "Service : '" << service_name << "'" << std::endl;
      service_entry.tree_dump(out_, "", indent_oss.str());
    }
  }

  out_ << indent << du::i_tree_dumpable::inherit_tag(inherit_)
       << "Initialized    : " 
       << this->is_initialized() 
       << std::endl;
}

}  // end of namespace service
}  // end of namespace datatools

