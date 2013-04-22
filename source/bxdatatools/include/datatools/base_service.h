/* base_service.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2013-04-22
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *
 * Description:
 *
 *   Base service.
 *
 * History:
 *
 */
#ifndef DATATOOLS_BASE_SERVICE_H_
#define DATATOOLS_BASE_SERVICE_H_

// Standard Library
#include <string>
#include <iostream>

// Third Party
// - Boost
#include <boost/scoped_ptr.hpp>

// Datatools
#include <datatools/i_tree_dump.h>
#include <datatools/service_tools.h>
#include <datatools/factory_macros.h>

namespace datatools {

class properties;

/*! \brief The base service class
 *
 *  The base_service class provides the interface of all
 *  datatools-based service objects.
 */
class base_service : public datatools::i_tree_dumpable {
 public:
  static bool g_debug;

 public:
  /// Constructor
  base_service(const std::string& name, const std::string& description = "",
               const std::string& version = "");

  /// Destructor
  virtual ~base_service();

  /// Get the name of the service
  const std::string& get_name() const;

  /// Check is the description is not empty
  bool has_description() const;

  /// Get the description of the service
  const std::string& get_description() const;

  /// Set the description of the service
  void set_description(const std::string& description);

  /// Check is the version ID is not empty
  bool has_version() const;

  /// Get the version ID of the service
  const std::string& get_version() const;

  /// Set the version ID of the service
  void set_version(const std::string& version);

  /// Undocumented unused/unimplemented method
  virtual void fetch_dependencies(
      service_dependency_dict_type& /*dependencies*/) const;

  /// Check if service is initialized
  virtual bool is_initialized() const = 0;

  /// Initialize the service using only a list of properties without the needs of other services
  virtual int initialize_standalone(
      const datatools::properties& config);

  /// Initialize the service using a list of properties with access to a dictionry of other services
  virtual int initialize(const datatools::properties& config,
                         service_dict_type& service_dict) = 0;

  /// Reset the service
  virtual int reset() = 0;

 protected:

  /// Set the service's name
  void set_name(const std::string& name);

  /// Smart print
  virtual void tree_dump(std::ostream& out = std::clog,
                         const std::string & title = "",
                         const std::string & indent = "",
                         bool inherit = false) const;

 protected:
  std::string name_;         //!< The name of the service
  std::string description_;  //!< The description of the service
  std::string version_;      //!< The version of the service

  // Factory stuff :
  DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(base_service);
};

}  // end of namespace datatools

#endif // DATATOOLS_BASE_SERVICE_H_

