#include <FileData.h>
#include <LittleFS.h>

#include "defines.h"
#include "settings.h"
#include "display.h"

static PotatoSettings settings;

FileData data(&LittleFS, "/settings.dat", 'B', &settings, sizeof(settings));

void setupSettings() {
    if(LittleFS.begin()) {
        Serial.println("LittleFS OK");
    }
    else {
        Serial.println("LittleFS ERR");
    }
    data.setTimeout(1000);
    data.read();
}

void settingsProcess() {
    data.tick();
}

float getSettingsAirTemperature() {
    return settings.airTemperature;
}
float getSettingsAirTemperatureDelta() {
    return settings.airTemperatureDelta;
}
bool getDeviceState() {
    return settings.on;
}
uint32_t getSensorsCheckInterval() {
    return settings.sensorsInterval;
}
uint32_t getMqttInterval() {
    return settings.mqttInterval;
}
float getAirTemperatureLowBound() {
    return settings.airTemperature - settings.airTemperatureDelta;
}
float getAirTemperatureHighBound() {
    return settings.airTemperature + settings.airTemperatureDelta;
}
float getFloorTemperatureLowBound() {
    return settings.floorTemperatureLow;
}
float getFloorTemperatureHighBound() {
    return settings.floorTemperatureHigh;
}
String getVersion() {
    return CURRENT_VERSION;
}

void setSettingsAirTemperature(float value) {
    settings.airTemperature = value;
    data.update();
}
void setSettingsAirTemperatureDelta(float value) {
    settings.airTemperatureDelta = value;
    data.update();
}
void setFloorTemperatureLowBound(float value) {
    settings.floorTemperatureLow = value;
    data.update();
}
void setFloorTemperatureHighBound(float value) {
    settings.floorTemperatureHigh = value;
    data.update();
}
void setSensorsCheckInterval(uint32_t value) {
    settings.sensorsInterval = value;
    data.update();
}
void setMqttInterval(uint32_t value) {
    settings.mqttInterval = value;
    data.update();
}

void toggleDeviceState() {
    settings.on = !settings.on;
    data.update();
    // mqttSendDeviceState();
}

void toggleDeviceState(bool state) {
  if(settings.on != state) {
    toggleDeviceState();
  }  
}

void resetSettings() {
  settings.airTemperature = SETTINGS_AIR_TEMPERATURE_DEFAULT;
  settings.airTemperatureDelta = SETTINGS_AIR_DELTA_DEFAULT;
  settings.floorTemperatureLow = SETTINGS_FLOOR_TEMPERATURE_MIN;
  settings.floorTemperatureHigh = SETTINGS_FLOOR_TEMPERATURE_MAX;
  settings.on = false;
  data.updateNow();
}
