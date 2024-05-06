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
    m_client.setCallback(mqttClientClass::callback);


    //store settings
    //call reconnect
    if (m_client.connected())
    {
        m_client.subscribe(settings.topic.c_str());
    }
}
bool mqttClientClass::handle()
{
    //if no connection, call reconnect
    if (!m_client.connected())
    {
        reconnect();
    }
    // call m_client.loop
    m_client.loop();
    // process message incoming from spa, by calling pullFromSpa()
    
    // if message from spa, call m_client.publish and return true, else return false
    // if message from spa transfer mqtt, call m_client.publish and return true, else return false
    
    return false;
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
}

void mqttClientClass::callback(char* topic, byte *payload, unsigned int length)
{
     String p;
     String t (topic);
     Event event;
     p.concat((char *)payload, length);
     Serial.printf("message recu du topic : %s=%s\n", topic, p.c_str());
      t.remove (0, t.lastIndexOf ("/") + 1);
      event.setType (MqttStringToType.at (t));
          event.setValue (p.toInt()); // Convert string to unsigned short
      Serial.printf("\tEnvoi de l'event %s vers le spa\n", event.toString().c_str());
      SpaMqttClient.pushToSpa (event);
}
mqttClientClass SpaMqttClient; // singleton object