#include <Arduino.h>
#include <SpaIot.h>
#include <WiFi.h>
#include "Mqttclient.h"
#include <Ccaptive_portal.h>
mqttSettings settings(/*server*/"jdeedom.btssn.lan",/*user*/ "jeedom",/*password*/ "W+pL(69g8u8$hY",/*topic*/ "romain/spa",/*client id*/ "client1");

using namespace SpaIot;
const unsigned long BaudRate = 115200;

Portal portal;
WiFiClient WifiClient;
SpaServer spa;
// My bus configuration :
// SDATA  -> GPIO23
// SCLK   -> GPIO18
// nWR    -> GPIO19
//Pins configuration
/*
[INPUT]
 SDATA    -> GPIO23
 SCLK     -> GPIO18
 nWR      -> GPIO19
 PULSE    -> GPIO21
 LUMIERE  -> GPIO22
 LED      -> GPIO2
[OUTPUT]
 S4       -> GPIO33
 S3       -> GPIO17
 S2       -> GPIO16
 S1       -> GPI27
 [JTAG]
TMS      -> GPIO14
TCK      -> GPIO13
TDO      -> GPIO15
TDI      -> GPIO12
NC       -> NC
[UART]
TXD0     -> TX
RXD0     -> RX


*/
const BusSettings MyBus (23, 18, 19);
String wifiSSID = "IoT";
String wifipassword = "BtsSnForEver2022";

void setup() {
  portal.initPortal();  //Init the captive portal
  Serial.begin(BaudRate);
  WiFi.begin(portal.wifiSSID.c_str(), portal.wifiPassword.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to the WiFi network");
  WiFi.softAPdisconnect(true);
  spa.addClient(MqttClient);
  MqttClient.begin(settings, WifiClient);
  if (spa.begin(ServerSettings("SPAIOT32SSP")))
  {
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