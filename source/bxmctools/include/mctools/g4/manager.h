/// \file mctools/g4/manager.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-05-04
 * Last modified: 2013-06-04
 *
 * License:
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Description:
 *
 *   G4 simulation manager
 *
 * History:
 *
 */

#ifndef MCTOOLS_G4_MANAGER_H
#define MCTOOLS_G4_MANAGER_H 1

// Standard library:
#include <string>
#include <map>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools :
#include <datatools/time_tools.h>
#include <datatools/logger.h>
// - Bayeux/mygsl :
#include <mygsl/rng.h>
#include <mygsl/seed_manager.h>
#include <mygsl/prng_state_manager.h>
// - Bayeux/geomtools :
#include <geomtools/manager.h>
// - Bayeux/genvtx :
#include <genvtx/manager.h>
// - Bayeux/genbb_help :
#include <genbb_help/manager.h>

// This project:
#include <mctools/g4/loggable_support.h>

namespace genvtx {
  class manager;
  class i_vertex_generator;
}
namespace genbb {
  class manager;
  class i_genbb;
}

#include <mctools/mctools_config.h>
#include <mctools/g4/g4_prng.h>
#include <mctools/g4/track_history.h>

#ifdef G4VIS_USE
class G4VisManager;
#endif // G4VIS_USE

class G4VSteppingVerbose;
class G4RunManager;
class G4UImanager;

namespace datatools {
  class multi_properties;
  class service_manager;
}

namespace mctools {

  /// Nested namespace of the Bayeux/mctools module library's Geant4 pluggin
  namespace g4 {

    class run_action;
    class event_action;
    class detector_construction;
    class primary_generator;
    class physics_list;
    class tracking_action;
    class stepping_action;
    class stacking_action;
    class simulation_ctrl;

    /// \brief The Geant4 simulation manager
    class manager : public loggable_support
    {
    public:

      // 2012-04-24 : limit the maximum alowed number of events:
      // http://hypernews.slac.stanford.edu/HyperNews/geant4/get/particles/528/1.html
      static const uint32_t NUMBER_OF_EVENTS_UPPER_LIMIT = 1000000000; /// Maximum number of events to be processed
      static const uint32_t NUMBER_OF_EVENTS_LOWER_LIMIT = 1; /// Minimum number of events to be processed
      static const uint32_t NUMBER_OF_EVENTS_WARNING_LIMIT = 1000000; /// Number of events that triggers an alarm
      static const std::string DEFAULT_PRNG_ID; /// Default safe PRNG identifier (see mygsl::rng class)

      /// \brief A set of constants used by the Geant4 manager
      struct constants
      {
        uint32_t    NO_LIMIT; /// Constant for unlimited number of simulated events
        std::string G4_MANAGER_LABEL; /// Label associated to the Geant4 manager's PRNG
        std::string VERTEX_GENERATOR_LABEL; /// Label associated to the vertex generator's PRNG
        std::string EVENT_GENERATOR_LABEL; /// Label associated to the event generator's PRNG
        std::string SHPF_LABEL; /// Label associated to the Step Hit Processor Factory's PRNG
        int32_t     DEFAULT_PRNG_STATES_SAVE_MODULO; /// Default PRNG internal state backup rate
        std::string DEFAULT_PRNG_STATES_FILE; /// Default PRNG internal state back filename

        constants ();

        static const constants & instance ();
      };

      typedef datatools::computing_time      CT_type;
      typedef std::map<std::string, CT_type> CT_map;

      /// Check if an external thread simulation control is plugged
      bool has_simulation_ctrl () const;

      /// Plug an external thread simulation control
      void set_simulation_ctrl (simulation_ctrl & a_simulation_ctrl);

      /// Unplug an external thread simulation control
      void reset_simulation_ctrl ();

      /// Return a non-mutable thread simulation control reference
      const simulation_ctrl & get_simulation_ctrl () const;

