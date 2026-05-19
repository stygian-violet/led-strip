#ifndef DECAY_TABLE_H_INCLUDED
#define DECAY_TABLE_H_INCLUDED

#include <math.h>

#include "common.h"

template<
  typename T, T precision,
  size_t min_brightness, size_t max_brightness,
  size_t min_colors, size_t max_colors
> class DecayTable {
public:
  static constexpr uint8_t min_brightness_level = ilog2(min_brightness);
  static constexpr uint8_t max_brightness_level = ilog2(max_brightness);

  static_assert(max_brightness_level >= min_brightness_level);
  static_assert(max_colors >= min_colors);

  static constexpr size_t rows = max_brightness_level - min_brightness_level + 1;
  static constexpr size_t cols = max_colors - min_colors + 1;
  static constexpr size_t size = rows * cols;

protected:
  T data[size];

public:
  constexpr DecayTable(
    uint8_t leds,
    uint8_t border,
    uint8_t decayed_brightness
  ) : data() {
    size_t i = 0;
    for (uint8_t b = min_brightness_level; b <= max_brightness_level; ++b) {
      uint16_t brightness = (1u << (b + 1)) - 1;
      double k = log(double(decayed_brightness) / brightness);
      for (size_t colors = min_colors; colors <= max_colors; ++colors) {
        size_t length = leds / colors - (2 * border + 1);
        double value = 0.99;
        if (length > 0 && length <= leds) {
          value = exp(k / length);
        }
        data[i++] = value * precision;
      }
    }
  }

  Fraction operator()(size_t brightness, size_t colors) const {
    brightness = clamp(
      ilog2(brightness),
      min_brightness_level,
      max_brightness_level
    ) - min_brightness_level;
    colors = clamp(colors, min_colors, max_colors) - min_colors;
    size_t i = brightness * cols + colors;
    T num;
    memcpy_P(&num, &data[i], sizeof(T));
    return Fraction(num, precision);
  }
};

#endif // DECAY_TABLE_H_INCLUDED
