/* i_data_sink.h
 * Author(s)     : Xavier Garrido <garrido@lal.in2p3.fr>
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
 *   Abstract data sink reader class.
 *
 * History:
 *
 */

#ifndef DPP_I_DATA_SINK_H_
#define DPP_I_DATA_SINK_H_ 1

#include <ostream>
#include <string>

#include <boost/cstdint.hpp>

#include <dpp/dpp_config.h>

#include <datatools/properties.h>
#include <datatools/things.h>

namespace dpp {
  
  /// \brief Data sink/writer abstract interface
  class i_data_sink
  {
  public:
    
    enum flags_type
      {
        blank    = 0x0,
        debug    = 0x1,
        preserve_existing_sink = 0x4,
        defaults = blank,
      };

    /// \brief Internal class of the i_data_sink interface class
    struct sink_record
    {
      static const int STATUS_CLOSED = 0;
      static const int STATUS_OPENED = 1;
      std::string label;
      std::string effective_label;
      int         status;

      sink_record (const std::string & a_label = "");
      void reset ();
    };

    bool is_debug () const;

    int get_debug_level () const;

    void set_debug_level (int);
 
    void set_preserve_existing_sink (bool a_value);

    bool is_preserve_existing_sink () const;

  protected:

    void set_defaults_ (uint32_t a_flags);

  public:

    virtual bool is_open () const;

    virtual bool is_sequential () const;

    virtual bool is_random () const;

    virtual void set (const std::string & a_sink_label);

    virtual void open () = 0;

    virtual bool store_next_record (const datatools::things & a_event_record) = 0;

    virtual void close () = 0;

    virtual void reset () = 0; 

  public:

    // ctor:
    i_data_sink (uint32_t a_flags = 0);

    // ctor:
    i_data_sink (const std::string & a_sink_label, uint32_t a_flags = 0);

    // dtor:
    virtual ~i_data_sink ();

  protected:

    int         _debug_level;
    bool        _preserve_existing_sink;
    sink_record _sink_record;

  };

}  // end of namespace dpp

#endif // DPP_I_DATA_SINK_H_

// end of i_data_sink.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
