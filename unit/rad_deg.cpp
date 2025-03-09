#include "angle.hpp"


    using MphRatio = std::ratio<10, 36>;

    using Mps = angle::Angle<MphRatio>;
    using Kph = angle::Angle<std::ratio<1>>;

int main(){

  angle::Deg d{180};

  std::cout << d.Get() << std::endl;;

  Kph k{100};

  Mps m = k;

  Kph kp = m;
  std::cout << k.Get() << " " << m.Get() << " " << kp.Get();

  angle::Rad v = d;


  return 0;
}
