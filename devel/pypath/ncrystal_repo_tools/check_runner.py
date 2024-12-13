
################################################################################
##                                                                            ##
##  This file is part of NCrystal (see https://mctools.github.io/ncrystal/)   ##
##                                                                            ##
##  Copyright 2015-2024 NCrystal developers                                   ##
##                                                                            ##
##  Licensed under the Apache License, Version 2.0 (the "License");           ##
##  you may not use this file except in compliance with the License.          ##
##  You may obtain a copy of the License at                                   ##
##                                                                            ##
##      http://www.apache.org/licenses/LICENSE-2.0                            ##
##                                                                            ##
##  Unless required by applicable law or agreed to in writing, software       ##
##  distributed under the License is distributed on an "AS IS" BASIS,         ##
##  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  ##
##  See the License for the specific language governing permissions and       ##
##  limitations under the License.                                            ##
##                                                                            ##
################################################################################

def get_available_checks_list():
    import pathlib
    return sorted( [ f.name[7:-3] for f in
                     pathlib.Path(__file__).parent.glob('_check_*.py') ] )

def run_check( name ):
    print()
    print(f">>>>> Running check: {name}")
    print()
    import importlib
    pymodname = '_check_%s'%name
    mod = importlib.import_module(f'..{pymodname}', __name__)
    assert hasattr(mod,'main')
    mod.main()

def run_all_checks():
    for c in get_available_checks_list():
        run_check(c)

#fixme:
#
# Planned additional tests:
#    * shebangs consistent
#    * check files left outside dirs
#
# Work on _climode_*.py:
#
#    * plot dependency graphs (between components)
#    * build the docs
#    * launch the local cmake based builds into some dir
#    * launch the ctests
#
# CI missing:
#
#    * exercising embedded data with e.g. tests.
#    * cpp check
#    * python coverage testing.
#    * static tests
#
# Misc missing:
#    * Restore .icc ?
