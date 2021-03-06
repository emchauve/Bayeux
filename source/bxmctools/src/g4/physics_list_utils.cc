// physics_list_utils.cc

// Ourselves:
#include <mctools/g4/physics_list_utils.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/exception.h>

// This project:
#include <mctools/g4/base_physics_constructor.h>

namespace mctools {

  namespace g4 {

    std::string get_builder_type_label(G4BuilderType type_)
    {
      if (type_ == bTransportation) return "transportation";
      else if (type_ == bElectromagnetic) return "electromagnetic";
      else if (type_ == bEmExtra) return "electromagnetic_extra";
      else if (type_ == bDecay) return "decay";
      else if (type_ == bHadronElastic) return "hadronic_elastic";
      else if (type_ == bHadronInelastic) return "hadronic_inelastic";
      else if (type_ == bStopping) return "stopping";
      else if (type_ == bIons) return "ions";
      else return "";
    }

    G4BuilderType get_builder_type(const std::string & label_)
    {
      if (label_ == "transportation") return bTransportation;
      else if (label_ == "electromagnetic") return bElectromagnetic;
      else if (label_ == "electromagnetic_extra") return bEmExtra;
      else if (label_ == "decay") return bDecay;
      else if (label_ == "hadronic_elastic") return bHadronElastic;
      else if (label_ == "hadronic_inelastic") return bHadronInelastic;
      else if (label_ == "stopping") return bStopping;
      else if (label_ == "ions") return bIons;
      else return bUnknown;
    }

    // *** physics_constructor_entry *** //

    physics_constructor_entry::physics_constructor_entry()
    {
      name = "";
      id = "";
      status = 0;
      handle.reset(0);
    }

    const base_physics_constructor &
    physics_constructor_entry::get_physics_constructor() const
    {
      DT_THROW_IF(! (status & STATUS_CREATED), std::logic_error,
                  "Physics constructor '" << name << "' with ID '" << id << "' is not created !");
      return handle.get();
    }

    base_physics_constructor &
    physics_constructor_entry::grab_physics_constructor()
    {
      DT_THROW_IF(! (status & STATUS_CREATED), std::logic_error,
                  "Physics constructor '" << name << "' with ID '" << id << "' is not created !");
      return handle.grab();
    }

    // *** physics_constructor_proxy *** //
    physics_constructor_proxy::physics_constructor_proxy(base_physics_constructor & pc_) : G4VPhysicsConstructor()
    {
      pc = &pc_;
      return;
    }

    physics_constructor_proxy::~physics_constructor_proxy()
    {
      pc = 0;
      return;
    }

    void physics_constructor_proxy::ConstructParticle()
    {
      pc->ConstructParticle();
      return;
    }

    void physics_constructor_proxy::ConstructProcess()
    {
      pc->ConstructProcess();
      return;
    }

    // *** physics_list_proxy *** //
    physics_list_proxy::physics_list_proxy(G4VModularPhysicsList & pl_) : G4VModularPhysicsList()
    {
      pl = &pl_;
      return;
    }

    physics_list_proxy::~physics_list_proxy()
    {
      pl = 0;
      return;
    }

    void physics_list_proxy::ConstructParticle()
    {
      pl->ConstructParticle();
      return;
    }

    void physics_list_proxy::ConstructProcess()
    {
      pl->ConstructProcess();
      return;
    }

    void physics_list_proxy::SetCuts()
    {
      pl->SetCuts();
      return;
    }

    // *** user_limits_info *** //

    user_limits_info::user_limits_info()
    {
      reset();
      return;
    }

    void user_limits_info::reset()
    {
      datatools::invalidate(step_max_length);
      datatools::invalidate(track_max_length);
      datatools::invalidate(track_max_time);
      datatools::invalidate(track_min_kinetic_energy);
      datatools::invalidate(track_min_range);
      return;
    }

    void user_limits_info::make_step_limitation(double step_max_length_)
    {
      reset();

      DT_THROW_IF(step_max_length_<= 0.0, std::domain_error,
                  "Invalid step max lenght (" << step_max_length_ << ") !");
      step_max_length = step_max_length_;

      return;
    }

    void user_limits_info::make_track_limitation(double track_max_length_,
                                                 double track_max_time_,
                                                 double track_min_kinetic_energy_,
                                                 double track_min_range_)
    {
      reset();

      DT_THROW_IF(track_max_length_ <= 0.0, std::domain_error,
                  "Invalid track max length (" << track_max_length_ << ") !");
      track_max_length = track_max_length_;

      DT_THROW_IF(track_max_time_<= 0.0, std::domain_error,
                  "Invalid track max time (" << track_max_time_ << ") !");
      track_max_time   = track_max_time_;

      DT_THROW_IF(track_min_kinetic_energy_<= 0.0, std::domain_error,
                  "Invalid track min kinetic energy (" << track_min_kinetic_energy_ << ") !");
      track_min_kinetic_energy = track_min_kinetic_energy_;

      DT_THROW_IF(track_min_range_<= 0.0, std::domain_error,
                  "Invalid track min range (" << track_min_range_ << ") !");
      track_min_range = track_min_range_;

      return;
    }

  } // end of namespace g4

} // end of namespace mctools
