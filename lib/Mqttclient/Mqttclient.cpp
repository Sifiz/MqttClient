#include "Mqttclient.h"
#include <map>
const int photo_transistor = 32;
int pwmPin = 21;
const int PWM_CHANNEL = 0;    // ESP32 has 16 channels which can generate 16 independent waveforms
const int PWM_FREQ = 10000;     // Recall that Arduino Uno is ~490 Hz. Official ESP32 example uses 5,000Hz
const int PWM_RESOLUTION = 8; // We'll use same resolution as Uno (8 bits, 0-255) but ESP32 can go up to 16 bits
const int MAX_DUTY_CYCLE = 255;
const int DELAY_MS = 4;  // delay between fade increments

using namespace SpaIot;
//convert string to event type
const std::map<String, Event::Type> mqttClientClass::MqttStringToType = {
    {"power", Event::Type::PowerOn},
    {"bubble", Event::Type::BubbleOn},
    {"heater", Event::Type::HeaterOn},
    {"filter", Event::Type::FilterOn},
    {"heat_reached", Event::Type::HeatReached},
    {"desired_temp", Event::Type::SetDesiredTemp},
    {"water_temp", Event::Type::WaterTemp},
};

//convert event type to string
const std::map<SpaIot::Event::Type, String> mqttClientClass::EventToString = {
    {Event::Type::PowerOn, "power"},
    {Event::Type::BubbleOn, "bubble"},
    {Event::Type::HeaterOn, "heater"},
    {Event::Type::FilterOn, "filter"},
    {Event::Type::HeatReached, "heat_reached"},
    {Event::Type::WaterTemp, "water_temp"},
    {Event::Type::DesiredTemp, "desired_temp"},
    };

// Constructor
mqttClientClass::mqttClientClass () : SpaClient ("mqttClientClass") {}


void mqttClientClass::begin(const mqttSettings & settings, Client & client)
{
  this->m_settings = settings;
    //configure mqtt client
    m_client.setClient(client);
    m_client.setServer(settings.server.c_str(), settings.port);
    m_client.setCallback(mqttClientClass::callback); //set callback function


    //store settings

    //call reconnect
    if (m_client.connected())
    {
        m_client.subscribe(settings.topic.c_str());
    }
  pinMode(photo_transistor, INPUT);
    // Sets up a channel (0-15), a PWM duty cycle frequency, and a PWM resolution (1 - 16 bits) 
  // ledcSetup(uint8_t channel, double freq, uint8_t resolution_bits);
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);

  // ledcAttachPin(uint8_t pin, uint8_t channel);
  ledcAttachPin(pwmPin, PWM_CHANNEL);
}
bool mqttClientClass::handle()
{
    Event event;
    //if no connection, call reconnect
    if (!m_client.connected())
    {
        reconnect();
    }
    // call m_client.loop
    m_client.loop();
    // process message incoming from spa, by calling pullFromSpa()
  while (pullFromSpa (event)) {

    Serial.printf ("Spa Event %s received from spa\n", event.toString().c_str());
    if (EventToString.count (event.type()) != 0) {
      String topic, payload;

      topic = m_settings.topic + "/" + EventToString.at (event.type());
      if (event.isBoolean()) {

        payload = event.value() == true ? "on" : "off";
      }
      else {

        payload = String (event.value());
      }

      if (event.type() == Event::Type::PowerOn && event.value() == true) {
        // force the desired temperature to be published
        pushToSpa (Event (Event::Type::DesiredTemp, 0));
      }
      m_client.publish (topic.c_str(), payload.c_str());
    }
  }
  return true;
}

bool mqttClientClass::isOpen() const
{
    return m_client.connected();
}

void mqttClientClass::reconnect()
{
    //while not connected , reconnect
    while (!m_client.connected())
    {
        //attempt to connect
        if (m_client.connect(m_settings.clientId.c_str(), m_settings.user.c_str(), m_settings.password.c_str()))
        {
            //resubscribe
            m_client.subscribe(m_settings.topic.c_str());
        }
        else
        {
            //if not connected, print error and wait 5 seconds
            Serial.print("failed, rc=");
            Serial.print(m_client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
    //if connected, subscribe to topic
    String luxvaluecommand = m_settings.topic + "/command/luxvalue";
    m_client.subscribe(luxvaluecommand.c_str());
    String luxautocommand = m_settings.topic + "/command/luxauto";
    m_client.subscribe(luxautocommand.c_str());
    for (auto item : MqttStringToType)
    {
        const String &topic = item.first;
        String command = m_settings.topic + "/command/" + topic;
        Serial.printf("The client %s subscribes to the topic %s... ", m_settings.clientId.c_str(), command.c_str());
        if (m_client.subscribe(command.c_str()))
        {
            Serial.println("OK");
        }
        else
        {
            Serial.println("Failed");
        }
    }
}



void mqttClientClass::callback(char* topic, byte *payload, unsigned int length)
{
     String p;
     String t (topic);
     Event event;
     p.concat((char *)payload, length);
     Serial.printf("message recu du topic : %s='%s'\n", topic, p.c_str());
    t.remove (0, t.lastIndexOf ("/") + 1); // remove the prefix to get the command
     Serial.printf("t : %s\n", t.c_str());
     //supprime retour a la ligne de p
      p.replace("\n", "");
    //integrer lumiere
  while (t == "luxauto") {
      //change mode au ligt
      if (p == "on") {
        Serial.println("Lux auto received:");
        Serial.println(p);
        int lux = map(analogRead(photo_transistor), 0, 4095, 0, 255);
        ledcWrite(PWM_CHANNEL, lux);
        Serial.println(lux);
        //print to mqqtt
        MqttClient.m_client.publish("spa/luxauto", p.c_str());
      }}
  if (t == "luxvalue" ) {
    //change frequency of pmw
    Serial.println("Lux value received:");
    Serial.println(p.toInt());
    int dutyCycle = p.toInt();
    ledcWrite(PWM_CHANNEL, map(dutyCycle, 0, 255, 255, 0));
    //publish to mqqtt
    MqttClient.m_client.publish("spa/luxvalue", p.c_str());
  }
  else{
  event.setType (MqttStringToType.at (t));
  if (event.isBoolean()) {

    event.setValue (p == "on");
    Serial.print("On");
  }
  else {

    event.setValue (p.toInt());

  }
      Serial.printf("\tEnvoi de l'event %s vers le spa\n", event.toString().c_str());
      MqttClient.pushToSpa (event);
}}
mqttClientClass MqttClient; // singleton object