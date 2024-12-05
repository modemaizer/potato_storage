#pragma once

#define CURRENT_VERSION         "0.5"
#define MDNS_NAME               "potato"
#define AIR_SENSOR_CLOCK_PIN    D1
#define AIR_SENSOR_DATA_PIN     D2
#define HEATER_PIN              D6
#define BUTTON_PIN              D7
#define FLOOR_SENSOR_PIN        D4
#define DISPLAY_ADDRESS         0x3C

#define MQTT_ID                         "mqtt_potato_storage"
#define MQTT_USER                       "potato"
#define MQTT_PASS                       "0112358"
#define MQTT_FLOOR_TEMPERATURE_TOPIC    "potato/floor_temp"
#define MQTT_AIR_TEMPERATURE_TOPIC      "potato/air_temp"
#define MQTT_HUMIDITY_TOPIC             "potato/humidity"
#define MQTT_HEATER_STATE_TOPIC         "potato/heater_state"
#define MQTT_COMMAND_TOPIC              "potato/"
#define MQTT_LOG_TOPIC                  "potato/log"
#define MQTT_STATE_TOPIC                "potato/state"
#define MQTT_STATE_ON                   "ON"
#define MQTT_STATE_OFF                  "OFF"