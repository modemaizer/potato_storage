#include <Arduino.h>
#include <PubSubClient.h>

#include "wifi_connector.h"
#include "settings.h"
#include "sensors.h"
#include "mqtt.h"
#include "defines.h"
#include "heater.h"

static WiFiClient wifiClient;
static PubSubClient mqttClient(wifiClient);
static const char* mqttServer = MQTT_SERVER;
static const int mqttPort = MQTT_PORT;

uint32_t mqttConnectTimer = 0;
uint32_t mqttSendTimer = 0;

boolean reconnect() {
  if (mqttClient.connect(MQTT_ID, MQTT_USER, MQTT_PASS)) {
    mqttClient.subscribe(MQTT_COMMAND_TOPIC);
    mqttSendDeviceState();
  }
  return mqttClient.connected();
}

void processMqtt()
{
  if (!mqttClient.connected() && (!mqttConnectTimer || millis() - mqttConnectTimer > MQTT_RECONNECT_TIMEOUT)) {
    mqttConnectTimer = millis();
    reconnect();
  }
  if (mqttClient.connected() && (!mqttSendTimer || millis() - mqttSendTimer > getMqttInterval())) {
    mqttSendTimer = millis();
    mqttSendDeviceState();
  }
  mqttClient.loop();
}

static void parseIncomingCommand(char *topic, byte *payload, unsigned int length)
{
  char command[length + 1];
  for (uint32_t i = 0; i < length; i++)
    command[i] = (char)payload[i];

  command[length] = '\0';

  if (strcmp(command, "on") == 0)
    toggleDeviceState(true);
  else if (strcmp(command, "off") == 0)
    toggleDeviceState(false);
  else if (strstr(command, "temp ") != NULL) {
    setSettingsAirTemperature(atof(&command[5]));
  }
  else if (strstr(command, "delta ") != NULL) {
    setSettingsAirTemperatureDelta(atof(&command[6]));
  }
  
  mqttPrintf(MQTT_LOG_TOPIC, "%s", command);
}

void setupMqtt()
{
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback([](char *topic, byte *payload, unsigned int length)
             { parseIncomingCommand(topic, payload, length); });
}

// example: mqttPrintf("topic", "Hello, %s! The answer is %d", "World", 42);
void mqttPrintf(const char *topic, const char *format, ...)
{
  uint8_t bufferSize = 250;
  char* buffer = new char[bufferSize];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, bufferSize, format, args);
  va_end(args);
  mqttClient.publish(topic, buffer);
  delete [] buffer;
}

void mqttSendDeviceState() {
  mqttPrintf(MQTT_STATE_TOPIC, "%s", getDeviceState() ? MQTT_STATE_ON : MQTT_STATE_OFF);
  mqttPrintf(MQTT_HEATER_STATE_TOPIC, "%d", isHeaterEnabled() ? MQTT_STATE_ON : MQTT_STATE_OFF);
  mqttPrintf(MQTT_FLOOR_TEMPERATURE_TOPIC, "%.2f", getFloorTemperature());
  mqttPrintf(MQTT_AIR_TEMPERATURE_TOPIC, "%.2f", getAirTemperature());
  mqttPrintf(MQTT_HUMIDITY_TOPIC, "%.2f", getHumidity());
}
