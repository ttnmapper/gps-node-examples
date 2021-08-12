/*
  Lora Ping
  This sketch demonstrates how to constantly send data with the MKR WAN 1300/1310 LoRa module.
  It is useful only for LoRa range testing, as the power usage is too high for normal use.
  This example code is in the public domain.
*/

#include <MKRWAN.h>

LoRaModem modem;

// Uncomment if using the Murata chip as a module
// LoRaModem modem(Serial1);

#include "arduino_secrets.h"
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
String appEui = SECRET_APP_EUI;
String appKey = SECRET_APP_KEY;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  // change this to your regional band (eg. US915, AS923, ...)
  if (!modem.begin(EU868)) {
    Serial.println("Failed to start module");
    while (1) {}
  };
  Serial.print("Your module version is: ");
  Serial.println(modem.version());
  Serial.print("Your device EUI is: ");
  Serial.println(modem.deviceEUI());

  int connected = modem.joinOTAA(appEui, appKey);
  if (!connected) {
    Serial.println("Something went wrong. Do you have coverage of the network?");
    while (1) {}
  }

  modem.minPollInterval(60);
  modem.setADR(true);
}

void loop() {
  modem.dataRate(5);
  int err;
  modem.beginPacket();
  err = modem.endPacket(true);
  Serial.print("TX result=");
  Serial.println(err);
  delay(10000);
}
