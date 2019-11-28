#ifndef NCrystal_LoadACE_hh
#define NCrystal_LoadACE_hh

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

#include "NCrystal/NCInfo.hh"
#include <limits>

namespace NCrystal {

  // Read .ace file and return a corresponding NCrystal::Info object from it
  // (it will have a reference count of 0 when returned).
  //

  NCRYSTAL_API const Info * loadACE( const char * ace_file,
                                       double temp = 293.15 //kelvin
                                       );

  //std::string version for convenience:
  NCRYSTAL_API const Info * loadACE( const std::string& ace_file,
                                       double temp = 293.15 //kelvin
                                       );


}

#endif
