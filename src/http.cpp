#include <ArduinoJson.h>

#include <ESP8266WebServer.h>
#include <LittleFS.h>

#include "wifi_connector.h"
#include "settings.h"
#include "sensors.h"
#include "heater.h"

ESP8266WebServer httpServer(80);

static void getDataEndpoint() {
  DynamicJsonDocument doc(512);
  doc["floorTemperature"] = getFloorTemperature();
  doc["airTemperature"] = getAirTemperature();
  doc["humidity"] = getHumidity();
  doc["heaterEnabled"] = isHeaterEnabled();
  doc["floorSensorOk"] = isFloorSensorOk();
  doc["airSensorOk"] = isAirSensorOk();
  doc["airSensorError"] = getAirSensorError();
  doc["enabled"] = getDeviceState();
  String buf;
  serializeJson(doc, buf);
  httpServer.send(200, "application/json", buf);
}

static void getSettingsEndpoint() {
  DynamicJsonDocument doc(512);
  doc["airTemperature"] = getSettingsAirTemperature();
  doc["airTemperatureDelta"] = getSettingsAirTemperatureDelta();;
  doc["floorTemperatureLow"] = getFloorTemperatureLowBound();
  doc["floorTemperatureHigh"] = getFloorTemperatureHighBound();
  doc["sensorsInterval"] = getSensorsCheckInterval();
  doc["mqttInterval"] = getMqttInterval();
  doc["version"] = getVersion();
  String buf;
  serializeJson(doc, buf);
  httpServer.send(200, "application/json", buf);
}

static void updateSettingsEndpoint() {
  if (httpServer.method() != HTTP_POST) {
    httpServer.send(405, "text/plain", "Method Not Allowed");
    return;
  } 
  if (httpServer.hasArg("air")){
    setSettingsAirTemperature(httpServer.arg("air").toFloat());
  }
  if (httpServer.hasArg("dAir")){
    setSettingsAirTemperatureDelta(httpServer.arg("dAir").toFloat());
  }
  if (httpServer.hasArg("fMin")){
    setFloorTemperatureLowBound(httpServer.arg("fMin").toFloat());
  }
  if (httpServer.hasArg("fMax")){
    setFloorTemperatureHighBound(httpServer.arg("fMax").toFloat());
  }
  if(httpServer.hasArg("i")) {
    setSensorsCheckInterval(httpServer.arg("i").toInt());
  }
  if(httpServer.hasArg("m")) {
    setMqttInterval(httpServer.arg("m").toInt());
  }
  getSettingsEndpoint();
}

static void switchDeviceStateEndpoint() {
  toggleDeviceState();
  httpServer.send(200, "text/plain", String(getDeviceState()));
}
 
static void restServerRouting() {
  httpServer.serveStatic("/", LittleFS, "/index.html");
  httpServer.serveStatic("/settings", LittleFS, "/settings.html");
  httpServer.serveStatic("/settings.css", LittleFS, "/settings.css");
  httpServer.serveStatic("/settings.js", LittleFS, "/settings.js");
  httpServer.on(F("/data"), HTTP_GET, getDataEndpoint);
  httpServer.on(F("/settings/data"), HTTP_GET, getSettingsEndpoint);
  httpServer.on(F("/settings/update"), HTTP_POST, updateSettingsEndpoint);
  httpServer.on(F("/switch"), HTTP_GET, switchDeviceStateEndpoint);
}

static void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += httpServer.uri();
  message += "\nMethod: ";
  message += (httpServer.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += httpServer.args();
  message += "\n";
  for (uint8_t i = 0; i < httpServer.args(); i++) {
    message += " " + httpServer.argName(i) + ": " + httpServer.arg(i) + "\n";
  }
  httpServer.send(404, "text/plain", message);
}

void setupHttp() {
  Serial.println("setup HTTP");
  restServerRouting();
  httpServer.onNotFound(handleNotFound);
  // httpUpdateServer.setup(&httpServer, "potato", "potato");
  httpServer.begin();
}

void processHttp() {
  httpServer.handleClient();
}
