// point_of_interest.cc

// Ourselves:
#include <mctools/biasing/point_of_interest.h>

// Standard library:

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/exception.h>
#include <datatools/properties.h>
// - Bayeux/mygsl:
#include <mygsl/min_max.h>
// - Bayeux/geomtools:
#include <geomtools/manager.h>
#include <geomtools/mapping.h>
#include <geomtools/id_selector.h>
#include <geomtools/bounding_data.h>
#include <geomtools/line_3d.h>
#include <geomtools/utils.h>
#include <geomtools/plane.h>

namespace mctools {

  namespace biasing {

    point_of_interest::point_of_interest()
    {
      _set_default();
      return;
    }

    void point_of_interest::set_name(const std::string & n_)
    {
      _name_ = n_;
      return;
    }

    const std::string & point_of_interest::get_name() const
    {
      return _name_;
    }

    void point_of_interest::set_position(const geomtools::vector_3d & p_)
    {
      _position_ = p_;
      return;
    }

    const geomtools::vector_3d & point_of_interest::get_position() const
    {
      return _position_;
    }

    geomtools::vector_3d & point_of_interest::grab_position()
    {
      return _position_;
    }

    void point_of_interest::set_orientation(const geomtools::vector_3d & o_)
    {
      DT_THROW_IF(o_.mag() < geomtools::constants::get_default_tolerance(), std::logic_error,
                  "Invalid orientation vector!");
      _orientation_ = o_.unit();
      return;
    }

    const geomtools::vector_3d & point_of_interest::get_orientation() const
    {
      return _orientation_;
    }

    void point_of_interest::set_attractivity(double a_)
    {
      _attractivity_ = a_;
      return;
    }

    double point_of_interest::get_attractivity() const
    {
      return _attractivity_;
    }

    void point_of_interest::set_radius(double r_)
    {
      DT_THROW_IF(r_ <= 0.0, std::domain_error,
                  "Invalid radius for point of interest '" << _name_ << "'");
      _radius_ = r_;
      return;
    }

    double point_of_interest::get_radius() const
    {
      return _radius_;
    }

    bool point_of_interest::is_valid() const
    {
      if (_name_.empty()) return false;
      if (_attractive_shape_ == ATTRACTIVE_SHAPE_INVALID) return false;
      if (!geomtools::is_valid(_position_)) return false;
      if (_attractive_shape_ == ATTRACTIVE_SHAPE_DISC && !geomtools::is_valid(_orientation_)) return false;
      if (!datatools::is_valid(_attractivity_)) return false;
      if (!datatools::is_valid(_radius_)) return false;
      return true;
    }

    void point_of_interest::invalidate()
    {
      _name_.clear();
      _set_default();
      return;
    }

    void point_of_interest::_set_default()
    {
      _attractive_shape_ = ATTRACTIVE_SHAPE_INVALID;
      geomtools::invalidate(_position_);
      geomtools::invalidate(_orientation_);
      datatools::invalidate(_attractivity_);
      datatools::invalidate(_radius_);
      return;
    }

    bool point_of_interest::has_attractivity() const
    {
      return datatools::is_valid(_attractivity_);
    }

    bool point_of_interest::is_attractive() const
    {
      DT_THROW_IF(!has_attractivity(), std::logic_error,
                  "Point of interest has no defined attractivity!");
      return _attractivity_ > 0.0;
    }

    bool point_of_interest::is_repulsive() const
    {
      DT_THROW_IF(!has_attractivity(), std::logic_error,
                  "Point of interest has no defined attractivity!");
      return _attractivity_ < 0.0;
    }

    bool point_of_interest::is_neutral() const
    {
      DT_THROW_IF(!has_attractivity(), std::logic_error,
                  "Point of interest has no defined attractivity!");
      return _attractivity_ == 0.0;
    }

