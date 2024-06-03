#include <Arduino.h>
#include <SpaIot.h>
#include <WiFi.h>
#include "Mqttclient.h"
#include <Ccaptive_portal.h>
#include <EEPROM.h>
mqttSettings settings(/*server*/"jeedom.btssn.lan",/*user*/ "jeedom",/*password*/ "t7n8ajpw3LEn",/*topic*/ "spa",/*client id*/ "client1");

using namespace SpaIot;
const unsigned long BaudRate = 115200;

Portal portal;
WiFiClient WifiClient;

/*
[INPUT]
 SDATA    -> GPIO23
 SCLK     -> GPIO18
 nWR      -> GPIO19
 PULSE    -> GPIO21
 LUMIERE  -> GPIO32
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
int sdata = 23;
int sclk = 18;
int nwr = 19;
int inh = 26;
int pulse = 21 ;
int led = 2;
// My button controllers
int S1 = 27;
int S2 = 16;
int S3 = 17;
int S4 = 33;
Multiplexer Mux ("U10", {S1, S2, S3, S4}, inh); // A->GPIO5, B->GPIO4, C->GPIO15, INH->GPIO16
const BusSettings MyBus (sdata, sclk, nwr);


//add my config to the SpaIot library

// My buttons configuration (SSP)
// MuxA and MuxB are defined in MyBoardSettings.h
const std::map<int, ButtonSettings> MyButtons = {
  { Filter,   ButtonSettings (Mux, 1) },  // Filter   -> A1
  { Bubble,   ButtonSettings (Mux, 3) },  // Bubble   -> A3
  { TempDown, ButtonSettings (Mux, 7) },  // TempDown -> A7

  { Power,    ButtonSettings (Mux, 10) },  // Power    -> B2
  { TempUp,   ButtonSettings (Mux, 12) },  // TempUp   -> B4
  { TempUnit, ButtonSettings (Mux, 13) },  // TempUnit -> B5
  { Heater,   ButtonSettings (Mux, 15) }   // Heater   -> B7
};

// My custom configuration
// MyBus is defined in MyBoardSettings.h
// SspLeds is a SpaIot object that manages the LEDs of the SSP
const HardwareSettings MyConfig (MyBus, SspLeds, MyButtons);
ServerSettings MySettings;
SpaServer spas;
int debug = 0 ;
String debug_wifi_ssid = "IoT";
String debug_wifi_password = "BtsSnForEver2022";


void writeCredentialsToEEPROM(const String& ssid, const String& password) {
    EEPROM.begin(512);
    EEPROM.writeString(0, ssid);
    EEPROM.writeString(ssid.length() + 1, password);
    EEPROM.commit();
    EEPROM.end();
  }
  
void clignoteraffichage() {
  digitalWrite(pulse, LOW);
  delay(100);
  digitalWrite(pulse, HIGH);
  delay(100);
  digitalWrite(pulse, LOW);
}


void setup() {
  HardwareSettings::addToRegister("MyConfig", MyConfig);
  MySettings.setSpaModel("MyConfig");
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
  pinMode(pulse, OUTPUT);
  //write ssid and pasword to the eeprom
  Serial.begin(BaudRate);
if (debug == 0) {
  portal.initPortal();  //Init the captive portal
  Serial.println("Portal so connect with portal wifi");
  WiFi.begin(portal.wifiSSID.c_str(), portal.wifiPassword.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    clignoteraffichage();
  }
  Serial.println("Connected to the WiFi network");
  //print IP and info
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  WiFi.softAPdisconnect(true);
  spas.addClient(MqttClient);
  MqttClient.begin(settings, WifiClient);
  if (spas.begin(ServerSettings("SPAIOT32SSP")))
  {
    Serial.println("Server begin succeeded");
  } else {
    Serial.println("Server begin failed");
    exit(EXIT_FAILURE);
  }}
  else {
    Serial.println(" DEBUG MODE connect with wifi static password");
    WiFi.begin(debug_wifi_ssid, debug_wifi_password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi...");
      clignoteraffichage();
    }
    Serial.println("Connected to the WiFi network");
    //print IP and info
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    WiFi.softAPdisconnect(true);
    spas.addClient(MqttClient);
    MqttClient.begin(settings, WifiClient);
    if (spas.begin(MySettings))
    {
      Serial.println("Server begin succeeded");
    } else {
      Serial.println("Server begin failed");
      exit(EXIT_FAILURE);
    }
  }

  

}

void loop() {
  // put your main code here, to run repeatedly:
  spas.handle();
}