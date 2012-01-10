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

#ifndef __cuts__multi_xor_cut_h
#define __cuts__multi_xor_cut_h 1

#include <string>
#include <list>

#include <cuts/i_cut.h>
#include <cuts/cut_macros.h>

namespace cuts {
  
  using namespace std;

  CUT_CLASS_DECLARE(multi_xor_cut)
  {
  public:

    typedef list<cut_handle_type> cuts_col_t;
    
  public: 
    
    void add_cut (cut_handle_type &);
    
    virtual void set_user_data (void *);
    
  public: 

    // ctor:
    CUT_INTERFACE_CTOR_DTOR (multi_xor_cut);

  private:

    // Macro to automate the registration of the cut :
    CUT_REGISTRATION_INTERFACE(multi_xor_cut);
 
  protected: 
    
    cuts_col_t _cuts_;

  };

} // end of namespace cuts

#endif // __cuts__multi_xor_cut_h

// end of multi_xor_cut.h
