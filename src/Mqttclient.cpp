#include "Mqttclient.h"
#include <map>

using namespace SpaIot;

const std::map<Event::Type, String> TypeToString = {
    {Event::Type::PowerOn, "power"},
    {Event::Type::BubbleOn, "bubble"},
    };

mqttClientClass::mqttClientClass() : SpaIot::SpaClient("mqqttClientClass")
{
    TypeToString.at(Event::Type::PowerOn);
}
void mqttClientClass::begin(const mqqttSettings & settings, Client & client){
    m_client.setServer(settings.server.c_str(), settings.port);
    //configure mqtt client
    //store settings
    //call reconnect
    if (m_client.connected())
    {
        m_client.subscribe(settings.topic.c_str());
    }
    else
    {
        reconnect();
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
    // process message incoming from spa, by calling pullFromSpa()
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
        if (m_client.connect(m_settings.user.c_str(), m_settings.password.c_str()))
        {
            //if connected, publish an announcement
            m_client.publish(m_settings.topic.c_str(), "hello world");
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