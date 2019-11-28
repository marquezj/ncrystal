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

#include "NCrystal/NCBkgdACE.hh"
#include "NCrystal/NCInfo.hh"
#include "NCBkgdACEXS.hh"
#include "NCRandUtils.hh"

#include <map>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <algorithm>
#include <vector>
#if __cplusplus >= 201103L
#  include <mutex>
#endif

namespace NCrystal {
  bool BkgdACE_checkinfo(const Info* ci, bool throwonerr )
  {
    nc_assert_always(ci);
    if ( ! ci->hasACEData() ){
      if (throwonerr) {
        NCRYSTAL_THROW(MissingInfo,"Passed info object lacks ACE data");
      } else {
        return false;
      }
    }
    return true;
  }
  namespace BkgdACECache {
    typedef std::pair<uint64_t,uint64_t> Key;
    typedef std::map<Key,RCHolder<const BkgdACEXS> > Map;
    static Map cache;
#if __cplusplus >= 201103L
    static std::mutex cache_mutex;
    //cache_mutex is used to protect cache access in case of multi-threading
    //(access should be rare, so a simple mutex approach is probably just fine).
#endif
  }
}

bool NCrystal::BkgdACE::hasSufficientInfo(const Info* ci)
{
  return ci && BkgdACE_checkinfo(ci,false);
}


NCrystal::BkgdACE::BkgdACE(const Info* ci)
   : ScatterIsotropic("ACEfile")
{
  nc_assert_always(ci);
  BkgdACE_checkinfo(ci,true);

  RCHolder<const BkgdACEXS> xs;
  const bool verbose = (std::getenv("NCRYSTAL_DEBUGSCATTER") ? true : false);
  //Construct unique cachekey for NCInfo instance and parameters above:
  BkgdACECache::Key cachekey(ci->getUniqueID(),0);
  {
#if __cplusplus >= 201103L
    std::lock_guard<std::mutex> lock(BkgdACECache::cache_mutex);
#endif
    //Search cache:
    BkgdACECache::Map::iterator it = BkgdACECache::cache.find(cachekey);
    if (it==BkgdACECache::cache.end()) {
      //not in cache, must create and insert in cache:
      if (verbose)
        std::cout<<"NCBkgdACE failed to find relevant BkgdACEXS object in cache (cachelength="
                 <<BkgdACECache::cache.size()<<"). Creating from scratch."<<std::endl;
      xs = createBkgdACEXS(ci);
      BkgdACECache::cache[cachekey] = xs;
    } else {
      //found in cache!
      nc_assert_always(it->second.obj());
      if (verbose)
        std::cout<<"NCBkgdACE found relevant BkgdACEXS object in cache (cachelength="
                 <<BkgdACECache::cache.size()<<")."<<std::endl;
      xs = it->second;
    }
    nc_assert_always(xs.obj()&&xs.obj()->refCount()>1);
    m_ACEmodel = xs.obj();
    m_ACEmodel->ref();
  }
  validate();
}

NCrystal::BkgdACE::~BkgdACE()
{
  if (m_ACEmodel && !std::getenv("NCRYSTAL_NEVERCLEARCACHES") ) {
#if __cplusplus >= 201103L
    std::lock_guard<std::mutex> lock(BkgdACECache::cache_mutex);
#endif
    //Remove from cache if we are the last user.
    BkgdACECache::Map::iterator it(BkgdACECache::cache.begin()),
                                      itE(BkgdACECache::cache.end());
    for (;it!=itE;++it) {
      if (it->second.obj()==m_ACEmodel) {
        assert(it->second.obj()->refCount()>=2);//normal assert, since we can't throw from destructors
        if (it->second.obj()->refCount()==2) {
          //cache itself holds 1 ref, we hold one in m_ACEmodel, and noone else does.
          BkgdACECache::cache.erase(it);
          if (std::getenv("NCRYSTAL_DEBUGSCATTER"))
            std::cout<<"NCBkgdACE destructor removed BkgdACEXS object from cache (cachelength="
                     <<BkgdACECache::cache.size()<<")."<<std::endl;
        }
        break;
      }
    }
    m_ACEmodel->unref();
  }
}

double NCrystal::BkgdACE::crossSectionNonOriented(double ekin) const
{
  return m_ACEmodel->getXS(ekin);
}

//generateScatteringNonOriented( double, double& a, double& de ) const
void NCrystal::BkgdACE::generateScatteringNonOriented( double ekin, double& ang, double& de ) const
{
  //ScatterXSCurve::generateScatteringNonOriented(ekin,angle,de);
  RandomBase * rand = getRNG();
  m_ACEmodel->generateScatteringNonOriented(ekin,ang,de,rand);
}

void NCrystal::BkgdACE::generateScattering( double ekin, const double (&indir)[3],
                                    double (&outdir)[3], double& de ) const
{
  // JIMD TODO
  // ScatterXSCurve::generateScattering(ekin,indir,outdir,de);
  RandomBase * rand = getRNG();
  double ang;
  m_ACEmodel->generateScatteringNonOriented(ekin,ang,de,rand);
  randDirectionGivenScatterMu(rand,std::cos(ang),indir,outdir);
}
