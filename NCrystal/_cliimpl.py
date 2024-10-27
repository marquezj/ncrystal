
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

"""Internal utilities needed by command-line scripts in _cli_*.py and the
utilities in cliutils.py. This is in particular needed to ensure that
command-line tools can be invoked both on the command line itself, but also from
a subprocess-free Python API via the

Of course, features should as far as possible be available via a dedicated
pythonic API. For instance, the hfg2ncmat.py module provides a pythonic API for
the features which can also be accessed via the command-line script
ncrystal_hfg2ncmat implemented in _cli_hfg2ncmat.py.

Notably, command line scripts implemented in their _cli_*.py modules, should:

* Use the create_ArgumentParser() below to instantiate their
  argparse.ArgumentParser objects.
* Provide a create_argparser_for_sphinx( progname ) function which returns an
  ArgumentParser object (before it has been used to parse anything).
* The scripts can raise normal exceptions, or even SystemExit exceptions. When
  used from the Python API these will be mapped to RuntimeError's.
* The entry point should be a function def main(argv) which should use the
  "@cli_entry_point" decorator:
* As all other code in NCrystal Python modules, do not use the normal print
  function, but instead use the print(..) function from the ._common
  module. Likewise, warning's should be emitted via the warn(..) function from
  that module.

    @cli_entry_point
    def main( argv = None ):

FIXME: We should have a unit test which checks for all of the above issues!

"""

__all__ = []

def warn(*a,**kw):
    from ._common import warn
    warn(*a,**kw)

def print(*a,**kw):
    from ._common import print
    print(*a,**kw)

_argparse_postinitfct = [ None ]
_argparse_extra_kwargs = [ None ]
def create_ArgumentParser( *args, **kwargs ):
    """Always create argparse.ArgumentParser objects from this method, to ensure
    output is redirected while invoking cmdline scripts with cliutils.run
    """
    from argparse import ArgumentParser
    for k,v in (_argparse_extra_kwargs[0] or {}).items():
        if k not in kwargs:
            kwargs[k] = v
    parser = ArgumentParser( *args, **kwargs )
    #Ensure consistent argparse --help output from older versions, by changing a
    #section title:
    _fix_argparse_action_group_title(parser,'optional arguments','options')
    f = _argparse_postinitfct[0]
    if f is not None:
        f(parser)
    return parser

class ctxmgr_modify_argparse_creation:

    def __init__(self, *, exit_on_error, redirect_stdout_to_stderr ):
        self.__exit_on_error = exit_on_error
        self.__redirect_stdout_to_stderr = redirect_stdout_to_stderr

    def __enter__(self):
        self.__orig_postinitfct = _argparse_postinitfct[0]
        orig_postinitfct = self.__orig_postinitfct
        redirect_stdout_to_stderr = self.__redirect_stdout_to_stderr
        exit_on_error = self.__exit_on_error
        def f(parser):
            if orig_postinitfct:
                orig_postinitfct(parser)
            if not exit_on_error:
                def error(message):
                    from argparse import ArgumentError
                    raise ArgumentError(None,message)
                parser.error = error
            if not redirect_stdout_to_stderr:
                return
            #Monkey patch object to redirect stdout/stderr of argparse to
            #NCrystal's print handler.
            if not hasattr(parser,'_print_message'):
                warn("argparse _print_message method (from the unofficial API)"
                     " disappeared. Argparse output redirection to NCrystal msg"
                     " handler will not work")
                return
            def _print_message( message, file=None):
                if message:
                    print(message)
            parser._print_message = _print_message
        _argparse_postinitfct[0] = f
        self.__orig_extra_kwargs = _argparse_extra_kwargs[0]
        orig_extra_kwargs = self.__orig_extra_kwargs
        new_extra_kwargs = dict( (k,v)
                                 for k,v in (orig_extra_kwargs or {}).items() )
        #exit_on_error only added in python 3.9. We detect the support by
        #looking in the docstring of the class:
        from argparse import ArgumentParser
        if 'exit_on_error' in ArgumentParser.__doc__:
            new_extra_kwargs['exit_on_error'] = self.__exit_on_error
        _argparse_extra_kwargs[0] = new_extra_kwargs

    def __exit__(self,*a,**kw):
        _argparse_postinitfct[0] = self.__orig_postinitfct
        _argparse_extra_kwargs[0] = self.__orig_extra_kwargs

