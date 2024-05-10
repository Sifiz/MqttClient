#include <Arduino.h>
#include <SpaIot.h>
#include <WiFi.h>
#include "Mqttclient.h"

mqttSettings settings(/*server*/"jdeedom.btssn.lan",/*user*/ "jeedom",/*password*/ "W+pL(69g8u8$hY",/*topic*/ "romain/spa",/*client id*/ "client1");


String wifiSSID = "IoT";
String wifipassword = "BtsSnForEver2022";

using namespace SpaIot;
const unsigned long BaudRate = 115200;


WiFiClient WifiClient;
SpaServer spa;

void setup() {
  Serial.begin(BaudRate);
  WiFi.begin(wifiSSID.c_str(), wifipassword.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to the WiFi network");
  spa.addClient(MqttClient);
  MqttClient.begin(settings, WifiClient);

  if (spa.begin() {
    Serial.println("Server begin succeeded");
  } else {
    Serial.println("Server begin failed");
    exit(EXIT_FAILURE);
  }


}

void loop() {
  // put your main code here, to run repeatedly:
  spa.handle();
}