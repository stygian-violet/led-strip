#ifndef LED_STRIP_H_INCLUDED
#define LED_STRIP_H_INCLUDED

#include <avr/pgmspace.h>

#include "color.h"

template<size_t num_leds, color::RGB Color, typename DT>
class LedStripState {
public:
  using Index = Uint<minsizeof(num_leds)>;
  using ColorChannel = decltype(Color::red);

protected:
  Color frame[num_leds];

  const DT& decay;
  const Color *pgm_colors = 0;

  bool interpolate = true;
  uint8_t color_border_width = 1;

  ColorChannel brightness = 0;
  Index num_colors = 0;
  Index offset = 0;

  uint32_t ms_per_frame = 1;
  uint32_t last_frame_ms = 0;

  void generate_frame();
  void interpolate_frame(const Fraction& f);

public:
  LedStripState(const DT& decay) : decay(decay) {}

  ColorChannel get_brightness() const { return brightness; }
  void set_brightness(ColorChannel value) { brightness = value; }
  void set_ms_per_frame(uint32_t ms) { ms_per_frame = ms; }
  void set_colors(const Color *pgm_c, Index n) { pgm_colors = pgm_c; num_colors = n; }
  void set_colors(const Pair<const Color*, size_t>& p) { pgm_colors = p.first; num_colors = p.second; }
  void set_color_border_width(uint8_t w) { color_border_width = w; }
  void set_interpolation(bool b) { interpolate = b; }

  const Color* update(uint32_t ms);
};

template<size_t num_leds, color::RGB Color, typename DT>
const Color* LedStripState<num_leds, Color, DT>::update(uint32_t ms) {
  if (ms - last_frame_ms >= ms_per_frame) {
    offset = (offset + num_leds - 1) % num_leds;
    last_frame_ms = ms;
  } else if (!interpolate) {
    return 0;
  }
  generate_frame();
  if (interpolate) {
    interpolate_frame(Fraction(ms - last_frame_ms, ms_per_frame));
  }
  return frame;
}

template<size_t num_leds, color::RGB Color, typename DT>
void LedStripState<num_leds, Color, DT>::generate_frame() {
  using Brightness = Uint<sizeof(brightness) * 2>;
  constexpr uint8_t brightness_shift =
    (sizeof(Brightness) - sizeof(brightness)) * 4;

  Index leds_per_color = num_leds / num_colors;
  Index leds_per_color_rem = num_leds % num_colors;
  Index decay_start = 2 * color_border_width + 1;
  Index x = offset;
  Fraction decay_factor = decay(brightness, num_colors);

  for (Index c = 0; c < num_colors; ++c) {
    Color color = color::white<Color>();
    Brightness current_brightness = (Brightness(brightness) << brightness_shift);
    for (Index i = 0; i <= leds_per_color; ++i) {
      if (i == leds_per_color && c >= leds_per_color_rem) {
        break;
      }
      if (i == color_border_width) {
        // color = colors[c];
        memcpy_P(&color, &pgm_colors[c], sizeof(color));
      }
      if (i >= decay_start) {
        current_brightness *= decay_factor;
      }
      frame[x++ % num_leds] = color::with_brightness(
        color,
        current_brightness >> brightness_shift
      );
    }
  }
}

template<size_t num_leds, color::RGB Color, typename DT>
void LedStripState<num_leds, Color, DT>::interpolate_frame(const Fraction& f) {
  Color first = frame[0];
  for (Index i = 0; i < num_leds; ++i) {
    frame[i] = color::mix(
      frame[i],
      i == num_leds - 1 ? first : frame[i + 1],
      f
    );
  }
}

#endif // LED_STRIP_H_INCLUDED