      /// Return a mutable thread simulation control reference
      simulation_ctrl & grab_simulation_ctrl ();

      /// Check is an external service manager plugged
      bool has_service_manager () const;

      /// Return a mutable service manager reference
      datatools::service_manager & grab_service_manager();

      /// Return a non-mutable service manager reference
      const datatools::service_manager & get_service_manager() const;

      /// Plug an external service manager
      void set_service_manager (datatools::service_manager & smgr_);

      /// Check if an external service manager is plugged
      bool has_external_geom_manager () const;

      /// Plug an external geometry manager
      void set_external_geom_manager (const geomtools::manager & a_geometry_manager);

      /// Return a mutable event_action reference
      event_action & grab_user_event_action ();

      bool has_number_of_events_modulo () const;

      void set_number_of_events_modulo (int);

      int get_number_of_events_modulo () const;

      void set_use_run_header_footer (bool a_use_run_header_footer);

      bool using_run_header_footer () const;

      void set_prng_state_save_modulo (int a_modulo);

      int get_prng_state_save_modulo () const;

      bool has_prng_state_save_modulo () const;

      bool using_time_stat () const;

      void set_using_time_stat (bool);

      bool forbids_private_hits () const;

      void set_dont_save_no_sensitive_hit_events (bool a_dont_save);

      bool dont_save_no_sensitive_hit_events () const;

      void set_forbid_private_hits (bool a_forbid);

      const CT_map & get_CT_map () const;

      CT_map & grab_CT_map ();

      bool is_initialized () const;

      bool is_automatic () const;

      bool is_interactive () const;

      bool is_batch () const;

      void set_interactive (bool);

      bool using_event_number_as_seed () const;

      /*** input seeds file ***/

      bool has_input_prng_seeds_file () const;

      void reset_input_prng_seeds_file ();

      void set_input_prng_seeds_file (const std::string &);

      const std::string & get_input_prng_seeds_file () const;

      /*** output seeds file ***/

      bool has_output_prng_seeds_file () const;

      void reset_output_prng_seeds_file ();

      void set_output_prng_seeds_file (const std::string &);

      const std::string & get_output_prng_seeds_file () const;

      /*** output PRNG states file ***/

      void reset_output_prng_states_file ();

      bool has_output_prng_states_file () const;

      void set_output_prng_states_file (const std::string &);

      const std::string & get_output_prng_states_file () const;

      /*** input PRNG states file ***/

      void reset_input_prng_states_file ();

      bool has_input_prng_states_file () const;

      void set_input_prng_states_file (const std::string &);

      const std::string & get_input_prng_states_file () const;

      /*** the seed of the G4 main random engine ***/

      bool has_mgr_prng_seed () const;

      int get_mgr_prng_seed () const;

      void set_mgr_prng_seed (int rseed_);

      /*** the output data file name ***/

      void set_output_data_file (const std::string &);

      /*** the number of events to be generated ***/

      void set_number_of_events (uint32_t);

      uint32_t get_number_of_events () const;

      /*** the G4 visualization flag ***/

      void set_g4_visualization (bool);

      bool has_g4_visualization () const;

      /*** the G4 macro ***/

      void set_g4_macro (const std::string & g4_macro_);

      const std::string & get_g4_macro () const;

      bool has_g4_macro () const;

      /*** PRNG management ***/

      mygsl::rng & grab_vg_prng ();

      mygsl::rng & grab_eg_prng ();

      mygsl::rng & grab_shpf_prng ();

      const mygsl::rng & get_mgr_prng () const;

      mygsl::rng & grab_mgr_prng ();

      const g4_prng & get_g4_prng () const;

      g4_prng & grab_g4_prng ();

      const mygsl::seed_manager & get_seed_manager () const;

      mygsl::seed_manager & grab_seed_manager ();

      const mygsl::prng_state_manager & get_state_manager () const;

