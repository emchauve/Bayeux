// -*- mode: c++ ; -*-
/** \file geomtools/address_set.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-08
 * Last modified: 2010-02-08
 *
 * License:
 *
 * Description:
 *   Set of addresses from a geometry ID
 *
 */

#ifndef GEOMTOOLS_ADDRESS_SET_H
#define GEOMTOOLS_ADDRESS_SET_H 1

#include <iostream>
#include <set>

#include <boost/cstdint.hpp>

namespace geomtools {

  /// \brief A set of index representing subaddresses (for geomtools::geom_id)
  class address_set
  {

  public:

    /// \brief Mode to test the belonging of an address to the set
    enum mode_type
    {
      MODE_INVALID = -1, //! Invalid mode
      MODE_NONE    = 0, //!< No candidate value is valid
      MODE_ALL     = 1, //!< All candidate values are valid
      MODE_RANGE   = 2, //!< A candidate value is valid only if it lies in some specific range of values
      MODE_LIST    = 3, //!< A candidate value is valid only if it belongs to a specific list of values
      MODE_DEFAULT = MODE_NONE
    };

  private:

    /// Reset the range informations
    void _reset_range_ ();

    /// Reset the list informations
    void _reset_list_ ();

  public:
    /// Check the validity of the set
    bool is_valid () const;

    /// Invalidate the set
    void invalidate ();

    /// Check the reverse flag
    bool is_reverse () const;

    /// Set the reverse flag
    void set_reverse (bool = true);

    /// Check if the mode is defined
    bool is_mode_none () const;

    /// Check if the mode 'all' is activated
    bool is_mode_all () const;

    /// Check if mode 'range' is used
    bool is_mode_range () const;

    /// Check if mode 'list' is used
    bool is_mode_list () const;

    /// Set the mode 'mode'
    void set_mode_none ();

    /// Set the mode 'all'
    void set_mode_all ();

    /// Set the mode 'range'
    void set_mode_range ();

    /// Set the range ('range' mode only)
    void set_range (uint32_t a_min, uint32_t a_max);

    /// Set the mode 'range' and associated range
    void set_mode_range (uint32_t a_min, uint32_t a_max);

    /// Set the mode 'list'
    void set_mode_list ();

    /// Add an address to the list ('list' mode only)
    void add_to_list (uint32_t a_value);

    /// Check if a value matches the collection of addresses
    bool match (uint32_t a_value) const;

    /// Terminate
    void reset ();

    /// Constructor
    address_set ();

    /// Print operator
    friend std::ostream & operator<< (std::ostream & a_out, const address_set & a_addset);

    /// Reader operator
    friend std::istream & operator>> (std::istream & a_in, address_set & a_addset);

  private:

    int           _mode_;           //!< The mode defining the collection of valid addresses
    uint32_t      _range_min_;      //!< In 'range' mode, the minimum bound of valid addresses
    uint32_t      _range_max_;      //!< In 'range' mode, the maximum bound of valid addresses
    std::set<uint32_t> _addresses_; //!< In 'list' mode, the explicit list of valid addresses
    bool          _reverse_;        //!< In any mode, this flag reverse the matching of the candidate addresses

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_ADDRESS_SET_H
