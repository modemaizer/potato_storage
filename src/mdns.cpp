#include <Arduino.h>
#include <ESP8266mDNS.h>

#include "defines.h"
#include "display.h"

void setupMdns() {
  Serial.println("setup MDNS");
  if (MDNS.begin(MDNS_NAME)) {
    MDNS.addService("http", "tcp", 80);
  }
}

void processMdns() {
  MDNS.update();
}