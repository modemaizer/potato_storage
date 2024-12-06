#include <Arduino.h>

#include "defines.h"
#include "sensors.h"
#include "settings.h"
#include "mqtt.h"
#include "heater.h"

void setupHeater() {
  pinMode(HEATER_PIN, OUTPUT);
  digitalWrite(HEATER_PIN, LOW);
}

void switchHeater(bool enable) {
  bool currentState = isHeaterEnabled();
  digitalWrite(HEATER_PIN, enable ? HIGH : LOW);
  if(currentState != isHeaterEnabled()) {
    mqttSendDeviceState();
  }
}

static boolean needHeat() {
  if(!getDeviceState() || !isFloorSensorOk() || !isAirSensorOk() || (getAirTemperature() >= getAirTemperatureHighBound() && getFloorTemperature() > getSettingsAirTemperature())) {
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
