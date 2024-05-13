#pragma once
#include <Arduino.h>

class Portal
{
public:
    Portal();
    String initPortal();
public:
    const char *ssid;     // FYI The SSID can't have a space in it.
    const char *password; // no password
    String wifiSSID;
    String wifiPassword;
};