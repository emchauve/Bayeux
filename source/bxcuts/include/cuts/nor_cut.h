// -*- mode: c++ ; -*- 
/* nor_cut.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-18
 * Last modified: 2010-09-18
 * 
 * License: 
 * 
 * Description: 
 *   NOR binary cut
 * 
 * History: 
 * 
 */

#ifndef _CUTS_NOR_CUT_H
#define _CUTS_NOR_CUT_H 1

#include <cuts/i_binary_cut.h>

namespace cuts {

  /// \brief The NOR binary cut
  class nor_cut : public i_binary_cut
  {
  
  public: 

    // ctor:
    CUT_INTERFACE_NORESET_CTOR_DTOR (nor_cut);

  private:

    // Macro to automate the registration of the cut :
    CUT_REGISTRATION_INTERFACE(nor_cut);

  };

} // end of namespace cuts

#endif // _CUTS_NOR_CUT_H

// end of nor_cut.h
