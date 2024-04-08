#include <Arduino.h>
#include <SpaIot.h>
#include <WiFi.h>
#include "Mqttclient.h"

mqttSettings settings("jdeedom.btssn.lan", "jeedom", "W+pL(69g8u8$hY", "romain/spa", "client1");

String wifiSSID = "IoT";
String wifipassword = "BtsSnForEver2022";

const unsigned long BaudRate = 115200;

WiFiClient WifiClient;

void setup() {
  Serial.begin(BaudRate);
  WiFi.begin(wifiSSID.c_str(), wifipassword.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to the WiFi network");

}

void loop() {
  // put your main code here, to run repeatedly:
}