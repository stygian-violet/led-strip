#ifndef COLOR_H_INCLUDED
#define COLOR_H_INCLUDED

#include "common.h"

namespace color {
  template<typename T>
  concept RGB = requires(T t) {
    t.red;
    t.green;
    t.blue;
  };

  template<typename T>
  concept HRGB = RGB<T> && requires(T t) {
    t.brightness;
  };

  template<typename T>
  concept RGBW = RGB<T> && !HRGB<T> && requires(T t) {
    t.white;
  };

  template<RGB T> constexpr bool has_brightness() { return false; }
  template<HRGB T> constexpr bool has_brightness() { return true; }

  template<RGB T> constexpr bool has_white() { return false; }
  template<RGBW T> constexpr bool has_white() { return true; }

  template<typename T>
  constexpr T channel(uint8_t value) {
    return T(value) << ((sizeof(T) - sizeof(value)) * 8);
  }

  template<typename T>
  constexpr T channel_max() {
    return T(-1);
  }

  template<RGB Color>
  constexpr Color white() {
    Color res;
    res.red = channel_max<decltype(res.red)>();
    res.green = channel_max<decltype(res.green)>();
    res.blue = channel_max<decltype(res.blue)>();
    if constexpr (has_brightness<Color>()) {
      res.brightness = channel_max<decltype(res.brightness)>();
    }
    return res;
  }

  template<RGBW Color>
  constexpr Color white() {
    Color res;
    res.red = 0;
    res.green = 0;
    res.blue = 0;
    res.white = channel_max<decltype(res.white)>();
    return res;
  }

  template<RGB Color>
  Color with_brightness(
    const Color& src,
    decltype(src.red) brightness,
    decltype(src.red) prev_brightness = channel_max<decltype(src.red)>()
  ) {
    Color dst;
    Fraction f(brightness, prev_brightness);
    dst.red = src.red * f;
    dst.green = src.green * f;
    dst.blue = src.blue * f;
    if constexpr (has_white<Color>()) {
      dst.white = src.white * f;
    }
    return dst;
  }

  template<HRGB Color>
  Color with_brightness(
    const Color& src,
    decltype(src.brightness) brightness,
    decltype(src.brightness) prev_brightness = channel_max<decltype(src.brightness)>()
  ) {
    Color dst;
    Fraction f(brightness, prev_brightness);
    dst.red = src.red;
    dst.green = src.green;
    dst.blue = src.blue;
    dst.brightness = src.brightness * f;
    return dst;
  }

  template<RGB Color>
  Color min(const Color& src, const Color& src2) {
    Color dst;
    dst.red = ::min(src.red, src2.red);
    dst.green = ::min(src.green, src2.green);
    dst.blue = ::min(src.blue, src2.blue);
    if constexpr (has_white<Color>()) {
      dst.white = ::min(src.white, src2.white);
    }
    if constexpr (has_brightness<Color>()) {
      dst.brightness = ::min(src.brightness, src2.brightness);
    }
    return dst;
  }

  template<RGB Color>
  Color max(const Color& src, const Color& src2) {
    Color dst;
    dst.red = ::max(src.red, src2.red);
    dst.green = ::max(src.green, src2.green);
    dst.blue = ::max(src.blue, src2.blue);
    if constexpr (has_white<Color>()) {
      dst.white = ::max(src.white, src2.white);
    }
    if constexpr (has_brightness<Color>()) {
      dst.brightness = ::max(src.brightness, src2.brightness);
    }
    return dst;
  }

  template<RGB Color>
  Color mix(
    const Color& src,
    const Color& src2,
    const Fraction& f
  ) {
    Color dst;
    dst.red = ::mix(src.red, src2.red, f);
    dst.green = ::mix(src.green, src2.green, f);
    dst.blue = ::mix(src.blue, src2.blue, f);
    if constexpr (has_white<Color>()) {
      dst.white = ::mix(src.white, src2.white, f);
    }
    if constexpr (has_brightness<Color>()) {
      dst.brightness = ::mix(src.brightness, src2.brightness, f);
    }
    return dst;
  }

  template<RGB Color>
  constexpr Color rgb(uint8_t red, uint8_t green, uint8_t blue) {
    Color res;
    if constexpr (has_white<Color>()) {
      uint8_t white = ::min(::min(red, green), blue);
      red -= white;
      green -= white;
      blue -= white;
      res.white = channel<decltype(res.white)>(white);
    }
    res.red = channel<decltype(res.red)>(red);
    res.green = channel<decltype(res.green)>(green);
    res.blue = channel<decltype(res.blue)>(blue);
    if constexpr (has_brightness<Color>()) {
      res.brightness = channel_max<decltype(res.brightness)>();
    }
    return res;
  }
};

#endif // COLOR_H_INCLUDED
