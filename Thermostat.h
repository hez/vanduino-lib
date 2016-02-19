#ifndef thermostat_h
#define thermostat_h
#include <Time.h>
#include <TimeLib.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <ExtendedDHT.h>

#include <PushButton.h>
#include <Relay.h>
#include <ThermostatProgram.h>

#define DEBUG

// time between screen refreshes in seconds
#define TIME_BETWEEN_TEMP_DISPLAY 3
#define TIME_FOR_BUTTON_PRESSES 5
//#define ALWAYS_FETCH_TEMPERATURES    // Enable if you want don't want to skip temp fetches to make button presses more responsive
#define DEG_CHAR (char)223

#define FURNACE_OFF 0
#define MINUMUM_FURNACE_RUN 60//*30     // minumum run for furnance is 30min
#define NO_TARGET_TEMPERATURE -1
#define NO_SELECTED_PROGRAM -1
#define START_TARGET_TEMPERATURE 16   // start temperature for setting target is 16deg
#define DISPLAY_TEMPERATURE 1
#define DISPLAY_TARGET_TEMPERATURE 2
#define DISPLAY_CURRENT_PROGRAM 3

class Thermostat {
public:
  ExtendedDHT sensor;
  Relay furnace_relay;

  ThermostatProgram *programs[5] = {NULL, NULL, NULL, NULL, NULL};
  ThermostatProgram *current_running = NULL;
  short selected_program = NO_SELECTED_PROGRAM;

  time_t last_temp_display = 0;
  time_t furnace_on_at = FURNACE_OFF;
  time_t last_button_press = 0;
  short target_temperature = NO_TARGET_TEMPERATURE;
  bool fan_on = false;
  unsigned short current_screen = 0;

  Thermostat(const int sensor_pin, const int sensor_type);
  void loop();
  const bool addProgram(ThermostatProgram*);
  ThermostatProgram* getProgram(const unsigned int pos) {
    return this->programs[pos];
  }
  const unsigned short programCount() {
    for(int i = 0; i < sizeof(this->programs); i++) {
      if(this->programs[i] == NULL)
        return i;
    }
  }

private:
  void initSensor();
  void initRelays();

  void manageFan();

  void ensureFurnaceStarted(const time_t current_time);
  void furnaceShutdown(const time_t current_time);

  void registerButtonPress(const time_t current_time) {
    this->last_button_press = current_time;
  };
};

#endif
