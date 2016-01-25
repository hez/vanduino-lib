#ifndef push_button_h
#define push_button_h
#include <Wire.h>

class PushButton {
  static const int DOWN_STATE = 0x1;
  int state = 0x1;
  int pin;
  unsigned long last_read;

public:
  PushButton(int _pin) {
    this->pin = _pin;
  };

  void init() {
    pinMode(this->pin, INPUT);
  };

  bool pressed() {
    int current = poll();
    /*Serial.print("button is "); Serial.print(current);
    Serial.print(" was "); Serial.print(this->state);
    Serial.print(" reading from "); Serial.print(this->pin);
    Serial.println("");*/
    if(current != this->state && current == this->DOWN_STATE) {
      this->state = current;
      return true;
    }
    this->state = current;
    return false;
  };

private:
  int poll() {
    if(millis() - this->last_read > 200) {
      this->last_read = millis();
      return digitalRead(this->pin);
    } else {
      return this->state;
    }
  };
};

#endif
