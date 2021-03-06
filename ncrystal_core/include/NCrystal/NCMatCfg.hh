#ifndef NCrystal_MatCfg_hh
#define NCrystal_MatCfg_hh

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  This file is part of NCrystal (see https://mctools.github.io/ncrystal/)   //
//                                                                            //
//  Copyright 2015-2017 NCrystal developers                                   //
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

#include "NCrystal/NCException.hh"
#include <set>
#include <ostream>

namespace NCrystal {

  class Info;
  class SCOrientation;

  class MatCfg {
  public:

    /////////////////////////////////////////////////////////////////////////////
    // Class which is used to define the configuration of a given material,    //
    // which usually includes not only the path to a material data file in any //
    // supported format, but also other parameters needed for a particular     //
    // use-case, can be set up such as temperature, packing factor or          //
    // orientation.                                                            //
    /////////////////////////////////////////////////////////////////////////////


    /////////////////////////////////////////////////////////////////////////////
    // Constructor:                                                            //
    /////////////////////////////////////////////////////////////////////////////
    //
    // Construct material configuration by supplying at least the path to a data
    // file (in any supported format). Optionally, it is possible to set
    // configuration parameters by appending one or more occurances of
    // "<name>=<value>" to the file name, all separated with semi-colons (;).
    //
    // Thus the single line:
    //
    //    MatCfg cfg("myfile.ncmat;temp=77.0;packingfactor=0.8");
    //
    // Has the same effect as the three lines:
    //
    //    MatCfg cfg("myfile.ncmat");
    //    cfg.set_temp(77.0);
    //    cfg.set_packingfactor(0.8);
    //
    // The supported variable names and the set methods which they invoke
    // are documented below.
    //
    MatCfg( const std::string& datafile_and_parameters );
    //
    // Note that it is also possible to set the (initial) values of parameters,
    // by embedding a statement like NCRYSTALMATCFG[temp=500.0;dcutoff=0.2] into
    // the datafile itself. These parameters can of course still be overridden,
    // and such embedded configuration can be ignored entirely by appending
    // ";ignorefilecfg" to the filename:
    //
    //    MatCfg cfg("myfile.ncmat;ignorefilecfg").
    //
    // Also note that for convenience some parameters optionally support the
    // specification of units when setting their values via strings. For
    // example, the following ways of setting the temperature all correspond to
    // the freezing point of water:
    //
    //     ";temp=273.15" ";temp=273.15K" ";temp=0C" "temp=32F"
    //
    // No matter how the temperature was set, get_temp() will return the value
    // in kelvin (273.15) afterwards.

    /////////////////////////////////////////////////////////////////////////////
    // Possible parameters and their meaning:                                  //
    /////////////////////////////////////////////////////////////////////////////
    //
    // temp................: [ double, fallback value is 293.15 ]
    //                       Temperature of material in Kelvin.
    //                       [ Recognised units: "K", "C", "F" ]
    //
    // dcutoff.............: [ double, fallback value is 0 ]
    //                       D-spacing cutoff in Angstrom. Crystal planes with
    //                       spacing below this value will not be created. The
    //                       special setting dcutoff=0 causes the code to
    //                       attempt to select an appropriate threshold
    //                       automatically, and the special setting dcutoff=-1
    //                       means that HKL lists should not be created at all
    //                       (often used with skipbragg=true).
    //                       [ Recognised units: "Aa", "nm", "mm", "cm", "m" ]
    //
    // dcutoffupper........: [ double, fallback value is infinity ]
    //                       Like dcutoff, but representing an upper cutoff
    //                       instead (this is rarely desired).
    //                       [ Recognised units: "Aa", "nm", "mm", "cm", "m" ]
    //
    // packingfactor.......: [ double, fallback value is 1.0 ]
    //                       Packing factor which can be less than 1.0 for
    //                       powders, which can thus be modelled as
    //                       polycrystals with reduced density (not to be
    //                       confused with the *atomic* packing factor).
    //
    // mosaicity...........: [ double, no fallback value ]
    //                       Mosaic spread in mosaic single crystals, in
    //                       radians.
    //                       [ Recognised units: "rad", "deg", "arcmin", "arcsec" ]
    //
    // nphonon.............: [ integer, fallback value is 0 ]
    //                       Controls number of terms in phonon expansion when
    //                       estimating the background cross-section curve
    //                       (currently only used with .ncmat files). Higher
    //                       numbers potentially provide increased precision at
    //                       the cost of added initialisation time. If the value
    //                       is 0, the code will attempt to automatically select
    //                       a reasonable number of terms for the material in
    //                       question. If the value is -1, the background
    //                       cross-sections won't be provided at all.
    //
    // expandhkl...........: [ bool, fallback value is false ]
    //                       Request that lists of equivalent HKL planes be
    //                       created in Info objects, if the information is
    //                       available (normal users should not need this).
    //
    // orientationprimary..: [ special, no fallback value ]
    //                       Used to specify orientation of single crystals, by
    //                       providing both a vector in the crystal frame and
    //                       the lab frame, as explained in more detail in the
    //                       file NCSCOrientation.hh. If the six numbers in the
    //                       two vectors are respectively (c1,c2,c3) and
    //                       (l1,l2,l3), this is specified as:
    //                       "orientationprimary=@crystal:c1,c2,c3@lab:l1,l2,l3"
    //                       If (c1,c2,c3) are points in hkl space, simply
    //                       append "_hkl" to "@crystal:", as in:
    //                       "orientationprimary=@crystal_hkl:c1,c2,c3@lab:l1,l2,l3"
    //
    // orientationsecondary: [ special, no fallback value ]
    //                       Similar to orientationprimary, but the direction
    //                       might be modified slightly in case of imprecise
    //                       input, up to the value of orientationtolerance).
    //                       See the file NCSCOrientation.hh for more details.
    //
    // orientationtolerance: [ double, fallback value is 0.0001 ]
    //                       Tolerance parameter for the secondary direction of
    //                       the single crystal orientation, here in radians. See the file
    //                       NCSCOrientation.hh for more details.
    //                       [ Recognised units: "rad", "deg", "arcmin", "arcsec" ]
    //
    // overridefileext.....: [ string, fallback value is "" ]
    //                       In case the filename contains a misleading
    //                       extension, this can be used to override which
    //                       extension will be returned by
    //                       getDataFileExtension().
    //
    // braggonly...........: [ bool, fallback value is false ]
    //                       If set, scatter factories should skip all
    //                       components except everything except Bragg
    //                       diffraction.
    //
    // skipbragg...........: [ bool, fallback value is false ]
    //                       If set, scatter factories should skip Bragg
    //                       diffraction and only create other components.
    //
    // scatterbkgdmodel....: [ string, fallback value is "best" ]
    //                       Influence background model chosen by scatter
    //                       factories. The default value of "best" implies that
    //                       they should pick the most realistic one they
    //                       provide. The default NCrystal factories (for now!)
    //                       only support "simplethermalising" and
    //                       "simpleelastic" which both implies the model
    //                       implemented NCSimpleBkgd.hh, but with different
    //                       values of the "thermalise" parameter described in
    //                       that file.
    //
    // infofactory.........: [ string, fallback value is "" ]
    //                       By supplying the name of an NCrystal factory, this
    //                       parameter can be used by experts to circumvent the
    //                       usual factory selection algorithms and instead
    //                       choose the factory for creating NCrystal::Info
    //                       instances directly.
    //
    // scatterfactory.....: [ string, fallback value is "" ]
    //                       Similar to infofactory, this parameter can be used
    //                       to directly select factory with which to create
    //                       NCrystal::Scatter instances.
    //
    // absorptionfactory..: [ string, fallback value is "" ]
    //                       Similar to infofactory, this parameter can be used
    //                       to directly select factory with which to create
    //                       NCrystal::Absorption instances.

    /////////////////////////////////////////////////////////////////////////////
    // Methods for setting parameters:                                         //
    /////////////////////////////////////////////////////////////////////////////
    //
    //Directly set from C++ code:
    void set_temp( double );
    void set_dcutoff( double );
    void set_dcutoffupper( double );
    void set_packingfactor( double );
    void set_mosaicity( double );
    void set_nphonon( int );
    void set_expandhkl( bool );
    void set_orientationtolerance( double );
    void set_overridefileext( const std::string& );
    void set_braggonly( bool );
    void set_skipbragg( bool );
    void set_scatterbkgdmodel( const std::string& );
    void set_infofactory( const std::string& );
    void set_scatterfactory( const std::string& );
    void set_absorptionfactory( const std::string& );
    //
    //Special setter method, which will set mosaicity and all orientation
    //parameters based on an SCOrientation object:
    void setOrientation( const SCOrientation& );

    void set_orientationprimary( bool crystal_dir_is_point_in_hkl_space,
                                 const double (&crystal_direction)[3],
                                 const double (&lab_direction)[3] );
    void set_orientationsecondary( bool crystal_dir_is_point_in_hkl_space,
                                   const double (&crystal_direction)[3],
                                   const double (&lab_direction)[3] );
    void get_orientationprimary( bool& crystal_dir_is_point_in_hkl_space,
                                 double (&crystal_direction)[3],
                                 double (&lab_direction)[3] );
    void get_orientationsecondary( bool& crystal_dir_is_point_in_hkl_space,
                                   double (&crystal_direction)[3],
                                   double (&lab_direction)[3] );
    //
    // Set parameters from a string, using the same format as that supported by
    // the constructor, e.g. "par1=val1;...;parn=valn":
    void applyStrCfg( const std::string&  );

    /////////////////////////////////////////////////////////////////////////////
    // Methods for accessing parameters or derived information:                //
    /////////////////////////////////////////////////////////////////////////////
    //
    // Directly access from C++
    double get_temp() const;
    double get_dcutoff() const;
    double get_dcutoffupper() const;
    double get_packingfactor() const;
    double get_mosaicity() const;
    int get_nphonon() const;
    bool get_expandhkl() const;
    double get_orientationtolerance() const;
    bool get_braggonly() const;
    bool get_skipbragg() const;
    const std::string& get_overridefileext() const;
    const std::string& get_scatterbkgdmodel() const;
    const std::string& get_infofactory() const;
    const std::string& get_scatterfactory() const;
    const std::string& get_absorptionfactory() const;
    //
    // Specialised getters for derived information and datafile:
    bool isSingleCrystal() const;//true if mosaicity or orientation parameters are set
    bool isPolyCrystal() const;//same as !isSingleCrystal()
    SCOrientation createSCOrientation() const;//Create and return a new SCOrientation object based on parameters.
    const std::string& getDataFile() const;//Resolved path to the datafile
    const std::string& getDataFileAsSpecified() const;//Path to the datafile as specified in the constructor
    const std::string& getDataFileExtension() const;//Extension of datafile (actual unless overridefileext is set)
    //
    std::string toStrCfg( bool include_datafile = true, const std::set<std::string> * only_parnames = 0 ) const;
    std::string toEmbeddableCfg() const;//Produces a string like "NCRYSTALMATCFG[temp=500.0;dcutoff=0.2]" which can be embedded in data files.
    void dump( std::ostream &out, bool add_endl=true ) const;

    bool ignoredEmbeddedConfig() const;//true if constructor received ";ignorefilecfg" keyword.

    /////////////////////////////////////////////////////////////////////////////
    // Copy/assign/clone/destruct                                              //
    /////////////////////////////////////////////////////////////////////////////
    //
    // Copy/assignment/cloning is allowed and is a priori cheap, since internal
    // data structures are shared until modified (aka copy-on-write):
    MatCfg(const MatCfg&);
    MatCfg& operator=(const MatCfg&);
    MatCfg clone() const { return MatCfg(*this); }
    //A completely "clean" clone, unconnected to the original and no internal caches set is obtainable with:
    MatCfg cloneUnshared() const { MatCfg c(*this); c.cow(); return c; }
    //
    //Destructor:
    ~MatCfg();


    /////////////////////////////////////////////////////////////////////////////
    // Interface for NCrystal factory infrastructure:                          //
    /////////////////////////////////////////////////////////////////////////////
    //
    // Advanced interface, which is intended solely for use by the NCrystal
    // factory infrastructure, in order to avoid unnecessarily re-initialising
    // expensive objects.
    //
    // Monitor parameter accesses (does not take ownership of the spy and
    // parameter modifications will be forbidden while such monitoring is in
    // place):
    struct AccessSpy {
      virtual ~AccessSpy(){};
      virtual void parAccessed(const std::string& parname) = 0;
    };
    bool hasAccessSpy(AccessSpy*) const;
    void addAccessSpy(AccessSpy*) const;
    void removeAccessSpy(AccessSpy*) const;
    //
    //Verify that the parameter values are not inconsistent or
    //incomplete. Throws BadInput exception if they are. This will automatically
    //be invoked by the factory infrastructure:
    void checkConsistency() const;
    //
    //returns a string like "par1=val1;...;parn=valn" for the specified
    //parameters only (using "<>" as value for unset parameters):
    void getCacheSignature( std::string& signature,
                            const std::set<std::string>& parameters ) const;

  private:

    struct Impl;
    Impl* m_impl;
    void cow();

  };

  inline std::ostream& operator<< (std::ostream& s, const MatCfg& cfg) { cfg.dump(s,false); return s; }

}

#endif
