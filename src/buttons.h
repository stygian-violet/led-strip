#ifndef BUTTONS_H_INCLUDED
#define BUTTONS_H_INCLUDED

#include <stdint.h>

class Buttons {
protected:
  uint8_t debounce;
  uint8_t state = 0;
  volatile uint8_t next = 0;
  volatile uint32_t time = 0;
public:
  Buttons(uint8_t debounce) : debounce(debounce) {}
  void begin(uint8_t initial_state) { next = state = initial_state; }
  void input(uint8_t st, uint32_t ms);
  uint8_t update(uint32_t ms);
};

#endif // BUTTONS_H_INCLUDED
