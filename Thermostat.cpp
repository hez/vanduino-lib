#include <Thermostat.h>

Thermostat::Thermostat(const int sensor_pin, const int sensor_type) :
    sensor(sensor_pin, sensor_type),
    furnace_relay(8)
{
  initSensor();
  initRelays();
}

void Thermostat::loop() {
  const time_t cur_timestamp = now();

  // Button presses
  /*
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
    registerButtonPress(cur_timestamp);
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
    registerButtonPress(cur_timestamp);
    ensureFurnaceStarted(cur_timestamp);
    displayTargetTemperature();
  }

  if(this->left_button.pressed()) {
#ifdef DEBUG
    Serial.println("left button pressed");
#endif
    this->target_temperature = NO_TARGET_TEMPERATURE;
    this->last_temp_display = cur_timestamp - TIME_BETWEEN_TEMP_DISPLAY;
    registerButtonPress(cur_timestamp);
  }

  if(this->right_button.pressed()) {
    this->selected_program += 1;
    if(this->selected_program >= this->programCount())
      this->selected_program = 0;
#ifdef DEBUG
    Serial.println("right button pressed");
    Serial.print("Current program: ");
    Serial.println(this->selected_program);
#endif
    this->last_temp_display = cur_timestamp; // reset temp display
    registerButtonPress(cur_timestamp);
    displayProgram();
  }

  // Main screen display
  if(cur_timestamp - this->last_temp_display >= TIME_BETWEEN_TEMP_DISPLAY) {
    this->last_temp_display = cur_timestamp;
    displayTemperature(cur_timestamp);
  }
  */

  // Manage the fan
  manageFan();

  // Check if we should shut down the furnace
  furnaceShutdown(cur_timestamp);
}

const bool Thermostat::addProgram(ThermostatProgram* program) {
  for(int i = 0; i < sizeof(this->programs); i++) {
    if(this->programs[i] == NULL) {
      this->programs[i] = program;
      return true;
    }
  }
  return false;
}

void Thermostat::initSensor() {
  this->sensor.begin();
}

void Thermostat::initRelays() {
  this->furnace_relay.init();
  this->furnace_relay.turn_off();
}

void Thermostat::manageFan() {
  if(this->furnace_on_at == FURNACE_OFF || this->target_temperature == NO_TARGET_TEMPERATURE) {
    this->fan_on = false;
    return;
  }

  if(this->target_temperature + 3 >= round(this->sensor.getTemperature())) {
    this->fan_on = true;
  }

  if(this->target_temperature < round(this->sensor.getTemperature())) {
    this->fan_on = false;
  }
}

void Thermostat::ensureFurnaceStarted(const time_t current_time) {
  if(this->furnace_on_at == FURNACE_OFF) {
#ifdef DEBUG
    Serial.print("furnace turned on at: ");
    Serial.print(current_time);
    Serial.println();
#endif
    this->furnace_on_at = current_time;
    this->furnace_relay.turn_on();
  }
}

void Thermostat::furnaceShutdown(const time_t current_time) {
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
}

/*
void Thermostat::displayTemperature(const time_t current_time) {
  // Do we require a full screen refresh?
  //    Don't do a full screen refresh unless we have to, doing so each time
  //    causes noticable flicker as the lcd doesn't redraw that quickly.
  const bool full_display = this->current_screen != DISPLAY_TEMPERATURE;
#ifdef ALWAYS_FETCH_TEMPERATURES
  const bool allow_temp_fetch = true;
#else
  const bool allow_temp_fetch = current_time - this->last_button_press >= TIME_FOR_BUTTON_PRESSES;
#endif

  if(full_display)
    this->display.clear();

  // 1st line: date/time
  this->display.setCursor(0, 0);
  this->displayDateTime();

  // 2nd line
  if(full_display) {
    this->display.setCursor(0, 1);
    this->display.print(F("Temperature: "));
  } else {
    this->display.setCursor(13, 1);
  }
  this->display.print(round(this->sensor.getTemperature(allow_temp_fetch)));
  this->display.print(DEG_CHAR);

  // 3rd line
  if(full_display) {
    this->display.setCursor(0, 2);
    this->display.print(F("Humidity: "));
  } else {
    this->display.setCursor(10, 2);
  }
  this->display.print(round(this->sensor.getHumidity(allow_temp_fetch)));
  this->display.print(F("%  "));

  // 4th line
  if(full_display) {
    this->display.setCursor(0, 3);
    this->display.print(F("Heater: "));
  } else {
    this->display.setCursor(8, 3);
  }
  // Are we running a program?
  if(this->current_running != NULL) {
    this->display.print(this->current_running->getName());
  } else if(this->furnace_on_at == FURNACE_OFF) {
    this->display.print(F("off        "));
  } else {
    if(this->target_temperature == NO_TARGET_TEMPERATURE) {
      this->display.print(F("turning off"));
    } else {
      this->display.print(F("on "));
      this->display.print(this->target_temperature);
      this->display.print(DEG_CHAR);
      if(this->fan_on)
        this->display.print((char)243);
      this->display.print(F("   "));
    }
  }

  // Set last displayed for next go around
  this->current_screen = DISPLAY_TEMPERATURE;
}

void Thermostat::displayDateTime() {
  const int cur_min = minute();

  this->display.print(year());
  this->display.print(F("-"));
  this->display.print(month());
  this->display.print(F("-"));
  this->display.print(day());
  this->display.print(F(" "));
  this->display.print(hour());
  this->display.print(F(":"));

  if(cur_min < 10)
    this->display.print(F("0"));

  this->display.print(cur_min);
  this->display.print(F("  "));
}

void Thermostat::displayTargetTemperature() {
  this->display.clear();
  this->display.setCursor(0, 1);
  this->display.print(F("Target: "));
  this->display.print(this->target_temperature);
  this->current_screen = DISPLAY_TARGET_TEMPERATURE;
}

void Thermostat::displayProgram() {
  ThermostatProgram* program = this->programs[this->selected_program];
  this->display.clear();
  this->display.setCursor(0, 0);
  this->display.print(F("Program: "));
  this->display.print(this->selected_program + 1);
  this->display.print(F("/"));
  this->display.print(this->programCount());
  this->display.setCursor(0, 1);
  this->display.print(program->getName());
  this->display.setCursor(0, 2);
  this->display.print(program->getTarget());
  this->display.print(DEG_CHAR);
  this->current_screen = DISPLAY_CURRENT_PROGRAM;
}
*/
