#include <Arduino.h>
#include <AM2320.h>
#include <microDS18B20.h>
#include <EncButton.h>

#include "defines.h"
#include "settings.h"
#include "heater.h"
#include "display.h"
#include "sensors.h"

static float dsTemp = 0;
static float amTemp = 0;
static float humidity = 0;
static bool amError = false;
static bool dsError = false;
static String amErrorText = "";
uint32_t sensorsTimer = 0;

AM2320 am;
MicroDS18B20<FLOOR_SENSOR_PIN> ds;
Button btn(BUTTON_PIN);

void setupSensors() {
  Serial.println("init sensors");
  am.begin(AIR_SENSOR_DATA_PIN, AIR_SENSOR_CLOCK_PIN);
  btn.init(BUTTON_PIN, INPUT_PULLUP);
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
  ds.setResolution(10);
  ds.requestTemp();
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

void processSensors() {
  if (!sensorsTimer || millis() - sensorsTimer > getSensorsCheckInterval()) {
    sensorsTimer = millis();
    
    readAir();        // while air sensor is processing
    readFloor();

    if(!isFloorSensorOk()) {
      switchHeater(false);
      return;
    }

    processHeater();
  }
  
  btn.tick();
  if(btn.holdFor(2000)) {
    resetSettings();
  }
}

float getFloorTemperature() {
  return dsTemp;
}

float getAirTemperature() {
  return amTemp;
}

float getHumidity(){
  return humidity;
}

bool isFloorSensorOk() {
  return !dsError;
}

bool isAirSensorOk() {
  return !amError;
}

String getAirSensorError() {
  return amErrorText;
}
