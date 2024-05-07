#pragma once
#include <Arduino.h>
#include <SpaIot.h>
#include <PubSubClient.h>

class mqttSettings
{
    public:
        int port;
        int buffer;
        int bufferSize;
        String server;
        String user;
        String password;
        String topic;
        String clientId;
        mqttSettings(String server, String user ,String password, String topic, String clientId, int port = 1883)
        : 
        server(server), user(user), password(password), topic(topic), clientId(clientId), port(port) {}
        mqttSettings() : port(1883)  {};
        mqttSettings(String server, String topic, String clientId, int port = 1883)
        : 
        server(server), topic(topic), port(port) {}
};
class mqttClientClass : public SpaIot::SpaClient
{ //dérivation de la classe spaclient //
    public:
    mqttClientClass(); //constructeur par défault
    void begin(const mqttSettings & settings, Client & client); //fonction de connexion au serveur mqtt
    bool handle(); //fonction de gestion des messages mqtt
    bool isOpen() const; //fonction de test de connexion
    void reconnect(); //fonction de reconnexion
    private:
    mqttSettings m_settings; //paramètres de connexion
    mutable PubSubClient m_client; //client mqtt
    static void callback(char* topic, byte* payload, unsigned int length);
    static const std::map<String, SpaIot::Event::Type> MqttStringToType;
    static const std::map<SpaIot::Event::Type, String> EventToString;
};
extern mqttClientClass MqttClient;