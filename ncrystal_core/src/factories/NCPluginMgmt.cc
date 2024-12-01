////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  This file is part of NCrystal (see https://mctools.github.io/ncrystal/)   //
//                                                                            //
//  Copyright 2015-2024 NCrystal developers                                   //
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

#include "NCrystal/factories/NCPluginMgmt.hh"
#include "NCrystal/internal/utils/NCDynLoader.hh"
#include "NCrystal/internal/utils/NCString.hh"
#include "NCrystal/internal/utils/NCMsg.hh"

//MT TODO: Do we need to make these thread-safe?

namespace NC = NCrystal;
namespace NCP = NCrystal::Plugins;

namespace NCRYSTAL_NAMESPACE {
  namespace Plugins {

    namespace {
      std::mutex& getPluginMgmtMutex()
      {
        static std::mutex mtx;
        return mtx;
      }
      std::vector<PluginInfo>& getPLList()
      {
        static std::vector<PluginInfo> thelist;
        return thelist;
      }

      void actualLoadPlugin( PluginInfo pinfo,
                             std::function<void()> regfct )
      {
        //Mutex is already locked when this is called!
        nc_assert_always(pinfo.pluginType==PluginType::Dynamic||pinfo.pluginType==PluginType::Builtin);
        bool verbose = ncgetenv_bool("DEBUG_PLUGIN");
        std::string ptypestr(pinfo.pluginType==PluginType::Dynamic?"dynamic":"builtin");
        if (verbose)
          NCRYSTAL_MSG("Loading "<<ptypestr<<" plugin \""<<pinfo.pluginName<<"\".");

        for ( const auto& pl : getPLList() ) {
          if ( pl.pluginName == pinfo.pluginName )
            NCRYSTAL_THROW2(CalcError,"ERROR: attempting to load plugin named \""<<pinfo.pluginName<<"\" more than once!");
        }

        try {
          regfct();
        } catch (std::exception& e) {
          NCRYSTAL_RAWOUT("NCrystal ERROR: Problems while loading plugin!\n");
          throw;
        }

        getPLList().push_back(pinfo);

        if (verbose)
          NCRYSTAL_MSG("Done loading plugin \""<<pinfo.pluginName<<"\".");
      }
    }
  }
}

namespace NCRYSTAL_NAMESPACE {
  namespace Plugins {
    namespace {
      PluginInfo loadDynamicPluginImpl( std::string path_to_shared_lib,
                                        std::string pluginName,
                                        std::string regfctname )
      {
        PluginInfo pinfo;
        pinfo.pluginType = PluginType::Dynamic;
        pinfo.fileName = path_to_shared_lib;
        pinfo.pluginName = pluginName;

        NCRYSTAL_LOCK_GUARD(getPluginMgmtMutex());

        if (ncgetenv_bool("DEBUG_PLUGIN"))
          NCRYSTAL_MSG("Attempting to loading dynamic library with plugin: "<<pinfo.fileName);

        DynLoader dl( pinfo.fileName,
                      DynLoader::ScopeFlag::local,//reduce symbol interference between plugins
                      DynLoader::LazyFlag::now );//better get the error upon load

        if (pinfo.pluginName.empty())
          pinfo.pluginName = dl.getFunction<const char*()>("ncplugin_getname")();
        auto regfct = dl.getFunction<void()>(regfctname);

        dl.doNotClose();//avoid dlclose (sadly, this is leaky but otherwise we
        //might get segfaults at programme shutdown...)

        actualLoadPlugin( pinfo, std::move(regfct) );

        return pinfo;
      }
    }
  }
}

NCP::PluginInfo NCP::loadDynamicPlugin( std::string path_to_shared_lib )
{
  return loadDynamicPluginImpl( path_to_shared_lib, "", "ncplugin_register" );
}

NCP::PluginInfo NCP::loadBuiltinPlugin( std::string pluginName,
                                        std::function<void()> regfct )
{
  PluginInfo pinfo;
  pinfo.pluginType = PluginType::Builtin;
  pinfo.pluginName = pluginName;
  NCRYSTAL_LOCK_GUARD(getPluginMgmtMutex());
  actualLoadPlugin( pinfo, std::move(regfct) );
  return pinfo;
}