      mygsl::prng_state_manager & grab_state_manager ();

      void record_current_prng_states ();

      void set_event_generator_name (const std::string &);

      void set_event_generator_seed (int);

      void set_vertex_generator_name (const std::string &);

      void set_vertex_generator_seed (int);

      void set_shpf_prng_seed (int);

      /*** Track history management ***/

      /// (De)Activate the track history object
      void set_use_track_history (const bool);

      /// Check if a track history object is available
      bool has_track_history () const;

      /// Get a non-mutable reference to the track history object
      const track_history & get_track_history () const;

      /// Get a mutable reference to the track history object
      track_history & grab_track_history ();

      /// Get a non-mutable reference to the geometry manager
      const geomtools::manager & get_geom_manager () const;

      /*** Vertex/Event generation ***/

      /// Get a non-mutable reference to the vertex generator manager
      const genvtx::manager & get_vg_manager () const;

      /// Get a mutable reference to the vertex generator manager
      genvtx::manager & grab_vg_manager ();

      /// Check if a vertex generator is available
      bool has_vertex_generator () const;

      /// Return a non-mutable reference to a embeded vertex generator
      const genvtx::i_vertex_generator & get_vertex_generator () const;

      /// Get a non-mutable reference to the embeded event generator
      const genbb::manager & get_eg_manager () const;

      /// Get a mutable reference to the embeded event generator
      genbb::manager & grab_eg_manager ();

      /// Check if an event generator is available
      bool has_event_generator () const;

      /// Return a non-mutable reference to a embeded event generator
      const genbb::i_genbb & get_event_generator () const;

      /// Basic default print
      void dump (std::ostream & = std::clog) const;

      /// Basic print
      void dump_base (std::ostream & out_ = std::clog,
                      const std::string & title_ = "",
                      const std::string & indent_ = "") const;

      /// Constructor:
      manager ();

      /// Destructor
      virtual ~manager ();

      /// Initialize the Geant4 manager
      void initialize (const datatools::multi_properties & mp_);

      /// Reset the Geant4 manager
      void reset ();

      /// Run the simulation session
      void run_simulation ();

    protected:

      void _init_defaults ();

      virtual void _init_core ();

      virtual void _init_geometry ();

      virtual void _init_vertex_generator ();

      virtual void _init_event_generator ();

      virtual void _init_detector_construction ();

      virtual void _init_physics_list ();

      virtual void _init_run_action ();

      virtual void _init_event_action ();

      virtual void _init_primary_generator_action ();

      virtual void _init_tracking_action ();

      virtual void _init_stepping_action ();

      virtual void _init_stacking_action ();

      void _init_seeds ();

      void _init_prngs_states ();

      virtual void _init_prngs ();

      virtual void _init_time_stat ();

      virtual void _at_init ();

      virtual void _at_reset ();

      virtual void _at_run_simulation ();

    private:

      // Controls:
      bool _initialized_; /// Initializion flag

      // Configuration:
      const datatools::multi_properties * _multi_config_;   /// Setup parameters

      // User interface mode:
      bool _interactive_; /// Flag for interactive session
      bool _g4_visualization_; /// Flag to activate Geant4 visualization
      bool _use_event_number_as_seed_; // not used

      std::string       _simulation_ctrl_label_; /// Label for simulation thread control
      simulation_ctrl * _simulation_ctrl_; /// Simulation thread control instance

      // Service manager :
      datatools::service_manager * _service_manager_; /// Service manager

      // Geometry manager :
      const geomtools::manager  *  _external_geom_manager_; /// External geometry manager
      geomtools::manager           _geom_manager_; /// Embeded geometry manager

      // Vertex generation manager :
      int                          _vg_prng_seed_; /// Seed for the embeded PRNG for vertex generation
      mygsl::rng                   _vg_prng_; /// Embeded PRNG for vertex generation
      genvtx::manager              _vg_manager_; /// Vertex generator manager
      std::string                  _vg_name_; /// Name of the active vertex generator
      genvtx::i_vertex_generator * _vertex_generator_; /// Active vertex generator

