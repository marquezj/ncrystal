#include "NCrystal/NCDefs.hh"
#include <iostream>

namespace NC = NCrystal;

int main()
{
  using OptStr = NC::Optional<std::string>;
  auto pr = [](const OptStr& s) {
    if ( s.has_value() )
      std::cout<<"OptString("<<s.value()<<")"<<std::endl;
    else
      std::cout<<"OptString-NOVALUE"<<std::endl;
  };
  //  std::string sss{5,'e'};
  //  NC::Optional<std::string> s("sdfsdf");
  NC::Optional<std::string> s;
  pr(s);
  s.emplace(5,'e');
  pr(s);
  s.emplace(70,'e');
  pr(s);
  s = NC::NullOpt;
  pr(s);
  s = "sdfsdfsdfsf sd fsd fsd f sf sd fs df sdf sf sdfddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd";
  pr(s);
  s = OptStr("2222sdfsdfsdfsf sd fsd fsd f sf sd fs df sdf sf sdfddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd");
  pr(s);
  OptStr s2("3333sdfsdfsdfsf sd fsd fsd f sf sd fs df sdf sf sdfddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd");
  pr(s2);
  s = std::move(s2);
  pr(s2);
  pr(s);
  OptStr s3(std::string(50,'a'));
  pr(s3);
  OptStr s4(OptStr("hej"));
  pr(s4);
  OptStr s5 = OptStr(NC::NullOpt);
  pr(s5);

  OptStr s4b(s4);
  pr(s4b);
  OptStr s5b(s5);
  pr(s5b);

  OptStr s4c(std::move(s4));
  pr(s4c);
  OptStr s5c(std::move(s5));
  pr(s5c);

  OptStr s4d;
  s4d = std::move(s4c);
  pr(s4c);
  OptStr s5d;
  s5d = std::move(s5c);
  pr(s5c);

  return 0;
}
