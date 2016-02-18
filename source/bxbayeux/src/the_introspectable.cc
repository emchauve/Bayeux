// the_introspectable.cc

// Ourselves:
#include <bayeux/bayeux_config.h>

// Standard Library:
#include <iostream>

// Third Party:
// - Boost:
#include <boost/scoped_ptr.hpp>

// Load the link guard definition :
#include <datatools/detail/reflection_link_guard.h>
#include <datatools/the_introspectable.h>

// Load the link guard implementation :
namespace datatools {
  namespace detail {
    namespace reflection {
      dynamic_link_guard::dynamic_link_guard()
      {
        return;
      }
      dynamic_link_guard::~dynamic_link_guard()
      {
        return;
      }
      dynamic_link_guard& dynamic_link_guard::instance()
      {
        static boost::scoped_ptr<dynamic_link_guard> guard (0);
        if ( guard.get () == 0) {
          guard.reset (new dynamic_link_guard);
        }
        return *guard.get ();
      }

    } // end namespace reflection
  } // end namespace detail
} // end namespace datatools

#if BAYEUX_WITH_GEOMTOOLS == 1
#include <geomtools/detail/reflection_link_guard.h>
#include <geomtools/the_introspectable.h>

// Load the link guard implementation :
namespace geomtools {
  namespace detail {
    namespace reflection {
      dynamic_link_guard::dynamic_link_guard()
      {
        return;
      }
      dynamic_link_guard::~dynamic_link_guard()
      {
        return;
      }
      dynamic_link_guard& dynamic_link_guard::instance()
      {
        static boost::scoped_ptr<dynamic_link_guard> guard (0);
        if ( guard.get () == 0) {
          guard.reset (new dynamic_link_guard);
        }
        return *guard.get ();
      }

    } // end namespace reflection
  } // end namespace detail
} // end namespace geomtools

#endif //  BAYEUX_WITH_GEOMTOOLS == 1

#if BAYEUX_WITH_GENBB_HELP == 1
#include <genbb_help/detail/reflection_link_guard.h>
#include <genbb_help/the_introspectable.h>

// Load the link guard implementation :
namespace genbb {
  namespace detail {
    namespace reflection {
      dynamic_link_guard::dynamic_link_guard()
      {
        return;
      }
      dynamic_link_guard::~dynamic_link_guard()
      {
        return;
      }
      dynamic_link_guard& dynamic_link_guard::instance()
      {
        static boost::scoped_ptr<dynamic_link_guard> guard (0);
        if ( guard.get () == 0) {
          guard.reset (new dynamic_link_guard);
        }
        return *guard.get ();
      }

    } // end namespace reflection
  } // end namespace detail
} // end namespace genbb

#endif // BAYEUX_WITH_GENBB_HELP == 1

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