    void point_of_interest::initialize(const datatools::properties & config_,
                                       const geomtools::manager * geomgr_)
    {
      geomtools::vector_3d position;
      geomtools::vector_3d orientation;
      double radius;
      double default_length_unit = CLHEP::mm;

      if (_name_.empty()) {
        if (config_.has_key("name")) {
          set_name(config_.fetch_string("name"));
        }
      }

      if (! geomtools::is_valid(_position_)) {
        if (config_.has_key("position")) {
          std::string position_str = config_.fetch_string("position");
          DT_THROW_IF(!geomtools::parse(position_str, position), std::logic_error,
                      "Cannot parse the point of interest's position from '"
                      << position_str << "'!");
          set_position(position);
        }
      }

      if (_attractive_shape_ == ATTRACTIVE_SHAPE_INVALID) {
        if (config_.has_key("attractive_shape")) {
          std::string attractive_shape_label = config_.fetch_string("attractive_shape");
          if (attractive_shape_label == "sphere") {
            _attractive_shape_ = ATTRACTIVE_SHAPE_SPHERE;
          } else if (attractive_shape_label == "disc") {
            _attractive_shape_ = ATTRACTIVE_SHAPE_DISC;
          } else {
            DT_THROW(std::logic_error, "Unsupported attractive shape '" << attractive_shape_label << "'!");
          }
        } else {
          _attractive_shape_ = ATTRACTIVE_SHAPE_SPHERE;
        }
      }
        
      if (_attractive_shape_ == ATTRACTIVE_SHAPE_DISC) {
        if (! geomtools::is_valid(_orientation_)) {
          if (config_.has_key("orientation")) {
            std::string orientation_str = config_.fetch_string("orientation");
            DT_THROW_IF(!geomtools::parse(orientation_str, orientation), std::logic_error,
                        "Cannot parse the point of interest's orientation from '"
                        << orientation_str << "'!");
            set_orientation(orientation);
          }
        }
        if (! geomtools::is_valid(_orientation_)) {
          DT_THROW(std::logic_error, "Missing orientation for disk of interest!");
        }
      }

      if (geomgr_) {
        const geomtools::mapping * geo_mapping = nullptr;
        std::string mapping_plugin_name = "";
        if (config_.has_key("geometry.mapping_plugin")) {
          mapping_plugin_name = config_.fetch_string("geometry.mapping_plugin");
        }
        geo_mapping = &geomgr_->get_mapping(mapping_plugin_name);
        
        if (! geomtools::is_valid(_position_) && ! datatools::is_valid(_radius_)) {
          geomtools::geom_id poi_gid;
          if (config_.has_key("geometry.origin")) {
            std::string geom_id_origin_str = config_.fetch_string("geometry.origin");
            geomtools::id_selector poi_location_selector;
            poi_location_selector.set_id_mgr(geomgr_->get_id_mgr());
            poi_location_selector.initialize(geom_id_origin_str);
            const geomtools::geom_info_dict_type & geom_infos = geo_mapping->get_geom_infos();
            std::list<const geomtools::geom_id *> gids;
            for (geomtools::geom_info_dict_type::const_iterator i
                   = geom_infos.begin ();
                 i != geom_infos.end ();
                 i++) {
              const geomtools::geom_id & gid = i->first;
              if (poi_location_selector.match(gid)) {
                gids.push_back(&gid);
              }
            }
            DT_THROW_IF(gids.size () != 1, std::logic_error,
                        "Cannot find an unique GID from origin rule '" << geom_id_origin_str << "' !");
            poi_gid = *gids.front();
          }

          if (! poi_gid.is_valid()) {
            if (config_.has_key("geometry.id")) {
              std::string geom_id_str = config_.fetch_string("geometry.id");
              std::istringstream gid_iss(geom_id_str);
              geomtools::geom_id gid;
              gid_iss >> gid;
              DT_THROW_IF(!gid_iss, std::logic_error,
                          "Cannot parse geom id from '" << geom_id_str << "'!");
              DT_THROW_IF (!geo_mapping->has_geom_info(gid), std::logic_error,
                           "Cannot find geom Id '" << gid << "' from the geometry mapping!");
              poi_gid = gid;
            }
          }

          if (poi_gid.is_valid()) {
            const geomtools::geom_info & ginfo = geo_mapping->get_geom_info(poi_gid);
            const geomtools::placement & volume_placement = ginfo.get_world_placement();
            set_position(volume_placement.get_translation());
            const geomtools::logical_volume & log_volume = ginfo.get_logical();
            if (log_volume.has_shape()) {
              const geomtools::i_shape_3d & vol_shape = log_volume.get_shape();
              if (vol_shape.has_bounding_data()) {
                const geomtools::bounding_data & bd = vol_shape.get_bounding_data();
                mygsl::min_max mm;
                if (bd.is_cylinder()) {
                  mm.add(hypot(bd.get_rmax(), bd.get_zmin()));
                  mm.add(hypot(bd.get_rmax(), bd.get_zmax()));
                } else {
                  mm.add(geomtools::vector_3d(bd.get_xmin(), bd.get_ymin(), bd.get_zmin()).mag());
                  mm.add(geomtools::vector_3d(bd.get_xmin(), bd.get_ymin(), bd.get_zmax()).mag());
                  mm.add(geomtools::vector_3d(bd.get_xmin(), bd.get_ymax(), bd.get_zmin()).mag());
                  mm.add(geomtools::vector_3d(bd.get_xmin(), bd.get_ymax(), bd.get_zmax()).mag());
                  mm.add(geomtools::vector_3d(bd.get_xmax(), bd.get_ymax(), bd.get_zmin()).mag());
                  mm.add(geomtools::vector_3d(bd.get_xmax(), bd.get_ymax(), bd.get_zmax()).mag());
                  mm.add(geomtools::vector_3d(bd.get_xmax(), bd.get_ymin(), bd.get_zmin()).mag());
                  mm.add(geomtools::vector_3d(bd.get_xmax(), bd.get_ymin(), bd.get_zmax()).mag());
                }
                set_radius(mm.get_max() * 1.05);
              }
            }
          }
          else DT_THROW_IF (true, std::logic_error,
                            "Invalid POI_GID ('" << poi_gid << "'). Abort!");
        }
      }

      if (config_.has_key("radius")) {
        radius = config_.fetch_real("radius");
        DT_THROW_IF(radius <= 0.0, std::domain_error,
                    "Point of interest cannot have negative radius!");
        if (! config_.has_explicit_unit("radius")) {
          radius *= default_length_unit;
        }
        set_radius(radius);
      }
      
      if (! datatools::is_valid(_attractivity_)) {
        if (config_.has_key("attractivity_label")) {
          std::string attractivity_str = config_.fetch_string("attractivity_label");
          if (attractivity_str == "attractive") {
            set_attractivity(+1.0);
          } else if (attractivity_str == "repulsive") {
            set_attractivity(-1.0);
          } else {
            DT_THROW(std::logic_error, "Invalid attractivity label '" <<  attractivity_str << "'!");
          }
        }
      }
      
      if (config_.has_key("attractivity")) {
        double attractivity = config_.fetch_real("attractivity");
        if (has_attractivity()) {
          DT_THROW_IF (attractivity * _attractivity_ < 0.0,
                       std::logic_error,
                       "Incompatible attractivity value with former attractivity status!");
        }
        set_attractivity(attractivity);
      }
       
      if (config_.has_key("skip_check_inside")) {
        _skip_check_inside_ = config_.fetch_boolean("skip_check_inside");
      }
      
      DT_THROW_IF(! has_attractivity(), std::logic_error, "Missing attractivity for PoI '" << _name_ << "'!");

      return;
    }

