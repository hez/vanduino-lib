#ifndef thermostat_program_h
#define thermostat_program_h

class ThermostatProgram {
  const char* name;
  const unsigned int target;

public:
  ThermostatProgram(const char* name, const unsigned int target) :
    name(name), target(target)
    {};

  const char* getName() {
    return this->name;
  };
  const unsigned int getTarget() {
    return this->target;
  };
};

#endif
