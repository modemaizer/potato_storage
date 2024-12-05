#pragma once

void setupSensors();

float getFloorTemperature();
float getAirTemperature();
float getHumidity();

bool isFloorSensorOk();
bool isAirSensorOk();
String getAirSensorError();

void readFloor();
void readAir();

void processSensors();
