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

#include "NCParseACE.hh"
#include "NCString.hh"
#include "NCFile.hh"
#include "NCrystal/NCDefs.hh"
#include <cctype>
#include <cstdlib>
#include <iostream>

NCrystal::ACEParser::ACEParser(const char* fn)
{
  std::vector<int> iz;
  std::vector<double> aw;
  std::vector<int> nxs;
  std::vector<int> jxs;


  int nil, nieb, itie, itix, itxe, itce, itcx, nen, idpnc, ne_el;

  nc_assert(NCrystal::file_exists(fn));
  std::ifstream infile;
  infile.open(fn);
  if (!infile.good())
    NCRYSTAL_THROW2(DataLoadError,"Problems opening file: "<<fn);

  std::string line_str; // First data line
  getline(infile, line_str);
  std::string hz_str = line_str.substr(0,10);
  std::string dbl_str = line_str.substr(11,12);
  dbl_str = line_str.substr(23,12);
  std::string hd_str = line_str.substr(35,12);
  std::string hk_str;
  getline(infile, hk_str);


  std::string tmpstr;
  int tmpint;
  double tmpdbl;

  for (int i=0;i<4;i++){
    getline(infile, tmpstr);
    std::istringstream iss(tmpstr);
    while(iss >> tmpint && iss >> tmpdbl) {
      iz.push_back(tmpint);
      aw.push_back(tmpdbl);
    }
  }

  for (int i=0;i<2;i++){
    getline(infile, tmpstr);
    std::istringstream iss(tmpstr);
    while(iss >> tmpint) {
      nxs.push_back(tmpint);
    }
  }

  for (int i=0;i<4;i++){
    getline(infile, tmpstr);
    std::istringstream iss(tmpstr);
    while(iss >> tmpint) {
      jxs.push_back(tmpint);
    }
  }

  nil = nxs[2];
  nieb = nxs[3];
  itie = jxs[0];
  itix = jxs[1];
  itxe = jxs[2];
  itce = jxs[3];
  itcx = jxs[4];
  idpnc = nxs[4];
  nen = itix - itie - 1;

  if ( nxs[6] != 0 ) {
    infile.close();
    NCRYSTAL_THROW(DataLoadError,"Only iwt=1 ACE files are currently supported");
  }

  std::vector<double> xss;

  while(getline(infile, tmpstr)){
    std::istringstream iss(tmpstr);
    while(iss >> tmpdbl) {
      xss.push_back(tmpdbl);
    }
  }

/*  std::cout << "itce =" << itce << ", NEel = " << xss[itce-1] << std::endl;
  std::cout << "idpnc =" << idpnc << std::endl;*/

  infile.close();

  for (int i = 0; i < nen ; i++){
    xs.insert(std::pair<double, double>(xss[itie + i ]*1e6, xss[itix + i - 1 ]));
  }

  for (int i = 0; i < nen ; i++){
    std::vector<double> tmpvector;
    std::map<double, std::vector<double>> tmpmap;
    for (int j = 0; j < nieb; j++){
      std::vector<double> tmpvector2;
      tmpvector.push_back(xss[itxe - 1 + i*nieb*(nil+2) + j*(nil+2) ]*1e6);
      for (int k = 0; k< nil +1 ; k++){
        tmpvector2.push_back(xss[itxe -1 + i*nieb*(nil+2) + j*(nil+2) + 1 + k]);
      }
      tmpmap.insert(std::pair<double,std::vector<double>>(tmpvector.back(), tmpvector2));
    }
    inel_enerdist.insert(std::pair<double, std::vector<double>>(xss[itie + i ]*1e6, tmpvector));
    inel_angdist.insert(std::pair<double,std::map<double, std::vector<double>>>(xss[itie + i ]*1e6, tmpmap));
  }

  if (itce != 0){
    if (idpnc == 4)  {
      ne_el = xss[itce-1];
      for (int i = 0; i < nen ; i++){
        xs_el.insert(std::pair<double, double>(xss[itce + i ]*1e6, xss[itce + ne_el + i ]*1e6));
      }
    } else {
      NCRYSTAL_THROW(DataLoadError,"Only coherent scattering is currently supported");
    }
/*    for (auto & elem: xs_el){
      std::cout << elem.first << " " << elem.second << "\n";
    }*/
  }
}

NCrystal::ACEParser::~ACEParser()
{
}

