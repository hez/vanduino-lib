#ifndef thermostat_h
#define thermostat_h
#include <Time.h>
#include <TimeLib.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <ExtendedDHT.h>

#include <PushButton.h>

// time between screen refreshes in seconds
#define TIME_BETWEEN_TEMP_DISPLAY 3
#define DEG_CHAR (char)223

#define NO_TARGET_TEMPERATURE -1
#define DISPLAYED_TEMPERATURE 1
#define DISPLAYED_TARGET_TEMPERATURE 2

class Thermostat {
  ExtendedDHT sensor;
  LiquidCrystal_I2C display;
  PushButton up_button;
  PushButton down_button;
  PushButton left_button;

  unsigned long last_temp_display = 0;
  int target_temperature = -1;
  unsigned int last_displayed = 0;

public:
  Thermostat(int display_address, int sensor_pin, int sensor_type) :
      display(display_address, 2, 3, 0, 4, 5, 6, 7, 3, POSITIVE),
      sensor(sensor_pin, sensor_type),
      up_button(13),
      down_button(12),
      left_button(11)
  {
    initLcd();
    initSensor();
    initButtons();
  };

  void loop() {
    const time_t cur_timestamp = now();
    if(this->up_button.pressed()) {
      Serial.println("up button pressed");
      if(this->target_temperature == NO_TARGET_TEMPERATURE) {
        this->target_temperature = 16;
      } else {
        this->target_temperature += 1;
      }
      this->last_temp_display = cur_timestamp; // reset temp display
      displayTargetTemperature();
    }
    if(this->down_button.pressed()) {
      Serial.println("down button pressed");
      if(this->target_temperature == NO_TARGET_TEMPERATURE) {
        this->target_temperature = 16;
      } else {
        this->target_temperature -= 1;
      }
      this->last_temp_display = cur_timestamp; // reset temp display
      displayTargetTemperature();
    }
    if(this->left_button.pressed()) {
      Serial.println("left button pressed");
      this->target_temperature = NO_TARGET_TEMPERATURE;
      this->last_temp_display = cur_timestamp - TIME_BETWEEN_TEMP_DISPLAY; // reset temp display
    }

    if(cur_timestamp - this->last_temp_display >= TIME_BETWEEN_TEMP_DISPLAY) {
      this->last_temp_display = cur_timestamp;
      displayTemperature();
    }
  }

private:
  void initLcd() {
    Serial.println("here in initlcd");
    this->display.begin(20,4);
    // ------- Quick 3 blinks of backlight  -------------
    for(int i = 0; i< 3; i++)
    {
      this->display.backlight();
      delay(150);
      this->display.noBacklight();
      delay(150);
    }
    this->display.backlight(); // finish with backlight on
  };

  void initSensor() {
    this->sensor.begin();
  };

  void initButtons() {
    up_button.init();
  };

  void displayTemperature() {
    // Do we require a full screen refresh?
    //    Don't do a full screen refresh unless we have to, doing so each time
    //    causes noticable flicker as the lcd doesn't redraw that quickly.
    bool full_display = this->last_displayed != DISPLAYED_TEMPERATURE;

    // First line
    if(full_display) {
      this->display.clear();
      this->display.print("Temperature: ");
    } else {
      this->display.setCursor(13,0);
    }
    this->display.print(round(this->sensor.getTemperature()));
    this->display.print(DEG_CHAR);

    // Second line
    if(full_display) {
      this->display.setCursor(0, 1);
      this->display.print("Humidity: ");
    } else {
      this->display.setCursor(10, 1);
    }
    this->display.print(round(this->sensor.getHumidity()));
    this->display.print("%  ");

    // Fourth line
    if(full_display) {
      this->display.setCursor(0, 3);
      this->display.print("Heater: ");
    } else {
      this->display.setCursor(8, 3);
    }
    if(this->target_temperature == NO_TARGET_TEMPERATURE) {
      this->display.print("off");
    } else {
      this->display.print(this->target_temperature);
      this->display.print(DEG_CHAR);
    }
    // Set last displayed for next go around
    this->last_displayed = DISPLAYED_TEMPERATURE;
  };

  void displayTargetTemperature() {
    this->display.clear();
    this->display.setCursor(0, 1);
    this->display.print("Target: ");
    this->display.print(this->target_temperature);
    this->last_displayed = DISPLAYED_TARGET_TEMPERATURE;
  };
};

#endif
