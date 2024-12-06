#include <Arduino.h>
#include <sstream>
#include <AutoOTA.h>

#include "defines.h"
#include "mqtt.h"
#include "ota.h"
#include "wifi_connector.h"

AutoOTA otaLocal(CURRENT_VERSION, OTA_LOCAL_CONFIG_PATH);
uint32_t ota_tmr = 0;

void processOta() {
  if (isWifiConnected() && (!ota_tmr || millis() - ota_tmr > OTA_INTERVAL)) {
    ota_tmr = millis();

    Serial.print("current version (local OTA): ");
    Serial.println(otaLocal.version());
    Serial.println("checking updates");
    String ver, notes;
    if (otaLocal.checkUpdate(&ver, &notes)) {
      Serial.print("Newer version local: ");
      Serial.println(otaLocal.version());
      mqttPrintf(MQTT_LOG_TOPIC, "%s", "Updating from local server");
      otaLocal.updateNow();
    }
  }
}
