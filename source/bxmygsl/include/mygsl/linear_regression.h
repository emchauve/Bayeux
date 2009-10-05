// mygsl::linear_regression.h

#ifndef __mygsl__linear_regression_h 
#define __mygsl__linear_regression_h 1

#include <gsl/gsl_errno.h>
#include <gsl/gsl_fit.h>

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>

#include <vector>
#include <mygsl/unary_eval.h>
#include <mygsl/datapoint.h>

namespace mygsl {
 
  class linear_regression : public unary_eval
    {
      class fit_data
	{
	public:
	  int    status; // 0 == fit results; no fit otherwise
	  size_t n;
	  bool   weighted;
	  bool   with_constant;
	  double c0;
	  double c1;
	  double cov00;
	  double cov01;
	  double cov11;
	  double sumsq;
	  double chisq;

	public:
	  fit_data ();
	  bool is_valid () const;
	  bool is_weighted () const;
	  bool has_constant () const;
	  void reset ();
	  double get_slope () const;
	  double get_constant () const;
	  size_t get_ndof () const;
	  double get_chisq () const;
	  double get_sumsq () const;
	  double get_slope_err () const;
	  double get_constant_err () const;
	};
      
    private:

      bool        __initialized;
      fit_data    __fit_data;
      bool        __delete;
      double    * __x;
      double    * __y;
      double    * __w;
      bool        __can_weighted;

    protected:

      double eval (double x_) const;

    public:

      void eval_err (double x_, double & y_, double & yerr_) const;

      bool is_initialized () const;

      bool can_weighted () const;

      const fit_data & get_fit_data () const;

      linear_regression ();

      linear_regression (const vector<datapoint> & p_);

      linear_regression (size_t npoints_, 
			 const double * x_, 
			 const double * y_, 
			 const double * w_ = 0);

      linear_regression (const vector<double> & x_, 
			 const vector<double> & y_, 
			 const vector<double> & w_);

      linear_regression (const vector<double> & x_, 
			 const vector<double> & y_);
      
      virtual ~linear_regression ();

      void init (const vector<datapoint> & p_);

      void init (size_t npoints_, 
		 const double * x_, 
		 const double * y_, 
		 const double * w_ = 0);

      void init (const vector<double> & x_, 
		 const vector<double> & y_, 
		 const vector<double> & w_);

      void init (const vector<double> & x_, 
		 const vector<double> & y_);

      void reset ();

      bool fit_linear ();

      bool fit_weighted_linear ();

      bool fit_linear_no_constant ();

      bool fit_weighted_linear_no_constant ();

    };

}

#endif // __mygsl__linear_regression_h

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */

// end of mygsl::linear_regression.h
