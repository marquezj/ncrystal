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

#include "NCBkgdACEXS.hh"
#include "NCrystal/NCInfo.hh"
#include "NCMath.hh"
#include "NCNeutronSCL.hh"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include "NCRandUtils.hh"
#include "NCrystal/NCCalcBase.hh"

NCrystal::BkgdACEXS::BkgdACEXS(const NCrystal::Info* ci)
{
    xsdata = ci->getACEData();
}

/*
double NCrystal::BkgdACEXS::sampleEnergyTransfer(const double& ekin, RandomBase*rng) const
{
  // JIMD TODO: Interpolar EnerDist
  NCRYSTAL_THROW2(CalcError,"Could not sample energy transfer at ekin = "<<ekin<<" eV");
  return 0;
}*/

void NCrystal::BkgdACEXS::generateScatteringNonOriented( double ekin, double& ang, double& de, RandomBase* rng) const{

    const double xsinel = getXS_inel(ekin);
    const double xsel = getXS_el(ekin);

    if (rng->generate() < xsinel/(xsinel + xsel)){
        const double r = rng->generate();

        typedef std::map<double, std::vector<double>>::const_iterator i_t;
        typedef std::vector<double>::const_iterator i_t3;

        const std::map<double, std::vector<double>> &data = xsdata.inel_enerdist;

        i_t i=data.upper_bound(ekin);

        std::vector<double> enerdist = i->second;

        const unsigned long n = std::distance(enerdist.begin(), enerdist.end() );
        const unsigned long k = n*r;
        i_t3 ener_iter = enerdist.begin();

        std::advance(ener_iter, k);

        double Eout = *ener_iter;

        de = Eout - ekin;

        typedef std::map<double, std::map<double, std::vector<double>>>::const_iterator i_t2;
        typedef std::map<double, std::vector<double>>::const_iterator i_t4;

        std::map<double, std::map<double, std::vector<double>>> data2 = xsdata.inel_angdist;

        i_t2 i2=data2.upper_bound(ekin);
        i_t4 enerang_iter = i2->second.begin();

        std::advance(enerang_iter, k);

        const std::vector<double> angdist = enerang_iter->second;

        const double r2 = rng->generate();

        const unsigned long n2 = std::distance(angdist.begin(), angdist.end() );
        const unsigned long k2 = n2*r2;

        i_t3 angiter = angdist.begin();

        std::advance(angiter, k2);

        ang = std::acos(*angiter);
    } else {

        // JIMD TODO: check for coh / incoh elastic

        de = 0.0; // elastic scattering

        const std::map<double, double> &data = xsdata.xs_el;
        typedef std::map<double, double>::const_iterator i_t;

        double E1;

        if (ekin < data.begin()->first){
            NCRYSTAL_THROW2(CalcError,"Trying to sample ACE Bragg scattering at ekin = "<<ekin<<" eV, below lowest edge.");
        }
        else if (ekin > data.rbegin()->first) {
            E1 = data.rbegin()->second;
        }
        else {
            i_t i=data.lower_bound(ekin);
            E1 =  i->second;
        };
        const double mu = 1 - E1/ekin;

        ang = std::acos(mu);
    }
}

double NCrystal::BkgdACEXS::getXS_inel(const double& kiEn) const
{
    typedef std::map<double, double>::const_iterator i_t;

    const std::map<double, double> &data = xsdata.xs;

    i_t i=data.upper_bound(kiEn);

    if(i==data.end())
    {
      return (--i)->second;
    }
    if (i==data.begin())
    {
      return i->second;
    }
    i_t l=i; --l;

    const double delta=(kiEn - l->first)/(i->first - l->first);
    return  delta*i->second +(1-delta)*l->second;
}

double NCrystal::BkgdACEXS::getXS_el(const double& kiEn) const
{
    // JIMD TODO: check for coh / incoh elastic

    typedef std::map<double, double>::const_iterator i_t;

    const std::map<double, double> &data = xsdata.xs_el;

    if (kiEn < data.begin()->first){
        return 0.0;
    }
    else if (kiEn > data.rbegin()->first) {
        return data.rbegin()->second/kiEn;
    }
    else {
        i_t i=data.lower_bound(kiEn);
        return  i->second/kiEn;
    };
}


double NCrystal::BkgdACEXS::getXS(const double& kiEn) const
{
    const double xsinel = getXS_inel(kiEn);
    const double xsel = getXS_el(kiEn);

    return  xsel + xsinel;
}

NCrystal::RCHolder<const NCrystal::BkgdACEXS> NCrystal::createBkgdACEXS( const NCrystal::Info* ci){
  return  NCrystal::RCHolder<const NCrystal::BkgdACEXS>(new BkgdACEXS(ci));
}