    bool point_of_interest::hit(const geomtools::vector_3d & source_,
                                const geomtools::vector_3d & direction_) const
    {
      const geomtools::vector_3d & S = source_;
      const geomtools::vector_3d & P = _position_;
      geomtools::vector_3d SP = P - S;
      double dist = SP.mag();
      if (!_skip_check_inside_ and dist < _radius_) {

        if (_attractive_shape_ == ATTRACTIVE_SHAPE_SPHERE) {
          return true;
        }
        if (_attractive_shape_ == ATTRACTIVE_SHAPE_DISC) {
          const geomtools::vector_3d & O = _orientation_;
          geomtools::plane plane_of_the_disk(P,O);
          if (plane_of_the_disk.is_on_surface(S, geomtools::constants::get_default_tolerance())) return true;
        }
        /*
         *                   _.-"""""-._ PoI
         *                 .'           `.
         *                /   ->          \
         *               |    SP   P       |
         *               |  + - ->+        |
         *               | S       \ R     |
         *                \         \     /
         *                 `._       \ _.'
         *                    `-.....-'
         */
      }
      if (SP.dot(direction_) < 0.0) {
        /*
         *                   _.-"""""-._ PoI
         *                 .'           `.
         *                / ->            \
         *       S       |  SP     P       |
         *        + - - -|- - - ->+        |
         *       /       |         \ R     |
         *      / -->     \         \     /
         *     /  dir      `._       \ _.'
         *    L               `-.....-'
         */
        return false;
      }
      // 2019-05-28 FM+RC : to be reviewed:
      double rho = datatools::invalid_real();
      if (_attractive_shape_ == ATTRACTIVE_SHAPE_SPHERE) {
        geomtools::vector_3d L = S + 2 * dist * direction_.unit();
        geomtools::line_3d SL(source_, L);
        /*
         *                   _.-"""""-._ PoI
         *                 .'           `.
         *                / ->            \
         *       S       |  SP     P       |
         *        + - - -|- - - ->+        |
         *         \     |   ..""  \ R     |
         *     -->  \   ..\"" rho   \     /
         *     dir   +""   `._       \ _.'
         *            \       `-.....-'
         *             \
         *              \
         *               \
         *                \
         *                 \
         *                  \
         *                   + L
         */
        rho = SL.get_distance_to_line(P);
      } else {
        // ATTRACTIVE_SHAPE_DISC
        const geomtools::vector_3d & O = _orientation_;
        geomtools::plane plane_of_the_disk(P,O);
        geomtools::vector_3d L = plane_of_the_disk.projection(S, direction_);
        geomtools::vector_3d LP = P - L;
        rho = LP.mag();
      }
      if (rho < _radius_) {
        return true;
      }
      return false;
    }

