#include <Arduino.h>

#include "defines.h"
#include "sensors.h"
#include "settings.h"

void setupHeater() {
  pinMode(HEATER_PIN, OUTPUT);
  digitalWrite(HEATER_PIN, LOW);
}

void switchHeater(bool enable) {
  digitalWrite(HEATER_PIN, enable ? HIGH : LOW);
}

static boolean needHeat() {
  if(!isFloorSensorOk() || !isAirSensorOk() || (getAirTemperature() >= getAirTemperatureHighBound() && getFloorTemperature() > getSettingsAirTemperature())) {
    return false;
  }
  return (getAirTemperature() <= getAirTemperatureLowBound() || getFloorTemperature() < getSettingsAirTemperature() - 2 || getFloorTemperature() <= 0);
}

void processHeater() {
  if(needHeat()) {
    if (getFloorTemperature() < getFloorTemperatureLowBound()) {
      switchHeater(true);
    }
    else if(getFloorTemperature() >= getFloorTemperatureHighBound()) {
      switchHeater(false);
    }
    return;
  }

  switchHeater(false);  
}

bool isHeaterEnabled() {
  return digitalRead(HEATER_PIN);
}
