#define INIT_ADDR 0
#define INIT_KEY 50
#define SETTINGS_ADDR 1

#include <AM2320.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <EspHtmlTemplateProcessor.h>
#include <microDS18B20.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266mDNS.h>
#include <EncButton.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

struct Settings {
  float airTemperature = 4;
  float airTemperatureDelta = 0.1;
  float floorTemperatureLow = 10;
  float floorTemperatureHigh = 13;
};

const char *mdnsName = "potato";
const char *ssid = "";
const char *password = "";

IPAddress local_IP(192, 168, 1, 220);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);

const long interval = 5000;  // interval between sensors checks (milliseconds)
unsigned long previousMillis = 0;  // last time sensors were checked

float dsTemp = 0;
float amTemp = 0;
float humidity = 0;
bool heater = false;
bool amError = false;
bool dsError = false;
String amErrorText = "";
bool needHeat = false;

Settings settings;

WiFiClient client;
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdateServer;
EspHtmlTemplateProcessor templateProcessor(&httpServer);

AM2320 am;
MicroDS18B20<D4> ds;
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

Button btn(D7);

void getData() {
  DynamicJsonDocument doc(512);
  doc["floorTemperature"] = dsTemp;
  doc["airTemperature"] = amTemp;
  doc["humidity"] = humidity;
  doc["heater"] = heater;
  doc["floorSensorOk"] = !dsError;
  doc["airSensorOk"] = !amError;
  String buf;
  serializeJson(doc, buf);
  httpServer.send(200, "application/json", buf);
}

String indexKeyProcessor(const String& key)
{
  if (key == "AIR_TEMPERATURE") return amError ? amErrorText : String(amTemp);
  else if (key == "HUMIDITY") return amError ? amErrorText : String(humidity);
  else if (key == "FLOOR_TEMPERATURE") return dsError ? "Sensor error" : String(dsTemp);
  else if (key == "HEATER_COLOR") return heater ? "red" : "blue";

  return "Key not found";
}

void getMainPage() {
  templateProcessor.processAndSend("/index.html", indexKeyProcessor);
}

void saveSettingsToEeprom(bool begin) {
  if(begin) {
    EEPROM.begin(64);
  }
  EEPROM.put(SETTINGS_ADDR, settings);
  EEPROM.commit();
}

void updateSettings() {
  bool changed = false;
  if (httpServer.hasArg("air")){
    settings.airTemperature = httpServer.arg("air").toFloat();
    changed = true;
  }
  if (httpServer.hasArg("dAir")){
    settings.airTemperatureDelta = httpServer.arg("dAir").toFloat();
    changed = true;
  }
  if (httpServer.hasArg("fLo")){
    settings.floorTemperatureLow = httpServer.arg("fLo").toFloat();
    changed = true;
  }
  if (httpServer.hasArg("fHi")){
    settings.floorTemperatureHigh = httpServer.arg("fHi").toFloat();
    changed = true;
  }
  if(changed) {
    saveSettingsToEeprom(true);
  }
}

void getSettings() {
  DynamicJsonDocument doc(512);
  doc["airTemperature"] = settings.airTemperature;
  doc["airTemperatureDelta"] = settings.airTemperatureDelta;
  doc["floorTemperatureLow"] = settings.floorTemperatureLow;
  doc["floorTemperatureHigh"] = settings.floorTemperatureHigh;
  String buf;
  serializeJson(doc, buf);
  httpServer.send(200, "application/json", buf);
}
 
void restServerRouting() {
  httpServer.on(F("/"), HTTP_GET, getMainPage);
  httpServer.on(F("/data"), HTTP_GET, getData);
  httpServer.on(F("/settings"), HTTP_GET, getSettings);
  httpServer.on(F("/settings/update"), HTTP_GET, updateSettings);
}

void handleNotFound() {
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

void showText(String text, uint8_t fontSize, unsigned long delayTime) {
  display.clearDisplay();
  display.setTextSize(fontSize);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print(text);
  display.setCursor(0,0);
  display.display();
  if(delayTime > 0) {
    delay(delayTime);
  }
}

void showTemperatures(float dsTemp, float amTemp, float humidity, bool heating) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print("Floor:    ");
  display.println(dsTemp);  
  display.print("Air:      ");
  display.println(amTemp);  
  display.print("Humidity: ");
  display.println(humidity);
  display.print("Heating:  ");
  display.print(heating ? "ON" : "OFF");
  display.setCursor(0,0);
  display.display();
}

void showIp() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print(WiFi.localIP());
  display.setCursor(0,0);
  display.display();
  delay(3000);
}

