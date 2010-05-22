// -*- mode: c++ ; -*- 
/* geom_info.cc
 */

#include <geomtools/geom_info.h>

namespace geomtools {

  using namespace std;

  const datatools::utils::properties & 
  geom_info::get_properties () const
  {
    return __properties;
  }

  datatools::utils::properties & 
  geom_info::get_properties ()
  {
    return __properties;
  }
  
  const placement & geom_info::get_world_placement () const
  {
    return __world_placement;
  }
  
  const logical_volume & geom_info::get_logical () const
  {
    return *__logical;
  }
  
  const geom_id & geom_info::get_id () const
  {
    return __id;
  }
  
  bool geom_info::has_logical () const
  {
    return __logical != 0;
  }
     
  geom_info::geom_info ()
  {
    __id.invalidate ();
    __world_placement.set (0., 0., 0., 0., 0., 0.);
    __logical = 0;
  }
   
  geom_info::geom_info (const geom_id & id_)
  {
    __id = id_;
    __world_placement.set (0., 0., 0., 0., 0., 0.);
    __logical = 0;
  }
    
  geom_info::geom_info (const geom_id & id_, 
			const placement & wp_,
			const logical_volume & l_)
  {
    __id = id_;
    __world_placement = wp_;
    __logical = &l_;
  }


  ostream & operator<< (ostream & out_, const geom_info & gi_)
  {
    out_ << "<" << gi_.__id << " @ "
	 << "{" << gi_.__world_placement << "}"
	 << " : " << gi_.__logical << " >";
    return out_;
  }

} // end of namespace geomtools

// end of geom_info.cc
