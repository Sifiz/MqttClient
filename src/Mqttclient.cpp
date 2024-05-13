#include "Mqttclient.h"
#include <map>

using namespace SpaIot;
//convert string to event type
const std::map<String, Event::Type> mqttClientClass::MqttStringToType = {
    {"power", Event::Type::PowerOn},
    {"bubble", Event::Type::BubbleOn},
    {"heater", Event::Type::HeaterOn},
    {"heat_reached", Event::Type::HeatReached},
    {"desired_temp", Event::Type::SetDesiredTemp},
    {"water_temp", Event::Type::WaterTemp},
    {"error_code", Event::Type::ErrorCode},
    {"no_event", Event::Type::NoEvent},
    {"any_event", Event::Type::AnyEvent},
};

//convert event type to string
const std::map<SpaIot::Event::Type, String> mqttClientClass::EventToString = {
    {Event::Type::PowerOn, "power"},
    {Event::Type::BubbleOn, "bubble"},
    {Event::Type::HeaterOn, "heater"},
    {Event::Type::HeatReached, "heatreached"},
    {Event::Type::WaterTemp, "watertemp"},
    {Event::Type::DesiredTemp, "desiredtemp"},
    {Event::Type::ErrorCode, "errorcode"},
    {Event::Type::NoEvent, "noevent"},
    {Event::Type::AnyEvent, "anyevent"},
    };

// Constructor
mqttClientClass::mqttClientClass () : SpaClient ("mqttClientClass") {}


void mqttClientClass::begin(const mqttSettings & settings, Client & client)
{
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
     Serial.printf("message recu du topic : %s=%s\n", topic, p.c_str());
  t.remove (0, t.lastIndexOf ("/") + 1); // remove the prefix to get the command

  event.setType (MqttStringToType.at (t));
  if (event.isBoolean()) {

    event.setValue (p == "on");
  }
  else {

    event.setValue (p.toInt());
  }
      Serial.printf("\tEnvoi de l'event %s vers le spa\n", event.toString().c_str());
      MqttClient.pushToSpa (event);
}
mqttClientClass MqttClient; // singleton object