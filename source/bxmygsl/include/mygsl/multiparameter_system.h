// mygsl::multiparameter_system.h

#ifndef __mygsl__multiparameter_system_h
#define __mygsl__multiparameter_system_h 1

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <map>
#include <vector>
#include <cmath>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlin.h>

#include <mygsl/unary_eval.h>
#include <mygsl/param_entry.h>

using namespace std;

namespace mygsl {

  class multiparameter_system
    {
    public:

      typedef vector<param_entry *> params_col;

    private:

      bool       __lock_params;
      params_col __params;
      params_col __free_params;
      params_col __auto_params;
      params_col __const_params;

    public:

      bool is_lock_params () const;

      size_t get_number_of_params () const;

      size_t get_number_of_free_params () const;

      size_t get_number_of_auto_params () const;

      size_t get_number_of_const_params () const;

      void init ();

      void reset ();

      void lock_params ();

      void unlock_params ();

      bool has_param_with_name (const string & name_) const;

      const param_entry & get_param_by_name (const string & name_) const;

      param_entry & get_param_by_name (const string & name_);

      const param_entry & get_param (int i_) const;

      param_entry & get_param (int i_);

      const param_entry & get_free_param (int i_) const;

      param_entry & get_free_param (int i_);

      const param_entry & get_auto_param (int i_) const;

      param_entry & get_auto_param (int i_);

      void add_param (const param_entry & pe_, const string & comment_ = "");
      
      multiparameter_system ();

      virtual ~multiparameter_system ();
      
      void print (std::ostream & out_ = clog, 
		  const string & title_ = "Multi-parameter system", 
		  const string & indent_ = "") const;

      void print_line (std::ostream & out_ = clog) const;

      void print_status (std::ostream & out_ = clog) const;

      void load_parameters (const std::string & filename_);

      void load_params (const std::string & filename_)
	{
	  load_parameters (filename_);
	}

      void load_parameters (std::istream & in_ );

      void store_parameters (const std::string & filename_) const;

      void store_params (const std::string & filename_) const
	{
	  store_parameters (filename_);
	}

      void store_parameters (std::ostream & out_) const;

      virtual void compute_automatic_params ();

      /* Any sub-class inherited from the 'multiparameter_system'
       * class must implement the virtual 'compute_automatic_params' 
       * method in order to set valid values to the parameters marked 
       * as 'automatic'.
       * 
       * At least these automatic parameters should invoke
       * the 'set_auto_computed (true)' or the 'set_value (XXX)' methods
       *
       * Example:
       *
       * class my_system : public multiparameter_system::compute_automatic_params ()
       * {
       * public:
       *   void multiparameter_system::compute_automatic_params ()
       *     {
       *       for (int i = 0; i < __auto_params.size (); i++)
       *	 {
       *	   __auto_params[i]->set_value (666.);
       *	   __auto_params[i]->set_auto_computed (true);
       *	 }
       *     }
       * };
       *
       */

    };

  struct multiparameter_system_test : public multiparameter_system
  {
    virtual void compute_automatic_params ()
    {
      for (int i = 0; i < get_number_of_auto_params (); i++)
	{
	  get_auto_param (i).set_value_no_check (666.);
	  get_auto_param (i).set_auto_computed (true);
	}
    }
  };

} // namespace mygsl

#endif // __mygsl__multiparameter_system_h

// end of mygsl::multiparameter_system.h
