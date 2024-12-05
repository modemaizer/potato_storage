#pragma once

#define SETTINGS_AIR_TEMPERATURE_DEFAULT    4
#define SETTINGS_AIR_DELTA_DEFAULT          0.1
#define SETTINGS_FLOOR_TEMPERATURE_MIN      10
#define SETTINGS_FLOOR_TEMPERATURE_MAX      13
#define SETTINGS_SENSORS_INTERVAL_DEFAULT   3000
#define SETTINGS_MQTT_INTERVAL_DEFAULT      10000

struct PotatoSettings {
  bool on = false;
  float airTemperature = SETTINGS_AIR_TEMPERATURE_DEFAULT;
  float airTemperatureDelta = SETTINGS_AIR_DELTA_DEFAULT;
  float floorTemperatureLow = SETTINGS_FLOOR_TEMPERATURE_MIN;
  float floorTemperatureHigh = SETTINGS_FLOOR_TEMPERATURE_MAX;
  uint32_t sensorsInterval = SETTINGS_SENSORS_INTERVAL_DEFAULT;
  uint32_t mqttInterval = SETTINGS_MQTT_INTERVAL_DEFAULT;
};

void setupSettings();
void settingsProcess();

float getSettingsAirTemperature();
float getSettingsAirTemperatureDelta();
bool getDeviceState();
uint32_t getSensorsCheckInterval();
uint32_t getMqttInterval();
float getAirTemperatureLowBound();
float getAirTemperatureHighBound();
float getFloorTemperatureLowBound();
float getFloorTemperatureHighBound();
String getVersion();

void setSettingsAirTemperature(float value);
void setSettingsAirTemperatureDelta(float value);
void setFloorTemperatureLowBound(float value);
void setFloorTemperatureHighBound(float value);
void setSensorsCheckInterval(uint32_t value);
void setMqttInterval(uint32_t value);

void toggleDeviceState();
void toggleDeviceState(bool state);

void resetSettings();