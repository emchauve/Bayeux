
#include <sstream>
#include <stdexcept>
#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/decay0/Bi210.h>
#include <genbb_help/primary_event.h>
#include <genbb_help/decay0/alpha.h>
#include <genbb_help/decay0/gamma.h>
#include <genbb_help/decay0/electron.h>
#include <genbb_help/decay0/positron.h>
#include <genbb_help/decay0/particle.h>
#include <genbb_help/decay0/pair.h>
#include <genbb_help/decay0/nucltransK.h>
#include <genbb_help/decay0/nucltransKL.h>
#include <genbb_help/decay0/nucltransKLM.h>
#include <genbb_help/decay0/nucltransKLM_Pb.h>
#include <genbb_help/decay0/beta.h>
#include <genbb_help/decay0/beta1.h>
#include <genbb_help/decay0/beta2.h>
#include <genbb_help/decay0/beta_1fu.h>
#include <genbb_help/decay0/PbAtShell.h>

namespace genbb {
  namespace decay0 {

    // Bi210.f 
    // This file was extracted from the 'decay0' program by V.I. Tretyak
    // Copyright 1995-2011 V.I. Tretyak
    // This program is free software
    // it under the terms of the GNU General Public License as published by
    // the Free Software Foundation
    // your option) any later version.
    // 
    // This program is distributed in the hope that it will be useful, but
    // WITHOUT ANY WARRANTY
    // MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    // General Public License for more details.
    // 
    // You should have received a copy of the GNU General Public License
    // along with this program
    // Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
    // 

    void Bi210(mygsl::rng & prng_, genbb::primary_event & event_, double tcnuc, double & tdnuc)
    {
      double t;
      double tdlev;
      double palfa;
      double pdecay;
      double tclev;
      double thlev;
      double thnuc;
      // Scheme of Bi210 decay ("Table of Isotopes", 7th ed., 1978).
      // Three-figured labels correspond to energies of 206Tl excited
      // levels in keV.
      // Input : tcnuc - time of creation of nucleus (sec)
      // Output: tdnuc - time of decay of nucleus (sec)
      // // common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),// ptime(100).
      // VIT, 14.08.1992, 22.10.1995
      // Update to NDS 99(2003)949 and empirical correction to the beta shape, VIT, 28.10.2006.
      thnuc=433036.8;
      tdnuc=tcnuc-thnuc/std::log(2.)*std::log(prng_());
      tclev=0.;
      pdecay=100.*prng_();
      if (pdecay <= 1.32e-4) goto label_10000;  // 0.000132% alfa to 206Tl
      goto label_20000;  // 99.999868% beta to 210Po
    label_10000  :
      palfa=100.*prng_();
      if (palfa <= 60.) goto label_304;  // 60%
      goto label_265;  // 40%
    label_304  :
      decay0_alpha(prng_, event_, 4.656,0.,0.,t);
      thlev=4.e-12;
      decay0_nucltransK(prng_, event_, 0.304,0.086,3.9e-1,0.,tclev,thlev,tdlev);
      return;
    label_265  :
      decay0_alpha(prng_, event_, 4.694,0.,0.,t);
      thlev=3.e-9;
      decay0_nucltransK(prng_, event_, 0.265,0.086,1.6e-1,0.,tclev,thlev,tdlev);
      return;
      // std::experimental correction factor to allowed spectrum from
      // H.Daniel, NP 31(1962)293 and RMP 40(1968)659:
      // cf(e)=(1+c1/w+c2*w+c3*w**2+c4*w**3), w=e/emass+1
    label_20000  :
      decay0_beta1(prng_, event_, 1.162,84.,0.,0.,t,28.466,0.578,-0.658,0.);
      return;
    }
    // end of Bi210.f




  } // end of namespace decay0 
} // end of namespace genbb 

// end of Bi210.cc
// Local Variables: --
// mode: c++ --
// End: --
