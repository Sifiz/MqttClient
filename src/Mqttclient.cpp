#include "Mqttclient.h"
#include <map>

using namespace SpaIot;

const std::map<Event::Type, String> TypeToString = {
    {Event::Type::PowerOn, "power"},
    {Event::Type::BubbleOn, "bubble"},
    {Event::Type::JetOn, "jet"},
    {Event::Type::SanitizerOn, "sanitizer"},
    {Event::Type::HeaterOn, "heater"},
    {Event::Type::HeatReached, "heatreached"},
    {Event::Type::WaterTemp, "watertemp"},
    {Event::Type::DesiredTemp, "desiredtemp"},
    {Event::Type::SetDesiredTemp, "setdesiredtemp"},
    {Event::Type::SanitizerTime, "sanitizertime"},
    {Event::Type::ErrorCode, "errorcode"},
    {Event::Type::AmbientTemperature, "ambienttemp"},
    {Event::Type::NoEvent, "noevent"},
    {Event::Type::AnyEvent, "anyevent"},
    };

mqttClientClass::mqttClientClass() : SpaIot::SpaClient("mqttClientClass")
{
    TypeToString.at(Event::Type::PowerOn);
    TypeToString.at(Event::Type::BubbleOn);
    TypeToString.at(Event::Type::JetOn);
    TypeToString.at(Event::Type::SanitizerOn);
    TypeToString.at(Event::Type::HeaterOn);
    TypeToString.at(Event::Type::HeatReached);
    TypeToString.at(Event::Type::WaterTemp);
    TypeToString.at(Event::Type::DesiredTemp);
    TypeToString.at(Event::Type::SetDesiredTemp);
    TypeToString.at(Event::Type::SanitizerTime);
    TypeToString.at(Event::Type::ErrorCode);
    TypeToString.at(Event::Type::AmbientTemperature);
    TypeToString.at(Event::Type::NoEvent);
    TypeToString.at(Event::Type::AnyEvent);

}

void mqttClientClass::begin(const mqttSettings & settings, Client & client)
{
    m_client.setServer(settings.server.c_str(), settings.port);
    //configure mqtt client

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

void mqttClientClass::callback(const char topic, byte *payload, unsigned int length)
{

}
