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

#include "NCrystal/NCLoadACE.hh"
#include "NCParseACE.hh"
#include "NCrystal/NCDefs.hh"
#include "NCFillHKL.hh"
#include "NCFile.hh"
#include "NCRotMatrix.hh"
#include "NCNeutronSCL.hh"
#include "NCLatticeUtils.hh"
#include "NCDebyeMSD.hh"

#include <vector>
#include <iostream>
#include <cstdlib>

const NCrystal::Info * NCrystal::loadACE( const std::string& ace_file,
                                            double temperature )
{
  return loadACE(ace_file.c_str(),temperature);
}
const NCrystal::Info * NCrystal::loadACE( const char * ace_file,
                                            double temperature )
{
  const bool verbose = (std::getenv("NCRYSTAL_DEBUGINFO") ? true : false);

  // JIMD TODO Check requested temperature is in the ACE file
  if (verbose)
    std::cout<<"NCrystal::loadACE called with ("<< ace_file
             <<", temp="<<temperature<<")"<<std::endl;

  ACEParser parser(ace_file);
  //nc_assert_always(parser.getAtomPerCell()>0);

  ////////////////////////
  // Create Info object //
  ////////////////////////

  Info * ace_lib = new Info();

  ACEData data;

  data.xs = parser.getXS();
  data.inel_enerdist = parser.getEnerDist();
  data.inel_angdist  = parser.getAngDist();
  data.xs_el = parser.getXS_el();

  ace_lib->setACEData(data);

  ///////////
  // Done! //
  ///////////

  ace_lib->objectDone();
  return ace_lib;

}