      // Event generation manager :
      int              _eg_prng_seed_; /// Seed for the embeded PRNG for event generation
      mygsl::rng       _eg_prng_; /// Embeded PRNG for event generation
      genbb::manager   _eg_manager_; /// Event generator manager
      std::string      _eg_name_; /// Name of the active event generator
      genbb::i_genbb * _event_generator_; /// Active event generator

      // Step hit processor factory PRNG :
      int              _shpf_prng_seed_; /// Seed for the embeded PRNG for step hit processor factories
      mygsl::rng       _shpf_prng_;      /// Embeded PRNG for step hit processor factory

      // Main Geant4 PRNG :
      int         _mgr_prng_seed_; /// Initial seed of the embeded PRNG
      mygsl::rng  _mgr_prng_; /// Embeded PRNG as the Geant4 main PRNG
      g4_prng     _g4_prng_; /// PRNG using the Geant4 interface

      // G4 objects:
      G4VSteppingVerbose * _g4_stepping_verbosity_; /// Geant4 stepping verbosity instance
      G4RunManager       * _g4_run_manager_;  /// Geant4 run manager
      G4UImanager        * _g4_UI_; /// Geant4 UI manager

      // User specified G4 interfaces :
      mctools::g4::detector_construction * _user_detector_construction_;
      mctools::g4::physics_list          * _user_physics_list_;
      mctools::g4::primary_generator     * _user_primary_generator_;
      mctools::g4::run_action            * _user_run_action_;
      mctools::g4::event_action          * _user_event_action_;
      mctools::g4::tracking_action       * _user_tracking_action_;
      mctools::g4::stepping_action       * _user_stepping_action_;
      mctools::g4::stacking_action       * _user_stacking_action_;

#ifdef G4VIS_USE
      // G4 visualization, if you choose to have it!
      G4VisManager * _g4_vis_manager_; /// Geant4 visualization manager
#endif // G4VIS_USE


      // A PRNG seed manager :
      mygsl::prng_seed_manager  _seed_manager_; /// PRNGs' seed manager

      // A PRNG state manager :
      mygsl::prng_state_manager _prng_state_manager_; /// Manager for PRNGs' internal states
      int                       _prng_state_save_modulo_; /// Event number modulo to backup PRNGs' internal states

      // Track historical infos :
      bool          _use_track_history_; /// Flag to activate track history
      track_history _track_history_; /// Track history data structure

      // User:
      std::string _input_prng_seeds_file_; /// Input filename for loading of PRNG seeds
      std::string _output_prng_seeds_file_; /// Output filename for storage of PRNG seeds
      std::string _input_prng_states_file_; /// Input filename for loading of PRNG internal states
      std::string _output_prng_states_file_; /// Output filename for storage of PRNG internal states
      std::string _output_data_file_; /// Output data filename
      uint32_t    _number_of_events_; /// Number of events to be processed
      int         _number_of_events_modulo_; /// Event number modulo for progression print
      std::string _g4_macro_; /// Geant4 macro to be processed
      int         _g4_tracking_verbosity_; /// Geant 4 tracking verbosity
      bool        _forbid_private_hits_; /// Flag to disable the storage of MC true hits from 'private' collection of hits
      bool        _dont_save_no_sensitive_hit_events_; /// Flag to store MC true hits from 'non-sensitive' volumes
      bool        _use_run_header_footer_; /// Store run header/footer in output file
      bool        _use_time_stat_; /// Flag to activate CPU time statistics
      CT_map      _CTs_; /// CPU time statistics

    };

  } // end of namespace g4

} // end of namespace mctools

/***************************
 * OCD support : interface *
 ***************************/

#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(mctools::g4::manager)

#endif // MCTOOLS_G4_MANAGER_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
