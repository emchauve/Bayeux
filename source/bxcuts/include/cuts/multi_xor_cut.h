// -*- mode: c++ ; -*- 
/* multi_xor_cut.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-18
 * Last modified: 2010-09-18
 * 
 * License: 
 * 
 * Description: 
 *   A abstract cut interface that combines two other cuts
 * 
 * History: 
 * 
 */

#ifndef _CUTS_MULTI_XOR_CUT_H
#define _CUTS_MULTI_XOR_CUT_H 1

#include <string>
#include <list>

#include <cuts/i_cut.h>
#include <cuts/cut_macros.h>

namespace cuts {

  //CUT_CLASS_DECLARE(multi_xor_cut)
  /// \brief A multi XOR cut
  class multi_xor_cut : public i_cut
  {
  public:

    typedef std::list<cut_handle_type> cuts_col_t;
    
  public: 
    
    void add_cut (cut_handle_type &);
    
    virtual void set_user_data (void *);
   
  public: 

    CUT_INTERFACE_CTOR_DTOR (multi_xor_cut);
 
  protected: 
    
    cuts_col_t _cuts_;
 
    // Macro to automate the registration of the cut :
    CUT_REGISTRATION_INTERFACE(multi_xor_cut);

  };

} // end of namespace cuts

#endif // _CUTS_MULTI_XOR_CUT_H

// end of multi_xor_cut.h
