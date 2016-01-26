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
  bool fan_on = false;
  unsigned int last_displayed = 0;

public:
  Thermostat(const int display_address, const int sensor_pin, const int sensor_type);
  void loop();

private:
  void initLcd();
  void initSensor();
  void initButtons();
  void initRelays();
  void manageFan();
  void ensureFurnaceStarted(const time_t current_time);
  void furnaceShutdown(const time_t current_time);
  void displayTemperature();
  void displayTargetTemperature();
};

#endif
