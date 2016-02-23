#ifndef push_button_h
#define push_button_h
#include <Wire.h>

class PushButton {
  static const int DOWN_STATE = 0x0;
  int state = 0x1;
  int pin;
  unsigned long last_read;

public:
  PushButton(const int _pin) {
    this->pin = _pin;
  };

  void init() {
    pinMode(this->pin, INPUT);
  };

  const bool pressed() {
    const int current = poll();
    // transition from down to up
    if(current != this->state && current != this->DOWN_STATE) {
      this->state = current;
      return true;
    }
    this->state = current;
    return false;
  };

private:
  const int poll() {
    if(millis() - this->last_read > 200) {
      this->last_read = millis();
      return digitalRead(this->pin);
    } else {
      return this->state;
    }
  };
};

#endif
