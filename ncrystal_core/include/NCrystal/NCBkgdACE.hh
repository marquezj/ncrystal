#ifndef NCrystal_BkgdACE_hh
#define NCrystal_BkgdACE_hh

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  This file is part of NCrystal (see https://mctools.github.io/ncrystal/)   //
//                                                                            //
//  Copyright 2015-2019 NCrystal developers                                   //
//                                                                            //
//  Licensed under the Apache License, Version 2.0 (the "License");           //
//  you may not use this file except in compliance with the License.          //
//  You may obtain a copy of the License at                                   //
//                                                                            //
//      http://www.apache.org/licenses/LICENSE-2.0                            //
//                                                                            //
//  Unless required by applicable law or agreed to in writing, software       //
//  distributed under the License is distributed on an "AS IS" BASIS,         //
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  //
//  See the License for the specific language governing permissions and       //
//  limitations under the License.                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//#include "NCrystal/NCScatterXSCurve.hh"
#include "NCrystal/NCScatterIsotropic.hh"

namespace NCrystal {

  class Info;
  class BkgdACEXS;

  class NCRYSTAL_API BkgdACE : public ScatterIsotropic {
  public:

    //Provides the total inelastic scattering cross section from an ACE file.
    //

    BkgdACE( const Info*);

    virtual double crossSectionNonOriented(double ekin) const;
    virtual void generateScatteringNonOriented( double ekin, double& ang, double& de ) const;
    virtual void generateScattering( double ekin, const double (&indir)[3],
                                     double (&outdir)[3], double& de ) const;

    //Check if BkgdACE can be created from the info:
    static bool hasSufficientInfo(const Info*);

  protected:
    virtual ~BkgdACE();
    const BkgdACEXS* m_ACEmodel;
  };
}

#endif
