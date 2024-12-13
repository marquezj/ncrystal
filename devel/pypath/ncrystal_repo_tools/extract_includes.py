
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

def _is_exceptional( f, incstatement ):
    if incstatement == 'NCrystal/ncapi.h':
        return True
    from .dirs import coreroot
    if not f.is_relative_to( coreroot ):
        return False
    frel = str(f.relative_to( coreroot ))
    if ( frel == 'include/NCrystal/factories/NCPluginBoilerplate.hh'
         and incstatement == 'NCrystal/NCrystal.hh' ):
        return True
    if ( frel == 'src/utils/NCCFileUtils.cc'
         and incstatement == 'NCCFileUtils.h' ):
        return True

_include_extractor = [None]
def get_include_staments_from_file( path, *,
                                    ignore_exceptional = True ):
    if _include_extractor[0] is None:
        import re
        pattern = b'^\\s*#\\s*include\\s*"\\s*(([a-zA-Z0-9_/.]+))\\s*"'
        _include_extractor[0] = re.compile(pattern).match

    extractor = _include_extractor[0]

    #For efficiency, initial dig through file using the grep command:
    import subprocess
    rv = subprocess.run( ['grep','.*#.*include.*"..*"',
                          str(path.absolute())],
                         capture_output = True )
    #grep exit code of 1 simply indicates no hits
    if rv.returncode not in (0,1) or rv.stderr:
        raise RuntimeError('grep command failed')
    if rv.returncode == 1:
        return set()
    res = []
    for line in rv.stdout.splitlines():
        v = extractor(line)
        if v:
            v = v.groups()[1].decode('utf8')
            if not ( ignore_exceptional and _is_exceptional( path, v ) ):
                res.append( v )
    return set(res)
