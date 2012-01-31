// -*- mode: c++ ; -*- 
/* cylindric_extrusion_boxed_model.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-02
 * Last modified: 2010-04-02
 * 
 * License: 
 * 
 * Description: 
 *
 *   Geometry model for a box with a central cylindric extrusion.
 * 
 * History: 
 * 
 */

#ifndef __geomtools__cylindric_extrusion_boxed_model_h
#define __geomtools__cylindric_extrusion_boxed_model_h 1

#include <iostream>
#include <string> 
#include <exception>
#include <limits> 

#include <geomtools/i_model.h>
#include <geomtools/model_macros.h>
#include <geomtools/box.h>
#include <geomtools/cylinder.h>
#include <geomtools/subtraction_3d.h>
#include <geomtools/logical_volume.h>
#include <geomtools/placement.h>
#include <geomtools/physical_volume.h>

namespace geomtools {

  using namespace std;

  // define a geometry model:
  class cylindric_extrusion_boxed_model : GEOMTOOLS_MODEL_INHERIT
  {
  public:

    static const string LABEL_PROPERTY_PREFIX;
    static const string EXTRUSION_LABEL;

  public:

    void set_material_name (const string &);

    const string & get_material_name () const;
  
    void set_mother_x (double);

    void set_mother_y (double);

    void set_mother_z (double);

    void set_extrusion_radius (double);

    double get_mother_x () const;
  
    double get_mother_y () const;
  
    double get_mother_z () const;

    double get_extrusion_radius () const;

    const geomtools::i_shape_3d & get_solid () const;


  public:
  
    cylindric_extrusion_boxed_model ();
  
    virtual ~cylindric_extrusion_boxed_model ();

  public:

    virtual string get_model_id () const;

  protected:
  
    virtual void _at_construct (const string & name_,
				const datatools::utils::properties & config_,
				models_col_t * models_ = 0);
     
  public: 

    virtual void tree_dump (ostream & out_         = clog, 
			    const string & title_  = "", 
			    const string & indent_ = "", 
			    bool inherit_          = false) const;

    static void gnuplot_draw_user_function (std::ostream &, 
					    const geomtools::vector_3d &, 
					    const geomtools::rotation_3d &,
					    const geomtools::i_object_3d &, 
					    void * = 0);

  private:

    string           _material_name_;
    double           _mother_x_, _mother_y_, _mother_z_;
    double           _extrusion_radius_;

    geomtools::box            _mother_box_;
    geomtools::cylinder       _extrusion_cylinder_;
    geomtools::subtraction_3d _extruded_solid_;

    logical_volume            _extruded_log_;
    placement                 _extruded_placement_;
    physical_volume           _extruded_phys_;

    geomtools::i_shape_3d * _solid_;

    // registration interface :
    GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(cylindric_extrusion_boxed_model);
   
  };

} // end of namespace geomtools

#endif // __geomtools__cylindric_extrusion_boxed_model_h

// end of cylindric_extrusion_boxed_model.h
