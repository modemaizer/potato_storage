#include "defines.h"
#include "wifi_connector.h"
#include "sensors.h"
#include "http.h"
#include "heater.h"
#include "settings.h"
#include "mdns.h"
#include "mqtt.h"

void onWifiConnected() {
  setupMdns();
  setupHttp();
}

void setupNetwork() {
  Serial.println("setup network");
  wifiInit();
  setupMqtt();
}

void processNetwork() {
  processMdns();
  processHttp();
  processMqtt();
}

void setup() {
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println("Start");

  setupSettings();
  setupSensors();
  setupHeater();
  setupNetwork();
}

void loop() {
  processSensors();
  processNetwork();
}
