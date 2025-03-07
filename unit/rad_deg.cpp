#include <ratio>
#include <iostream>

template <class Ratio>
struct Angle{
  static constexpr auto ratio = 
    static_cast<float>(Ratio::num) / static_cast<float>(Ratio::den);

  constexpr Angle() noexcept = default;
  explicit constexpr Angle(float r) noexcept : val{r} {}

  template <class S>
  constexpr Angle(const Angle<S>& r) noexcept :val{r.Get() / r.ratio * ratio}{}

  [[nodiscard]] constexpr float Get() const noexcept {return val;}
  explicit constexpr operator float() const noexcept {return val;}

private:
  float val = 0;
};

using PiRatio = std::ratio<66627445582888887, 21208174723389167>;

using RadR = std::ratio<PiRatio::num, PiRatio::den * 180>;
using DegR = std::ratio<1>;

using Rad = Angle<RadR>;
using Deg = Angle<DegR>;


template<class T>
float foo(const float param) {
    return param * T::num / static_cast<float>(T::den);
}


int main(){
 
  Deg d{180};
  Rad v = d;
  Deg dd = v;
  std::cout << d.Get() << ">" <<  v.Get()  << ">" << dd.Get() << std::endl;
  std::cout << static_cast<float>(d)  << ">" 
            << static_cast<float>(v)  << ">"
            << static_cast<float>(dd) << std::endl;

auto x = foo<std::ratio<4,7>>(42);
  std::cout << x << std::endl;
  
  return 0;
}