_cli_entry_points_called_from_actual_cmdline=[True]
class _cli_call_from_pyapi_ctx:
    def __enter__(self):
        self.__orig = _cli_entry_points_called_from_actual_cmdline[0]
        _cli_entry_points_called_from_actual_cmdline[0] = False
    def __exit__(self,*a,**kw):
        _cli_entry_points_called_from_actual_cmdline[0] = self.__orig

def cli_entry_point(func):
    def mainfct( argv = None ):
        #Called from python-API:
        if not _cli_entry_points_called_from_actual_cmdline[0]:
            assert argv, "Empty argv not ok when calling main in PyAPI mode"
            import os
            progname = argv[0]
            assert progname and isinstance(progname,str)
            assert '/' not in progname and '\\' not in progname
            assert '.' not in progname and '~' not in progname
            arglist = argv[1:]
            return func( progname, arglist )
        #Called from actual command-line, so we translate warnings and
        #exceptions to more suitable printouts:
        from ._common import WarningSpy
        from .exceptions import NCException
        if argv is None:
            import sys
            argv = sys.argv[:]
        assert argv
        import os
        progname = os.path.basename(argv[0])
        arglist = argv[1:]
        def block_warnings(msg_str, cat_str):
            if cat_str == 'NCrystalUserWarning':
                print('WARNING: %s'%msg_str)
                return True
            return False
        with WarningSpy(blockfct=block_warnings):
            try:
                func( progname, arglist )
            except NCException as e:
                n=e.__class__.__name__
                if n.startswith('NC'):
                    n = n[2:]
                raise SystemExit('%s ERROR: %s'%(n,e)) from e
            except Exception as e:
                raise SystemExit('ERROR: %s'%(e)) from e
    return mainfct

def _resolve_cmd_and_import_climod( cmdname, arguments ):
    #fixme to _cliimpl.py
    resolved_cmd = cli_tool_lookup_impl( cmdname )
    if resolved_cmd is None:
        from .exceptions import NCBadInput
        raise NCBadInput(f'Command line tool name "{cmdname}" not recognised')
    argv = [resolved_cmd['canonical_name']] + [a for a in arguments]

    import importlib
    clipymodname = '_cli_%s'%resolved_cmd['short_name']
    climod = importlib.import_module(f'..{clipymodname}', __name__)
    assert hasattr(climod,'main')
    return climod, argv


def cli_tool_list_impl( canonical_names = True  ):
    # Implementation of cliutils.cli_tool_list
    import pathlib
    short_names = [ f.name[5:-3] for f in
                    pathlib.Path(__file__).parent.glob('_cli_*.py') ]
    short_names.sort()
    if short_names:
        return short_names
    else:
        return [ _map_shortname_2_canonical_name(sn) for sn in short_names ]

def cli_tool_lookup_impl( name ):
    # Implementation of cliutils.cli_tool_lookup

    #Note: We basically have to treat only ncrystal-config and nctool as special
    #cases.
    if name == 'ncrystal-config':
        #Special case:
        #FIXME: We don't actually have such a script here yet!
        short_name = 'config'
    elif name.startswith('ncrystal_'):
        short_name = name[9:]
    else:
        short_name = name
    if short_name not in cli_tool_list_impl( canonical_names=False ):
        return None
    return dict( short_name = short_name,
                 canonical_name = _map_shortname_2_canonical_name(short_name) )

def _map_shortname_2_canonical_name( short_name ):
    return { 'config' : 'ncrystal-config',
             'nctool' : 'nctool' }.get( short_name,
                                        f'ncrystal_{short_name}' )

def _fix_argparse_action_group_title( parser, oldtitle, newtitle ):
    _ags = getattr(parser,'_action_groups',[])
    if any( getattr(ag,'title','')==newtitle for ag in _ags):
        return
    for ag in _ags:
        if getattr(ag,'title','') == oldtitle:
            setattr(ag,'title',newtitle)
            return
