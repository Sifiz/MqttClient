#include "Mqttclient.h"
mqttClientClass::mqttClientClass() : SpaIot::SpaClient("mqqttClientClass")
{
}
void mqttClientClass::begin(const mqqttSettings & settings, Client & client){
    //configure mqtt client
    //store settings
    //call reconnect
}
bool mqttClientClass::handle()
{
    //if no connection, call reconnect
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
    //if connected, subscribe to topic
}

void mqttClientClass::callback(const char topic, byte *payload, unsigned int length)
{
}
