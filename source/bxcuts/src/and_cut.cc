// -*- mode: c++ ; -*- 
/* and_cut.cc
 */

#include <cuts/and_cut.h>

#include <stdexcept>
#include <sstream>

#include <datatools/properties.h>

namespace cuts { 

  using namespace std;
  
  // Registration instantiation macro :
  CUT_REGISTRATION_IMPLEMENT(and_cut, "cuts::and_cut");
  
  // ctor:
  and_cut::and_cut (int a_debug_level) 
    : i_binary_cut ("cuts::and_cut",
                    "And cut",
                    "1.0",
                    a_debug_level)
  {
    return;
  }
 
  // dtor:
  CUT_DEFAULT_DESTRUCTOR_IMPLEMENT (and_cut)

  CUT_ACCEPT_IMPLEMENT_HEAD(and_cut)
  {
    int status_1 = _handle_1.get ().process ();
    int status_2 = _handle_2.get ().process ();
    if ((status_1 < 0) || (status_2 < 0))
      {
        return INAPPLICABLE;
      }
    if ((status_1 + status_2) == 2 * ACCEPTED) 
      {
        return (ACCEPTED);
      }
    return (REJECTED);
  }

  CUT_INITIALIZE_IMPLEMENT_HEAD(and_cut,
                                a_configuration,
                                a_service_manager,
                                a_cut_dict)
  {
    using namespace std;
    if (is_initialized ())
      {
        ostringstream message;
        message << "cuts::and_cut::initialize: "
                << "Cut '" << get_name () << "' is already initialized ! ";
        throw logic_error (message.str ());
      }

    this->i_binary_cut::_install_cuts (a_configuration,a_cut_dict);

    _set_initialized (true);
    return;      
  }
  
} // end of namespace cuts

// end of and_cut.cc