std::vector<NCP::PluginInfo> NCP::loadedPlugins()
{
  NCP::ensurePluginsLoaded();
  std::vector<NCP::PluginInfo> result;
  {
    NCRYSTAL_LOCK_GUARD(getPluginMgmtMutex());
    result = getPLList();
  }
  return result;
}

//Support for .ncmat files is ALWAYS enabled by default, unless disabled with
//the macro NCRYSTAL_DISABLE_NCMAT. Likewise, the standard scatter/absorption
//factories are always enabled, unless respectively NCRYSTAL_DISABLE_STDSCAT,
//NCRYSTAL_DISABLE_STDMPSCAT and NCRYSTAL_DISABLE_STDABS is defined.

#ifndef NCRYSTAL_DISABLE_STDDATASOURCES
extern "C" void NCRYSTAL_APPLY_C_NAMESPACE(register_stddatasrc_factory)();
#endif
#ifndef NCRYSTAL_DISABLE_STDSCAT
extern "C" void NCRYSTAL_APPLY_C_NAMESPACE(register_stdscat_factory)();
#endif
#ifndef NCRYSTAL_DISABLE_STDMPSCAT
extern "C" void NCRYSTAL_APPLY_C_NAMESPACE(register_stdmpscat_factory)();
#endif
#ifndef NCRYSTAL_DISABLE_STDABS
extern "C" void NCRYSTAL_APPLY_C_NAMESPACE(register_stdabs_factory)();
#endif
#ifndef NCRYSTAL_DISABLE_STDNCMAT
extern "C" void NCRYSTAL_APPLY_C_NAMESPACE(register_stdncmat_factory)();
#endif
#ifndef NCRYSTAL_DISABLE_STDLAZ
extern "C" void NCRYSTAL_APPLY_C_NAMESPACE(register_stdlaz_factory)();
#endif
#ifndef NCRYSTAL_DISABLE_QUICKFACT
extern "C" void NCRYSTAL_APPLY_C_NAMESPACE(register_quick_factory)();
#endif
#ifndef NCRYSTAL_DISABLE_EXPERIMENTALSCATFACT
extern "C" void NCRYSTAL_APPLY_C_NAMESPACE(register_experimentalscatfact)();
#endif


//If NCRYSTAL_HAS_BUILTIN_PLUGINS is defined, it is assumed that the code is
//being built with a void NCrystal::provideBuiltinPlugins() function defined in
//another module. The envisioned use-case is that plugins can be available in
//two modes: 1) as a separate shared library with standard symbols for loading
//the library, and 2) being built into a given NCrystal installation at compile
//time.
#ifdef NCRYSTAL_HAS_BUILTIN_PLUGINS
namespace NCRYSTAL_NAMESPACE {
  void provideBuiltinPlugins();
}
#endif

#ifdef NCRYSTAL_SIMPLEBUILD_DEVEL_MODE
#  include "NCrystal/internal/utils/NCFileUtils.hh"
namespace NCRYSTAL_NAMESPACE {
  namespace {
    std::string getSBLDPkgLib(const char* pkgname) {
      auto sbld_libdir_raw = std::getenv("SBLD_LIB_DIR");
      if ( !sbld_libdir_raw)
        NCRYSTAL_THROW(CalcError,"SBLD_LIB_DIR not set");
      auto soprefix = std::string(sbld_libdir_raw) + "/libPKG__";
      std::string lib(soprefix + pkgname);
      if ( file_exists(lib+".so") ) {
        lib+=".so";
      } else if (file_exists(lib+".dylib")) {
        lib+=".dylib";
      } else {
        NCRYSTAL_THROW2(FileNotFound,"Could not find"
                        " sbl pkg library (\""
                        <<pkgname<<"\"). Is the package enabled?");
      }
      return lib;
    }
  }
}
#endif

