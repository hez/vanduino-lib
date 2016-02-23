#ifndef thermostat_hardware_h
#define thermostat_hardware_h
#include <LiquidCrystal_I2C.h>
#include <MenuSystem.h>

#include <Thermostat.h>

#define DEBUG

#define DHTPIN 2
#define DHTTYPE DHT22

#define DISPLAY_NUMBER_OF_COLUMNS 20
#define DISPLAY_NUMBER_OF_ROWS 4

#define DISPLAYED_TEMPERATURE 1
#define DISPLAYED_TARGET_TEMPERATURE 2
#define DISPLAYED_CURRENT_PROGRAM 3
#define DISPLAYED_MENU 4
#define DISPLAYED_SHUTDOWN 4

class ThermostatHardware {
public:
  unsigned short current_screen = 0;

  Thermostat thermostat;

  // Buttons
  PushButton up_button;
  PushButton down_button;
  PushButton left_button;
  PushButton right_button;

  unsigned long last_button_press = 0;
  unsigned long last_screen_saver_display = 0;

  MenuSystem *menu_system;

  // LCD
  LiquidCrystal_I2C lcd;

  ThermostatHardware(int lcd_address, const int up_button_pin, const int down_button_pin, const int left_button_pin, const int right_button_pin, MenuSystem *ms) :
    thermostat(DHTPIN, DHTTYPE),
    lcd(lcd_address, 2, 3, 0, 4, 5, 6, 7, 3, POSITIVE),
    up_button(up_button_pin), down_button(down_button_pin),
    left_button(left_button_pin), right_button(right_button_pin)
  {
    menu_system = ms;
    ThermostatProgram *p1 = new ThermostatProgram("Morning Mode", 22);
    thermostat.addProgram(p1);
    p1 = new ThermostatProgram("Evening Mode", 19);
    thermostat.addProgram(p1);

    screenSetup(DISPLAY_NUMBER_OF_COLUMNS, DISPLAY_NUMBER_OF_ROWS);
  };

  void loop();
  void runTemperatureSet();
  void runShutdown();
  void logButtonPressed();
  void startScreenSaver() {
    last_button_press = 0;
  };
  void displayTemperature(const time_t);
  void displayMenu();
  void displayTargetTemperature();
  void displayDateTime();
  void displaySplashScreen();

private:
  void screenSetup(const unsigned short, const unsigned short);
  void buttonHandler();
};

#endif
