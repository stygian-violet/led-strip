#include "buttons.h"
#include "config.h"
#include "led_strip_state.h"

LedStripState<
  config::leds,
  config::Color,
  decltype(config::decay_table)
> ledstate(config::decay_table);
config::LedStrip leds;
Buttons buttons(config::button_debounce);

uint8_t color_set = 0;
uint8_t ms_per_frame = 0;

#ifdef BUTTON_PORT
void update_button_input() {
  buttons.input(~PIN(BUTTON_PORT) & config::button_mask, millis());
}

#ifdef BUTTON_INTERRUPT_VECTOR
ISR(PCINTx_vect(BUTTON_INTERRUPT_VECTOR)) {
  update_button_input();
}
#endif

void setup_buttons() {
  cli();
  DDR(BUTTON_PORT) &= ~config::button_mask;
  PORT(BUTTON_PORT) |= config::button_mask;
#ifdef BUTTON_INTERRUPT_VECTOR
#if defined(GIMSK) && defined(PCIE)
  GIMSK |= 1 << PCIE;
  PCMSK |= config::button_interrupt_mask;
#elif defined(PCICR)
  PCICR |= 1 << BUTTON_INTERRUPT_VECTOR;
  PCMSKx(BUTTON_INTERRUPT_VECTOR) |= config::button_interrupt_mask;
#else
#error Pin change interrupt setup is not implemented for this board
#endif
#endif // BUTTON_INTERRUPT_VECTOR
  buttons.begin(~PIN(BUTTON_PORT));
  sei();
}
#else
#undef BUTTON_INTERRUPT_VECTOR

uint8_t read_button_state() {
  uint8_t state = 0;
  if (!digitalRead(config::brightness_pin)) {
    state |= config::brightness_bit;
  }
  if (!digitalRead(config::speed_pin)) {
    state |= config::speed_bit;
  }
  if (!digitalRead(config::color_pin)) {
    state |= config::color_bit;
  }
  return state;
}

void update_button_input() {
  buttons.input(read_button_state(), millis());
}

void setup_buttons() {
  pinMode(config::brightness_pin, INPUT_PULLUP);
  pinMode(config::speed_pin, INPUT_PULLUP);
  pinMode(config::color_pin, INPUT_PULLUP);
  buttons.begin(read_button_state());
}
#endif // BUTTON_PORT

void change_brightness() {
  auto brightness = ledstate.get_brightness();
  if ((brightness >>= 1) < config::min_brightness) {
    brightness = config::max_brightness;
  }
  ledstate.set_brightness(brightness);
}

void change_speed() {
  ms_per_frame = (ms_per_frame + 1) % config::ms_per_frame.size();
  ledstate.set_ms_per_frame(config::ms_per_frame.read_P(ms_per_frame));
}

void change_colors() {
  color_set = (color_set + 1) % config::colors.rows();
  ledstate.set_colors(config::colors.read_P(color_set));
}

void setup() {
  setup_buttons();
  ledstate.set_colors(config::colors.read_P(color_set));
  ledstate.set_interpolation(config::interpolate);
  ledstate.set_color_border_width(config::color_border_width);
  ledstate.set_brightness(config::initial_brightness);
  ledstate.set_ms_per_frame(config::ms_per_frame.read_P(ms_per_frame));
  leds.clear(config::leds);
}

void loop() {
  uint32_t t = millis();

#ifndef BUTTON_INTERRUPT_VECTOR
  update_button_input();
#endif
  uint8_t pressed = buttons.update(t);
  if (pressed & config::brightness_bit) {
    change_brightness();
  }
  if (pressed & config::speed_bit) {
    change_speed();
  }
  if (pressed & config::color_bit) {
    change_colors();
  }

  auto frame = ledstate.update(t);
  if (frame) {
    leds.sendPixels(config::leds, frame);
  }

  if constexpr (config::ms_per_update > 0) {
    t = millis() - t;
    if (t < config::ms_per_update) {
      delay(config::ms_per_update - t);
    }
  }
}
