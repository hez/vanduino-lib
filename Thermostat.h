#ifndef thermostat_h
#define thermostat_h
#include <Time.h>
#include <TimeLib.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <ExtendedDHT.h>

#include <PushButton.h>
#include <Relay.h>

// time between screen refreshes in seconds
#define DISPLAY_NUMBER_OF_COLUMNS 20
#define DISPLAY_NUMBER_OF_ROWS 4
#define TIME_BETWEEN_TEMP_DISPLAY 3
#define DEG_CHAR (char)223

#define FURNACE_OFF 0
#define MINUMUM_FURNACE_RUN 60//*30     // minumum run for furnance is 30min
#define NO_TARGET_TEMPERATURE -1
#define START_TARGET_TEMPERATURE 16   // start temperature for setting target is 16deg
#define DISPLAYED_TEMPERATURE 1
#define DISPLAYED_TARGET_TEMPERATURE 2

class Thermostat {
  ExtendedDHT sensor;
  LiquidCrystal_I2C display;
  PushButton up_button;
  PushButton down_button;
  PushButton left_button;
  Relay furnace_relay;

  time_t last_temp_display = 0;
  time_t furnace_on_at = FURNACE_OFF;
  short target_temperature = NO_TARGET_TEMPERATURE;
  unsigned int last_displayed = 0;

public:
  Thermostat(const int display_address, const int sensor_pin, const int sensor_type) :
      display(display_address, 2, 3, 0, 4, 5, 6, 7, 3, POSITIVE),
      sensor(sensor_pin, sensor_type),
      up_button(13),
      down_button(12),
      left_button(11),
      furnace_relay(8)
  {
    initLcd();
    initSensor();
    initButtons();
    initRelays();
  };

  void loop() {
    const time_t cur_timestamp = now();
    if(this->up_button.pressed()) {
#ifdef DEBUG
      Serial.println("up button pressed");
#endif
      if(this->target_temperature == NO_TARGET_TEMPERATURE) {
        this->target_temperature = START_TARGET_TEMPERATURE;
      } else {
        this->target_temperature += 1;
      }
      this->last_temp_display = cur_timestamp; // reset temp display
      ensureFurnaceStarted(cur_timestamp);
      displayTargetTemperature();
    }
    if(this->down_button.pressed()) {
#ifdef DEBUG
      Serial.println("down button pressed");
#endif
      if(this->target_temperature == NO_TARGET_TEMPERATURE) {
        this->target_temperature = START_TARGET_TEMPERATURE;
      } else {
        this->target_temperature -= 1;
      }
      this->last_temp_display = cur_timestamp; // reset temp display
      ensureFurnaceStarted(cur_timestamp);
      displayTargetTemperature();
    }
    if(this->left_button.pressed()) {
#ifdef DEBUG
      Serial.println("left button pressed");
#endif
      this->target_temperature = NO_TARGET_TEMPERATURE;
      this->last_temp_display = cur_timestamp - TIME_BETWEEN_TEMP_DISPLAY; // reset temp display
    }

    if(cur_timestamp - this->last_temp_display >= TIME_BETWEEN_TEMP_DISPLAY) {
      this->last_temp_display = cur_timestamp;
      displayTemperature();
    }

    furnaceShutdown(cur_timestamp);
  }

private:
  void initLcd() {
#ifdef DEBUG
    Serial.println("here in initlcd");
#endif
    this->display.begin(DISPLAY_NUMBER_OF_COLUMNS, DISPLAY_NUMBER_OF_ROWS);
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
    this->up_button.init();
  };

  void initRelays() {
    this->furnace_relay.init();
    this->furnace_relay.turn_off();
  };

  void ensureFurnaceStarted(const time_t current_time) {
    if(this->furnace_on_at == FURNACE_OFF) {
#ifdef DEBUG
      Serial.print("furnace turned on at: ");
      Serial.print(current_time);
      Serial.println();
#endif
      this->furnace_on_at = current_time;
      this->furnace_relay.turn_on();
    }
  };

  void furnaceShutdown(const time_t current_time) {
    // Only try shutting down if furnace is actually running.
    if(this->furnace_on_at == FURNACE_OFF)
      return;

    // Shut down furnace if no target temperature and has ran long enough
    if(this->target_temperature == NO_TARGET_TEMPERATURE && current_time - this->furnace_on_at > MINUMUM_FURNACE_RUN) {
#ifdef DEBUG
      Serial.println("Shutting down furnace");
#endif
      this->furnace_on_at = FURNACE_OFF;
      this->furnace_relay.turn_off();
    }
  };

  void displayTemperature() {
    // Do we require a full screen refresh?
    //    Don't do a full screen refresh unless we have to, doing so each time
    //    causes noticable flicker as the lcd doesn't redraw that quickly.
    const bool full_display = this->last_displayed != DISPLAYED_TEMPERATURE;

    // First line
    if(full_display) {
      this->display.clear();
      this->display.print(F("Temperature: "));
    } else {
      this->display.setCursor(13, 0);
    }
    this->display.print(round(this->sensor.getTemperature()));
    this->display.print(DEG_CHAR);

    // Second line
    if(full_display) {
      this->display.setCursor(0, 1);
      this->display.print(F("Humidity: "));
    } else {
      this->display.setCursor(10, 1);
    }
    this->display.print(round(this->sensor.getHumidity()));
    this->display.print(F("%  "));

    // Fourth line
    if(full_display) {
      this->display.setCursor(0, 3);
      this->display.print(F("Heater: "));
    } else {
      this->display.setCursor(8, 3);
    }
    if(this->furnace_on_at == FURNACE_OFF) {
      this->display.print(F("off        "));
    } else {
      if(this->target_temperature == NO_TARGET_TEMPERATURE) {
        this->display.print(F("turning off"));
      } else {
        this->display.print(F("on ("));
        this->display.print(this->target_temperature);
        this->display.print(DEG_CHAR);
        this->display.print(F(")  "));
      }
    }
    // Set last displayed for next go around
    this->last_displayed = DISPLAYED_TEMPERATURE;
  };

  void displayTargetTemperature() {
    this->display.clear();
    this->display.setCursor(0, 1);
    this->display.print(F("Target: "));
    this->display.print(this->target_temperature);
    this->last_displayed = DISPLAYED_TARGET_TEMPERATURE;
  };
};

#endif
