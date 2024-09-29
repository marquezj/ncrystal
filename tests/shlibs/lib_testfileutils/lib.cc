#include <cstring>
#include "NCrystal/NCDefs.hh"
#include "NCrystal/internal/NCFileUtils.hh"

namespace NC = NCrystal;

extern "C" const char * nctest_ctypes_dictionary()
{
  return
    "int nctest_file_exists( const char * );"
    "const char * nctest_ncgetcwd();"
    ;
}

//////////////////////////////////////////////////////
//FIXME: To common test header:
#include <iostream>
#include <cstdlib>

namespace nctest {
  inline void printErrAndExit(const std::exception &e) noexcept(true) {

      const std::runtime_error* stdrte = dynamic_cast<const std::runtime_error*>(&e);
      if (stdrte)
        std::cout<<"NCTest ERROR (std::runtime_error): "<<stdrte->what()<<std::endl;
      else
        std::cout<<"NCTest ERROR (unknown)"<<std::endl;
      std::exit(1);
  }
}
#define NCCATCH catch (std::exception& e) { nctest::printErrAndExit(e); }
//////////////////////////////////////////////////////


extern "C" int nctest_file_exists( const char * f )
{
  int res;
  try {
    nc_assert_always( f );
    res = NC::file_exists( f ) ? 1 : 0;
  } NCCATCH;
  return res;
}

extern "C" const char * nctest_ncgetcwd()
{
  //For testing, we can get away with just using a large static buffer:
  static char buf[16384];
  try {
    std::string thecwd = NC::ncgetcwd();
    buf[0] = '\0';
    std::strncat(buf,thecwd.c_str(),sizeof(buf)-1);
  } NCCATCH;
  return &buf[0];
}

