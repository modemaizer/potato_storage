#pragma once

void processMqtt();
void setupMqtt();
void mqttPrintf(const char* topic, const char* format, ...);
void mqttSendDeviceState();
