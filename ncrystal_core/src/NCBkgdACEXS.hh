#ifndef NCrystal_BkgdACEXS_hh
#define NCrystal_BkgdACEXS_hh

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

#include "NCrystal/NCDefs.hh"
#include <vector>
#include <utility>
#include "NCPointwiseDist.hh"
#include "NCElIncXS.hh"
#include "NCrystal/NCInfo.hh"

namespace NCrystal {

  class Info;
  class BkgdACEXS : public RCBase {
  public:

    //Use ACE tables to calculate inelastic scattering

    double getXS(const double& ekin) const;
//    double sampleEnergyTransfer(const double& ekin, RandomBase*) const;
    void generateScatteringNonOriented( double ekin, double& ang, double& de, RandomBase* rng) const;
    virtual ~BkgdACEXS(){}

    //data and methods used just by initialisation code:
    //BkgdACEXS(double kt);
    BkgdACEXS(const Info* ci);

  private:
    ACEData xsdata;
    double getXS_el(const double& ekin) const;
    double getXS_inel(const double& ekin) const;
  };
  RCHolder<const BkgdACEXS> createBkgdACEXS( const Info* ci);
}

#endif
