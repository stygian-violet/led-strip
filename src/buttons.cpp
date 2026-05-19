#include "buttons.h"

#include "config.h"

void Buttons::input(uint8_t st, uint32_t ms) {
  uint8_t sreg = SREG;
  cli();
  if (next != st) {
    next = st;
    time = ms;
  }
  SREG = sreg;
}

uint8_t Buttons::update(uint32_t ms) {
  uint8_t sreg = SREG;
  cli();
  uint8_t next_ = next;
  uint8_t pressed = 0;
  if (next_ != state && ms - time >= debounce) {
    pressed = ~state & next_;
    state = next_;
  }
  SREG = sreg;
  return pressed;
}
