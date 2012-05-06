// -*- mode: c++; -*-
/* datatools::serialization::link_guard.h */

#ifndef __datatools__serialization__link_guard_h
#define __datatools__serialization__link_guard_h 1

namespace datatools {
  namespace serialization {
    class dynamic_link_guard
    {
    public:
      dynamic_link_guard ();
      ~dynamic_link_guard ();
      static dynamic_link_guard & instance ();
    private:
      static bool _g_devel_;
    };
  } // end namespace serialization
} // end namespace datatools

#endif // __datatools__serialization__link_guard_h

/* end of datatools::serialization::link_guard.h */
