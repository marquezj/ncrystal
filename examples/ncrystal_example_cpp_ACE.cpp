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

#include "NCrystal/NCrystal.hh"
#include <iostream>
#include <cstdlib>

class MyRandGen : public NCrystal::RandomBase {
public:
  MyRandGen() : RandomBase() {}
  virtual double generate() {
    //using rand from cstdlib for this small example (note,
    //this is not really recommended for scientific work):
    return std::rand() / (RAND_MAX + 1.);
  }
protected:
  virtual ~MyRandGen() {}
};

int main() {

  //Setup random generator:
  NCrystal::setDefaultRandomGenerator(new MyRandGen);

  //Create and use polycrystalline aluminium:
  const NCrystal::Scatter * pc = NCrystal::createScatter( "diam.iwt1.ace;temp=25C;bragg=0" );
  pc->ref();

  double wl = 2.5;//angstrom
  double ekin = NCrystal::wl2ekin(wl);
  double xsect = pc->crossSectionNonOriented(ekin);
  std::cout << "polycrystal Diam x-sect at " << wl << " Aa is " << xsect << " barn" << std::endl;

  double angle,delta_ekin;
  for (unsigned i=0;i<20;++i) {
    pc->generateScatteringNonOriented( ekin, angle, delta_ekin );
  }

  for (ekin=1.0e-5;ekin<2;ekin=ekin*1.1){
        double xsect = pc->crossSectionNonOriented(ekin);
         std::cout << ekin << " " << xsect << std::endl;
    }

//    std::cout <<"polycrystal random angle/delta-e at "<<wl<<" Aa is "
//              <<angle*57.2957795131<<" degrees and "<<delta_ekin*1e3<<" meV"<<std::endl;

/*  //Create and use water:
  const NCrystal::Scatter * h2o = NCrystal::createScatter( "lwtr-293.ace;temp=25C;bragg=0" );
  pc->ref();

  for (ekin=1.0e-5;ekin<2;ekin=ekin*1.1){
      double xsect = h2o->crossSectionNonOriented(ekin);
//      std::cout << ekin << " " << xsect << std::endl;
  }*/
/*  std::cout<<std::endl;
  std::cout<<std::endl;
  std::cout<<std::endl;
  ekin = 1.0;
  for (unsigned i=0;i<1000;++i) {
    h2o->generateScatteringNonOriented( ekin, angle, delta_ekin );
    ekin = ekin + delta_ekin;
    std::cout<<ekin<<std::endl;
  }*/


  return 0;
}
