#include <ThermostatHardware.h>

void ThermostatHardware::loop() {
  buttonHandler();
  thermostat.loop();

  if(millis() > last_button_press + 8 * 1000 && millis() > last_screen_saver_display + 1 * 1000) {
    Serial.print("last press: "); Serial.println(last_button_press);
    Serial.print(millis());
    last_screen_saver_display = millis();
    displayTemperature(now());
  }
}

void ThermostatHardware::screenSetup(const unsigned short num_columns, const unsigned short num_rows) {
  lcd.begin(num_columns, num_rows);
  // ------- Quick 3 blinks of backlight  -------------
  for(int i = 0; i< 3; i++)
  {
    lcd.backlight();
    delay(150);
    lcd.noBacklight();
    delay(150);
  }
  lcd.backlight();
}

// Menu setup
void ThermostatHardware::menuSetup() {
//  main_menu.add_item(&mm_mi1, &on_item1_selected);
//  main_menu.add_menu(&mu1);
  //mu1.add_item(&mu1_mi1, &on_item3_selected);
  menu_system.set_root_menu(&main_menu);
}

void ThermostatHardware::buttonHandler() {
  if(up_button.pressed()) {
    logButtonPressed();
    menu_system.prev();
    displayMenu();
  }
  if(down_button.pressed()) {
    logButtonPressed();
    menu_system.next();
    displayMenu();
  }
  if(right_button.pressed()) {
    logButtonPressed();
    menu_system.select();
    displayMenu();
  }
  if(left_button.pressed()) {
    logButtonPressed();
    menu_system.back();
    displayMenu();
  }
}

void ThermostatHardware::logButtonPressed() {
  last_button_press = millis();
}

void ThermostatHardware::displayTemperature(const time_t current_time) {
  // Do we require a full screen refresh?
  //    Don't do a full screen refresh unless we have to, doing so each time
  //    causes noticable flicker as the lcd doesn't redraw that quickly.
  const bool full_display = true;
#ifdef ALWAYS_FETCH_TEMPERATURES
  const bool allow_temp_fetch = true;
#else
  const bool allow_temp_fetch = current_time - thermostat.last_button_press >= TIME_FOR_BUTTON_PRESSES;
#endif

  if(full_display)
    lcd.clear();

  // 1st line: date/time
  lcd.setCursor(0, 0);
  // thermostat.displayDateTime();

  // 2nd line
  if(full_display) {
    lcd.setCursor(0, 1);
    lcd.print(F("Temperature: "));
  } else {
    lcd.setCursor(13, 1);
  }
  lcd.print(round(thermostat.sensor.getTemperature(allow_temp_fetch)));
  lcd.print(DEG_CHAR);

  // 3rd line
  if(full_display) {
    lcd.setCursor(0, 2);
    lcd.print(F("Humidity: "));
  } else {
    lcd.setCursor(10, 2);
  }
  lcd.print(round(thermostat.sensor.getHumidity(allow_temp_fetch)));
  lcd.print(F("%  "));

  // 4th line
  if(full_display) {
    lcd.setCursor(0, 3);
    lcd.print(F("Heater: "));
  } else {
    lcd.setCursor(8, 3);
  }
  // Are we running a program?
  if(thermostat.current_running != NULL) {
    lcd.print(thermostat.current_running->getName());
  } else if(thermostat.furnace_on_at == FURNACE_OFF) {
    lcd.print(F("off        "));
  } else {
    if(thermostat.target_temperature == NO_TARGET_TEMPERATURE) {
      lcd.print(F("turning off"));
    } else {
      lcd.print(F("on "));
      lcd.print(thermostat.target_temperature);
      lcd.print(DEG_CHAR);
      if(thermostat.fan_on)
        lcd.print((char)243);
      lcd.print(F("   "));
    }
  }
}

void ThermostatHardware::displayMenu() {
  lcd.clear();
  lcd.setCursor(0,0);
  // Display the menu
  Menu const* cp_menu = menu_system.get_current_menu();

  //lcd.print("Current menu name: ");
  lcd.print(cp_menu->get_name());
  lcd.setCursor(0,1);
  lcd.print(cp_menu->get_selected()->get_name());
}
