// base_electromagnetic_field.cc

// Ourselves:
#include <emfield/base_electromagnetic_field.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/service_manager.h>
#include <datatools/utils.h>
// - Bayeux/geomtools:
#include <geomtools/utils.h>

namespace emfield {

  // Factory stuff :
  DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(base_electromagnetic_field,
                                                   "emfield::base_electromagnetic_field/__system__")

  // static
  const char base_electromagnetic_field::ELECTRIC_FIELD_LABEL;
  const char base_electromagnetic_field::MAGNETIC_FIELD_LABEL;

  void base_electromagnetic_field::tree_dump(std::ostream & out_,
                                             const std::string & title_,
                                             const std::string & indent_,
                                             bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty()) indent = indent_;
    if (! title_.empty()) {
      out_ << indent << title_ << std::endl;
    }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Name  : ";
    if (_name_.empty()) {
      out_ << "<none>";
    } else {
      out_ << "'" << _name_ << "'";
    }
    out_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Class identifier : '" << get_class_id() << "'" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Address : " << "[@" << this << "]" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Initialized  : " <<  (is_initialized() ? "Yes": "No") << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Logging priority : '" << datatools::logger::get_priority_label(get_logging_priority()) << "'" << std::endl;

    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Electric field : " <<  (is_electric_field() ? "Yes": "No") << std::endl;
      if (is_electric_field()) {
        out_ << indent << datatools::i_tree_dumpable::skip_tag
             << datatools::i_tree_dumpable::tag
             << "Can be combined : " <<  (electric_field_can_be_combined() ? "Yes": "No") << std::endl;

        out_ << indent << datatools::i_tree_dumpable::skip_tag
             << datatools::i_tree_dumpable::last_tag
             << "Is time-dependent : " <<  (electric_field_is_time_dependent() ? "Yes": "No") << std::endl;
      }
    }

    {
      out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Magnetic field : " <<  (is_magnetic_field() ? "Yes": "No") << std::endl;
      if (is_magnetic_field()) {
        out_ << indent << datatools::i_tree_dumpable::inherit_skip_tag(inherit_)
             << datatools::i_tree_dumpable::tag
             << "Can be combined : " <<  (magnetic_field_can_be_combined() ? "Yes": "No") << std::endl;

        out_ << indent << datatools::i_tree_dumpable::inherit_skip_tag(inherit_)
             << datatools::i_tree_dumpable::last_tag
             << "Is time-dependent : " <<  (magnetic_field_is_time_dependent() ? "Yes": "No") << std::endl;
      }
    }

    return;
  }

  void base_electromagnetic_field::set_name(const std::string & name_)
  {
    _name_ = name_;
    return;
  }

  const std::string & base_electromagnetic_field::get_name() const
  {
    return _name_;
  }

  void base_electromagnetic_field::set_logging_priority(datatools::logger::priority priority_)
  {
    _logging_priority_ = priority_;
    return;
  }

  datatools::logger::priority base_electromagnetic_field::get_logging_priority() const
  {
    return _logging_priority_;
  }

  bool base_electromagnetic_field::is_initialized() const
  {
    return _initialized_;
  }

  void base_electromagnetic_field::_set_initialized(bool initialized_)
  {
    _initialized_ = initialized_;
    return;
  }

  void base_electromagnetic_field::_set_electric_field(bool ef_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Cannot change the electric field flag !");
    _electric_field_ = ef_;
    return;
  }

  bool base_electromagnetic_field::electric_field_can_be_combined() const
  {
    return _electric_field_can_be_combined_;
  }

  bool base_electromagnetic_field::magnetic_field_can_be_combined() const
  {
    return _magnetic_field_can_be_combined_;
  }

  bool base_electromagnetic_field::electric_field_is_time_dependent() const
  {
    return _electric_field_is_time_dependent_;
  }

  bool base_electromagnetic_field::magnetic_field_is_time_dependent() const
  {
    return _magnetic_field_is_time_dependent_;
  }

  void base_electromagnetic_field::_set_electric_field_is_time_dependent(bool td_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Cannot change the electric field traits !");
    _electric_field_is_time_dependent_ = td_;
    return;
  }

  void base_electromagnetic_field::_set_magnetic_field_is_time_dependent(bool td_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Cannot change the magnetic field traits !");
    _magnetic_field_is_time_dependent_ = td_;
    return;
  }

  void base_electromagnetic_field::_set_electric_field_can_be_combined(bool efcbc_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Cannot change the electric field combination flag !");
    _electric_field_can_be_combined_ = efcbc_;
    return;
  }

  void base_electromagnetic_field::_set_magnetic_field(bool mf_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Cannot change the magnetic field flag !");
    _magnetic_field_ = mf_;
    return;
  }

  void base_electromagnetic_field::_set_magnetic_field_can_be_combined(bool mfcbc_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Cannot change the magnetic field combination flag !");
    _magnetic_field_can_be_combined_ = mfcbc_;
    return;
  }

  bool base_electromagnetic_field::is_debug() const
  {
    return get_logging_priority() >= datatools::logger::PRIO_DEBUG;
  }

  void base_electromagnetic_field::set_debug(bool debug_)
  {
    if (debug_) set_logging_priority(datatools::logger::PRIO_DEBUG);
    else        set_logging_priority(datatools::logger::PRIO_FATAL);
    return;
  }

  bool base_electromagnetic_field::is_electric_field() const
  {
    return _electric_field_;
  }

  bool base_electromagnetic_field::is_magnetic_field() const
  {
    return _magnetic_field_;
  }

  void base_electromagnetic_field::_parse_basic_parameters(const datatools::properties & setup_,
                                                           datatools::service_manager & /*service_manager_*/,
                                                           field_dict_type & /*dictionary_*/)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Cannot setup configuration parameters at this stage ! Object is already initialized !");

    if (_name_.empty()) {
      if (setup_.has_flag("name")) {
        set_name(setup_.fetch_string("name"));
      }
    }

    datatools::logger::priority p =
      datatools::logger::extract_logging_configuration(setup_,
                                                       datatools::logger::PRIO_UNDEFINED,
                                                       true);
    if (p != datatools::logger::PRIO_UNDEFINED) {
      set_logging_priority(p);
    }

    if (setup_.has_flag("electric_field.can_be_combined")) {
      _set_electric_field_can_be_combined(true);
    }

    if (setup_.has_flag("magnetic_field.can_be_combined")) {
      _set_magnetic_field_can_be_combined(true);
    }

    return;
  }

  void base_electromagnetic_field::_set_defaults()
  {
    _logging_priority_ = datatools::logger::PRIO_FATAL;
    _electric_field_ = false;
    _magnetic_field_ = false;
    _electric_field_can_be_combined_ = false;
    _magnetic_field_can_be_combined_ = false;
    _electric_field_is_time_dependent_ = false;
    _magnetic_field_is_time_dependent_ = false;
    return;
  }

  // Constructor :
  base_electromagnetic_field::base_electromagnetic_field(uint32_t flags_)
  {
    _initialized_ = false;
    _set_defaults();

    // Special values :
    if (flags_ & DEBUG) set_debug(true);
    _electric_field_ = flags_ & ELECTRIC_FIELD;
    _magnetic_field_ = flags_ & MAGNETIC_FIELD;
    _electric_field_can_be_combined_ = flags_ & ELECTRIC_FIELD_CAN_BE_COMBINED;
    _magnetic_field_can_be_combined_ = flags_ & MAGNETIC_FIELD_CAN_BE_COMBINED;
    _electric_field_is_time_dependent_ = flags_ & ELECTRIC_FIELD_IS_TIME_DEPENDENT;
    _magnetic_field_is_time_dependent_ = flags_ & MAGNETIC_FIELD_IS_TIME_DEPENDENT;
    return;
  }


  // Destructor :
  base_electromagnetic_field::~base_electromagnetic_field()
  {
    if (is_initialized()) {
      DT_LOG_ERROR(datatools::logger::PRIO_ALWAYS,
                   "EM field still has its 'initialized' flag on !" <<
                   "The '::reset' method has not been invoked from the daughter class' destructor !" <<
                   "Possible bug !");
    }
    return;
  }

  bool base_electromagnetic_field::position_and_time_are_valid(const geomtools::vector_3d & position_,
                                                               double time_) const
  {
    return geomtools::is_valid(position_) && datatools::is_valid(time_);
  }


  int base_electromagnetic_field::compute_electromagnetic_field(const geomtools::vector_3d & position_,
                                                                double time_,
                                                                geomtools::vector_3d & electric_field_,
                                                                geomtools::vector_3d & magnetic_field_) const
  {
    geomtools::invalidate(electric_field_);
    geomtools::invalidate(magnetic_field_);
    if (! is_initialized()) {
      return STATUS_NOT_INITIALIZED;
    }

    if (! is_electric_field()) {
      return STATUS_NO_ELECTRIC_FIELD;
    }

    int se = compute_electric_field(position_, time_, electric_field_);
    if (se != STATUS_SUCCESS) return se;

    if (! is_magnetic_field()) {
      return STATUS_NO_MAGNETIC_FIELD;
    }

    int sb = compute_magnetic_field(position_, time_, magnetic_field_);
    return sb;
  }


  int base_electromagnetic_field::compute_electric_field(const geomtools::vector_3d & position_,
                                                         double time_,
                                                         geomtools::vector_3d & electric_field_) const
  {
    geomtools::invalidate(electric_field_);
    if (! is_initialized()) {
      return STATUS_NOT_INITIALIZED;
    }

    if (! is_electric_field()) {
      return STATUS_NO_ELECTRIC_FIELD;
    }
    if (! position_and_time_are_valid(position_, time_)) {
      return STATUS_INVALID_POSITION_TIME;
    }
    electric_field_.set(0., 0., 0.);
    return STATUS_SUCCESS;
  }


  int base_electromagnetic_field::compute_magnetic_field(const geomtools::vector_3d & position_,
                                                         double time_,
                                                         geomtools::vector_3d & magnetic_field_) const
  {
    geomtools::invalidate(magnetic_field_);
    if (! is_initialized()) {
      return STATUS_NOT_INITIALIZED;
    }
    if (! is_magnetic_field()) return STATUS_NO_MAGNETIC_FIELD;
    if (! position_and_time_are_valid(position_, time_)) {
      return STATUS_INVALID_POSITION_TIME;
    }
    magnetic_field_.set(0., 0., 0.);
    return STATUS_SUCCESS;
  }


  int base_electromagnetic_field::compute_field(char what_,
                                                const geomtools::vector_3d & position_,
                                                double time_,
                                                geomtools::vector_3d & field_) const
  {
    /*
      if (! position_and_time_are_valid(position_, time_))
      {
      return invalid_position_time;
      }
    */
    if (what_ == ELECTRIC_FIELD_LABEL) {
      if (! is_electric_field()) {
        return STATUS_NO_ELECTRIC_FIELD;
      }
      return compute_electric_field(position_, time_, field_);
    } else if (what_ == MAGNETIC_FIELD_LABEL) {
      if (! is_magnetic_field()) {
        return STATUS_NO_MAGNETIC_FIELD;
      }
      return compute_magnetic_field(position_, time_, field_);
    }
    geomtools::invalidate(field_);
    return false;
  }

  void base_electromagnetic_field::initialize_simple()
  {
    const datatools::properties dummy;
    initialize_standalone(dummy);
    return;
  }


  void base_electromagnetic_field::initialize_standalone(const datatools::properties & setup_)
  {
    datatools::service_manager dummy_srvcmgr;
    field_dict_type dummy_dict;
    initialize(setup_, dummy_srvcmgr, dummy_dict);
    return;
  }

  void base_electromagnetic_field::initialize_with_dictionary_only(const datatools::properties & setup_,
                                                                   field_dict_type & dictionary_)
  {
    datatools::service_manager dummy_srvcmgr;
    initialize(setup_, dummy_srvcmgr, dictionary_);
    return;
  }

  void base_electromagnetic_field::initialize_with_service_only(const datatools::properties & setup_,
                                                                datatools::service_manager & service_manager_)
  {
    field_dict_type dummy_dict;
    initialize(setup_, service_manager_, dummy_dict);
    return;
  }

} // end of namespace emfield
