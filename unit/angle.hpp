#include <ratio>
#include <iostream>

namespace angle{
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
}
