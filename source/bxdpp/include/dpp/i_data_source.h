/// \file dpp/i_data_source.h
/* Author(s)     : Xavier Garrido <garrido@lal.in2p3.fr>
 * Creation date : 2011-08-05
 * Last modified : 2013-02-15
 *
 * Copyright (C) 2011-2013 Xavier Garrido <garrido@lal.in2p3.fr>
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
 *   Abstract data source reader class.
 *
 */

#ifndef DPP_I_DATA_SOURCE_H
#define DPP_I_DATA_SOURCE_H 1

// Standard library:
#include <ostream>
#include <string>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/things.h>

// This project:
#include <dpp/dpp_config.h>

namespace dpp {

  /// \brief Data source/reader abstract interface
  class i_data_source
  {
  public:

    static const int64_t UNKNOWN_NUMBER_OF_ENTRIES = -1;

    /// \brief Internal class of the i_data_source interface class
    struct source_record
    {
      static const int STATUS_CLOSED = 0;
      static const int STATUS_OPENED = 1;
      std::string label;
      std::string effective_label;
      int         status;
      bool        processed;

      source_record(const std::string & label_ = "");
      void reset();
    };

    /// Set logging priority
    void set_logging_priority(datatools::logger::priority p_);

    /// Returns logging priority
    datatools::logger::priority get_logging_priority() const;

    virtual bool is_open() const;

    virtual bool is_sequential() const;

    virtual bool is_random() const;

    virtual void set(const std::string & source_label_);

    virtual void open() = 0;

    virtual bool can_load_record(int64_t entry_);

    virtual bool has_next_record() = 0;

    virtual bool load_next_record(datatools::things & event_record_) = 0;

    // Default failing load method :
    virtual bool has_number_of_entries() const;

    virtual int64_t get_number_of_entries() const;

    virtual bool load_record(datatools::things & event_record_, int64_t entry_);

    virtual int64_t get_number_of_metadata() const = 0;

    virtual bool load_metadata(datatools::properties & metadata_, int64_t entry_) = 0;

    virtual void close() = 0;

    virtual void reset() = 0;

    /// Constructor
    i_data_source(datatools::logger::priority priority_ = datatools::logger::PRIO_ERROR);

    /// Constructor
    i_data_source(const std::string & source_label_,
                  datatools::logger::priority priority_ = datatools::logger::PRIO_ERROR);

    /// Destructor:
    virtual ~i_data_source();

  protected:

    void _set_defaults(datatools::logger::priority priority_);

    virtual void _check_next_record() = 0;

    // Default failing load method :
    virtual bool _load_record(datatools::things & event_record_,
                              int64_t entry_);

    // Default failing getter method :
    virtual int64_t _get_number_of_entries() const;

  protected:

    datatools::logger::priority _logging; //!< The logging priority
    bool          _has_next_record;       //!< Next record flag
    source_record _source_record;         //!< Record dynamic informations about the data source

  };

}  // end of namespace dpp

#endif // DPP_I_DATA_SOURCE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
