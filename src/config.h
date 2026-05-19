#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include <stdint.h>

#include "array.h"
#include "color.h"
#include "decay_table.h"
#include "pixel_format.h"

namespace config {
  // LED strip

  inline constexpr size_t leds = 32;

  using LedStrip = ws2812<B, 1>;
  using Color = PixelFormat<extract_pixel_format((LedStrip*)0)>;
  using ColorChannel = decltype(Color::red);

  inline constexpr ColorChannel min_brightness = 7;
  inline constexpr ColorChannel max_brightness = 255;
  inline constexpr ColorChannel initial_brightness = max_brightness / 2;

  inline constexpr bool interpolate = true;
  inline constexpr uint8_t color_border_width = 1;

  inline constexpr uint16_t ms_per_update = 0;

  inline constexpr auto rgb(uint8_t r, uint8_t g, uint8_t b) {
    return color::rgb<Color>(r, g, b);
  }

  inline constexpr auto colors PROGMEM = make_static_array_2d(
    make_static_array(
      rgb(192,  75, 255),
      rgb(168, 255,  75)
    ),
    make_static_array(
      rgb( 75, 196, 255),
      rgb(255,  70,  70)
    ),
    make_static_array(
      rgb(192,  75, 255),
      rgb( 75, 196, 255),
      rgb(168, 255,  75),
      rgb(255,  70,  70)
    )
  );

  inline constexpr auto ms_per_frame PROGMEM = make_static_array(
    50u, 100u, 200u, 500u, 1000u, 20u
  );

  // Decay table

  using DecayValue = uint16_t;
  inline constexpr DecayValue decay_precision = 1000;
  inline constexpr ColorChannel decayed_brightness = 2;

  inline constexpr DecayTable<
    DecayValue, decay_precision,
    min_brightness, max_brightness,
    colors.min_row_size(), colors.max_row_size()
  > PROGMEM decay_table(leds, color_border_width, decayed_brightness);

  // Buttons

#define BUTTON_PORT B
#define BUTTON_INTERRUPT_VECTOR 0

  inline constexpr uint8_t button_debounce = 100;

  inline constexpr uint8_t brightness_pin = 0;
  inline constexpr uint8_t speed_pin = 2;
  inline constexpr uint8_t color_pin = 3;

#ifdef BUTTON_PORT
  inline constexpr uint8_t brightness_bit = 1 << brightness_pin;
  inline constexpr uint8_t speed_bit = 1 << speed_pin;
  inline constexpr uint8_t color_bit = 1 << color_pin;
  inline constexpr uint8_t button_mask = brightness_bit | speed_bit | color_bit;
  inline constexpr uint8_t button_interrupt_mask = button_mask;
#else
  inline constexpr uint8_t brightness_bit = 1 << 0;
  inline constexpr uint8_t speed_bit = 1 << 1;
  inline constexpr uint8_t color_bit = 1 << 2;
#endif
}

#endif // CONFIG_H_INCLUDED
