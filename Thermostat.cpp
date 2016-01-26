#include <Thermostat.h>

Thermostat::Thermostat(const int display_address, const int sensor_pin, const int sensor_type) :
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
}

void Thermostat::loop() {
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

void Thermostat::initLcd() {
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
}

void Thermostat::initSensor() {
  this->sensor.begin();
}

void Thermostat::initButtons() {
  this->up_button.init();
}

void Thermostat::initRelays() {
  this->furnace_relay.init();
  this->furnace_relay.turn_off();
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

void Thermostat::displayTemperature() {
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
}

void Thermostat::displayTargetTemperature() {
  this->display.clear();
  this->display.setCursor(0, 1);
  this->display.print(F("Target: "));
  this->display.print(this->target_temperature);
  this->last_displayed = DISPLAYED_TARGET_TEMPERATURE;
};