    void point_of_interest::tree_dump(std::ostream & out_,
                                      const std::string & title_,
                                      const std::string & indent_,
                                      bool inherit_) const
    {
      if (! title_.empty()) {
        out_ << indent_ << title_ << std::endl;
      }

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Name : '" << _name_ << "'" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Position : " << _position_ / CLHEP::mm << " mm" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Attractive shape : ";
      if (_attractive_shape_ == ATTRACTIVE_SHAPE_INVALID) {
        out_ << "<none>";
      } else if (_attractive_shape_ == ATTRACTIVE_SHAPE_SPHERE) {
        out_ << "sphere";
      } else if (_attractive_shape_ == ATTRACTIVE_SHAPE_DISC) {
        out_ << "disc";
      }
      out_ << std::endl;
      
      if (_attractive_shape_ == ATTRACTIVE_SHAPE_DISC) {
        out_ << indent_ << datatools::i_tree_dumpable::tag
             << "Orientation : " << _orientation_ / CLHEP::mm << " mm" << std::endl;
      }
      
      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Radius : " << _radius_ / CLHEP::mm << " mm" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Attractivity : " << _attractivity_ << " ("
           << (is_attractive() ? "attractive" : (is_repulsive() ? "repulsive": "neutral")) << ')'
           << std::endl;

      return;
    }

  } // namespace g4

} // namespace mctools
