#ifndef relay_h
#define relay_h
#include <Wire.h>

class Relay {
  int pin;

public:
  Relay(const int pin) {
    this->pin = pin;
  };

  void init() {
    pinMode(this->pin, OUTPUT);
  };

  void turn_on() {
#ifdef DEBUG
    Serial.println("Relay on");
#endif
    digitalWrite(this->pin, LOW);
  };

  void turn_off() {
#ifdef DEBUG
    Serial.println("Relay off");
#endif
    digitalWrite(this->pin, HIGH);
  };
};

#endif
