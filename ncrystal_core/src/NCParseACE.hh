#ifndef NCrystal_ParseACE_hh
#define NCrystal_ParseACE_hh

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

#include "NCVector.hh"
#include <map>
#include <string>
#include <vector>
#include <fstream>

namespace NCrystal {

  class Vector;
  class Info;

  class ACEParser {
  public:

    //Parse .ace files.


    ACEParser(const char* fn);
    ~ACEParser();
    inline std::map<double, double> getXS() const;
    inline std::map<double, std::vector<double>> getEnerDist() const;
    inline std::map<double, std::map<double, std::vector<double>>> getAngDist() const;
    inline std::map<double, double> getXS_el() const;

  private:

    std::map<double, double> xs;
    std::map<double, std::vector<double>> inel_enerdist;
    std::map<double, std::map<double, std::vector<double>>> inel_angdist;
    std::map<double, double> xs_el;

  };

}


////////////////////////////
// Inline implementations //
////////////////////////////

namespace NCrystal {
  /*inline double NCMATParser::getDebyeTemp() const {return m_debye_temp;}
  inline const std::map<std::string, double>& NCMATParser::getDebyeMap() const {return m_debye_map;}
  inline const std::map<std::string, std::vector<Vector>  >& NCMATParser::getAtomicPosMap() const { return m_atomic_pos; }
  inline unsigned NCMATParser::getAtomPerCell () const  {return m_atomnum;}
  inline unsigned NCMATParser::getSpacegroupNum() const { return m_sg;}
  inline void NCMATParser::getLatticeParameters(double &a, double &b, double &c, double &alpha, double &beta, double &gamma) const
  {a=m_a; b=m_b; c=m_c; alpha=m_alpha; beta=m_beta; gamma=m_gamma;  }*/
  inline std::map<double, double> ACEParser::getXS() const {return xs;}
  inline std::map<double, std::vector<double>> ACEParser::getEnerDist() const {return inel_enerdist;}
  inline std::map<double, std::map<double, std::vector<double>>> ACEParser::getAngDist() const {return inel_angdist;}
  inline std::map<double, double> ACEParser::getXS_el() const {return xs_el;}
}

#endif
