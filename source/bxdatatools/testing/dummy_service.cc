/* dummy_service.cc
 *
 * Copyright (C) 2011-2015 Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#include "dummy_service.h"

namespace datatools {

  /// Auto-registration of this service class in a central service Db
  DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(dummy_service, "datatools::dummy_service")

  const std::string & dummy_service::get_label() const
  {
    return _label_;
  }

  void dummy_service::set_label(const std::string & a_label)
  {
    _label_ = a_label;
    return;
  }

  bool dummy_service::is_initialized() const
  {
    return ! _label_.empty();
  }

  int dummy_service::initialize(const datatools::properties & a_config,
                                 service_dict_type & /*a_service_dict*/)
  {
    if(a_config.has_key("label")) {
      this->set_label(a_config.fetch_string("label"));
    }

    return EXIT_SUCCESS;
  }

  int dummy_service::reset()
  {
    _label_ = "";
    return EXIT_SUCCESS;
  }

  dummy_service::dummy_service()
  {
    _label_ = "";
    return;
  }

  dummy_service::~dummy_service()
  {
    if(dummy_service::is_initialized())
      {
        dummy_service::reset();
      }
    return;
  }

  void dummy_service::tree_dump(std::ostream & a_out ,
                                 const std::string & a_title,
                                 const std::string & a_indent,
                                 bool a_inherit) const
  {
    this->base_service::tree_dump(a_out, a_title, a_indent, true);
    a_out << a_indent << i_tree_dumpable::inherit_tag(a_inherit)
          << "Label : '" << _label_ << "'" << std::endl;

    return;
  }

}  // end of namespace datatools

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