void NCP::ensurePluginsLoaded()
{
  static std::atomic<bool> first(true);
  bool btrue(true);
  if (!first.compare_exchange_strong(btrue,false))
    return;//only the first call will proceed past this point. If we instead
           //used a mutex, we would get a deadlock (e.g. when registerFactory
           //calls ensurePluginsLoaded before checking that the new factory name
           //is unique). The MT-safety of the calls below should be the same as
           //for any other call to loadXXXPlugin.

  static bool done = false;
  if (done)
    return;
  done = true;

#ifdef NCRYSTAL_SIMPLEBUILD_DEVEL_MODE
  //Standard plugins, dynamic in simplebuild mode, except for datasources:
  auto loadstdplugin = []( const char* pkgname,
                           const char* pluginname,
                           std::string regfctname )
  {
    regfctname = std::string(ncrystal_xstr(NCRYSTAL_C_NAMESPACE)) + regfctname;
    loadDynamicPluginImpl( getSBLDPkgLib(pkgname), pluginname, regfctname);
  };
  loadstdplugin("NCScatFact","stdscat","register_stdscat_factory");
  loadstdplugin("NCScatFact","stdmpscat","register_stdmpscat_factory");
  loadstdplugin("NCExperimental","stdexpscat","register_experimentalscatfact");
  loadstdplugin("NCFactory_Laz","stdlaz","register_stdlaz_factory");
  loadstdplugin("NCAbsFact","stdabs","register_stdabs_factory");
  loadstdplugin("NCFactory_NCMAT","stdncmat","register_stdncmat_factory");
  loadstdplugin("NCQuickFact","stdquick","register_quick_factory");
  //loadstdplugin("NCNXSFactories","legacynxslaz","register_nxslaz_factories");
#endif

#ifndef NCRYSTAL_DISABLE_STDDATASOURCES
  loadBuiltinPlugin("stddatasrc",NCRYSTAL_APPLY_C_NAMESPACE(register_stddatasrc_factory));
#endif
#ifndef NCRYSTAL_SIMPLEBUILD_DEVEL_MODE
  //Standard plugins, always as builtin plugins:
#  ifndef NCRYSTAL_DISABLE_STDSCAT
  loadBuiltinPlugin("stdscat",NCRYSTAL_APPLY_C_NAMESPACE(register_stdscat_factory));
#  endif
#  ifndef NCRYSTAL_DISABLE_STDMPSCAT
  loadBuiltinPlugin("stdmpscat",NCRYSTAL_APPLY_C_NAMESPACE(register_stdmpscat_factory));
#  endif
#  ifndef NCRYSTAL_DISABLE_EXPERIMENTALSCATFACT
  loadBuiltinPlugin("stdexpscat",NCRYSTAL_APPLY_C_NAMESPACE(register_experimentalscatfact));
#  endif
#  ifndef NCRYSTAL_DISABLE_STDLAZ
  loadBuiltinPlugin("stdlaz",NCRYSTAL_APPLY_C_NAMESPACE(register_stdlaz_factory));
#  endif
#  ifndef NCRYSTAL_DISABLE_STDABS
  loadBuiltinPlugin("stdabs",NCRYSTAL_APPLY_C_NAMESPACE(register_stdabs_factory));
#  endif
#  ifndef NCRYSTAL_DISABLE_NCMAT
  loadBuiltinPlugin("stdncmat",NCRYSTAL_APPLY_C_NAMESPACE(register_stdncmat_factory));
#  endif
#  ifndef NCRYSTAL_DISABLE_QUICKFACT
  loadBuiltinPlugin("stdquick",NCRYSTAL_APPLY_C_NAMESPACE(register_quick_factory));
#  endif
#endif

  //Static custom (builtin) plugins:
#ifdef NCRYSTAL_HAS_BUILTIN_PLUGINS
  provideBuiltinPlugins();
#endif

  //Dynamic custom plugins, as indicated by environment variable:
  for ( auto& pluginlib : split2(ncgetenv("PLUGIN_LIST"),0,':') ) {
    trim(pluginlib);
    if (pluginlib.empty())
      continue;
#ifdef NCRYSTAL_SIMPLEBUILD_DEVEL_MODE
    //Fixme: std::atomic. Also, why are we loading ourselves?
    static bool firstpl=true;
    if (firstpl) {
      firstpl=false;
      DynLoader( getSBLDPkgLib("NCFactories"),
                 DynLoader::ScopeFlag::global ).doNotClose();
    }
#endif
    Plugins::loadDynamicPlugin(pluginlib);
  }
}