void setupWiFi() {
  WiFi.mode(WIFI_STA);
  String wifiText = "";
  if (!WiFi.config(local_IP, gateway, subnet)) {
    wifiText = "IP config failed!";
  }
  WiFi.begin(ssid, password);
  int retries = 0;
  while ((WiFi.status() != WL_CONNECTED) && (retries < 15)) {
    retries++;
    delay(500);
  }
  wifiText += "WiFi connected! IP: ";
  if (retries > 14) {
    wifiText = "WiFi connection FAILED";
  }
  if (WiFi.status() == WL_CONNECTED) {
    wifiText += WiFi.localIP().toString();
  }
  showText(wifiText, 1, 1000);
}

void setupMdns() {
  if (MDNS.begin(mdnsName)) {
    char buf[30];
    strcpy(buf, "MDNS name: ");
    strcat(buf, mdnsName);
    showText(buf, 2, 2000);
  }
}

void setupHttp() {
  restServerRouting();
  httpServer.onNotFound(handleNotFound);
  httpUpdateServer.setup(&httpServer, "potato", "potato");
  httpServer.begin();
  showText("HTTP server started", 1, 1000);
}

void setupNetwork() {
  setupWiFi();
  setupMdns();
  setupHttp();
}

void readFloor() {
  if (ds.readTemp()) {
    dsTemp = ds.getTemp();
    dsError = false;
  } 
  else {
    dsTemp = 0;
    dsError = true;
  }
}

void readAir() {
  float tempSum = 0;
  float humSum = 0;
  byte count = 0;
  int errorCode = 0;
  for(int i=0; i<5; i++) {
    if (am.measure()) {
      tempSum += am.getTemperature();
      humSum += am.getHumidity();
      count ++;
      delay(20);
    }
    else {  // error has occured
      errorCode = am.getErrorCode();
      delay(20);
    }
    if (count == 0) {
      amTemp = 0;
      humidity = 0;
      amError = true;
      switch (errorCode) {
        case 1: amErrorText = "Sensor offline"; break;
        case 2: amErrorText = "CRC validation failed"; break;
      }
    }
    else {
      amTemp = tempSum / count;
      humidity = humSum / count;
      amError = false;
      amErrorText = "";
    }
  }
}

float getAirTemperatureHighBound() {
  return settings.airTemperature + settings.airTemperatureDelta;
}

float getAirTemperatureLowBound() {
  return settings.airTemperature - settings.airTemperatureDelta;
}

void switchHeater(bool enable) {
  digitalWrite(D6, enable ? HIGH : LOW);
  heater = enable;
}

void checkHeatNeed() {
  if(dsError || amError || (amTemp >= getAirTemperatureHighBound() && dsTemp > settings.airTemperature)) {
    needHeat = false;
    return;
  }
  if(amTemp <= getAirTemperatureLowBound() || dsTemp < settings.airTemperature - 2 || dsTemp <= 0) {
    needHeat = true;
  }
}

void processHeater() {
  if(needHeat) {
    if (dsTemp < settings.floorTemperatureLow) {
      switchHeater(true);
    }
    else if(dsTemp >= settings.floorTemperatureHigh) {
      switchHeater(false);
    }
    return;
  }

  switchHeater(false);  
}

void processSensors() {
  ds.requestTemp(); // to get actual data we give some time for floor sensor
  readAir();        // while air sensor is processing
  readFloor();

  if(dsError) {
    switchHeater(false);
    return;
  }
  
  checkHeatNeed();

  processHeater();

  showTemperatures(dsTemp, amTemp, humidity, heater);
}

void initSettings() {
  EEPROM.begin(64);
  if (EEPROM.read(INIT_ADDR) != INIT_KEY) {
    EEPROM.write(INIT_ADDR, INIT_KEY);
    saveSettingsToEeprom(false);
  }
  else {
    EEPROM.get(SETTINGS_ADDR, settings);
  }
}

void resetSettings() {
  settings.airTemperature = 4;
  settings.floorTemperatureLow = 10;
  settings.floorTemperatureHigh = 13;
  saveSettingsToEeprom(true);
  showText("Reset settings", 2, 1000);
}

void setup() {
  initSettings();
  SPIFFS.begin();
  ds.setResolution(10);
  am.begin(D2, D1);

  pinMode(D5, OUTPUT);
  digitalWrite(D5, LOW);
  pinMode(D6, OUTPUT);
  digitalWrite(D6, LOW);
  pinMode(D7, INPUT_PULLUP);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  display.display();

  setupNetwork();
}

void loop() {
  btn.tick();
  if(btn.click()) {
    showIp();
  }
  if(btn.holdFor(2000)) {
    resetSettings();
  }

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    processSensors();
  }

  httpServer.handleClient();
}

