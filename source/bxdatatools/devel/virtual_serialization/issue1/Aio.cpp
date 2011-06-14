
#include <archives.hpp>
#include <boost/serialization/export.hpp>

#include <Abase.ipp>
#include <Ac1.ipp>
#include <Ac2.ipp>
#include <Ad1.ipp>

BOOST_CLASS_EXPORT_IMPLEMENT(A::base) 
BOOST_CLASS_EXPORT_IMPLEMENT(A::c1) 
BOOST_CLASS_EXPORT_IMPLEMENT(A::c2) 

// if we remove this : 'Bprg0.cxx' does not segfault
// but it breaks the other programs :
BOOST_CLASS_EXPORT_IMPLEMENT(A::d1) 

// end
