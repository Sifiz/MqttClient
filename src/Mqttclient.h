#pragma once
#include <Arduino.h>
#include <SpaIot.h>
#include <PubSubClient.h>

class mqqttSettings

{
    public:
        int port;
        String server;
        String user;
        String password;
        String topic;
        mqqttSettings(String server, String user ,String password, String topic, int port = 1883)
        : 
        server(server), user(user), password(password), topic(topic), port(port) {}
        mqqttSettings() : port(1883)  {};
        mqqttSettings(String server, String topic, int port = 1883)
        : 
        server(server), topic(topic), port(port) {}
};
class mqttClientClass : public SpaIot::SpaClient{ //dérivation de la classe spaclient //
    public:
    mqttClientClass(); //constructeur par défault
    void begin(const mqqttSettings & settings, Client & client); //fonction de connexion au serveur mqtt
    bool handle(); //fonction de gestion des messages mqtt
    bool isOpen() const; //fonction de test de connexion
    void reconnect(); //fonction de reconnexion
    private:
    mqqttSettings m_settings; //paramètres de connexion
    mutable PubSubClient m_client; //client mqtt
    static void callback(const char topic, byte* payload, unsigned int length);
};
extern mqttClientClass Mqttclient;