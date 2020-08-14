/*
Hugo ESP8266 Plus by Hayden Donald 2020
https://github.com/haydendonald/Hugo-ESP8266-Plus

This project is forked from Hugo ESP8266 by NiceThings (Martin Cerny) https://github.com/mcer12/Hugo-ESP8266
It uses the Hugo 4 button wifi remote which can be purchased from https://www.tindie.com/products/nicethings/hugo-esp8266-4-button-wifi-remote/

fileSystem.h - Responsible for handling the file system
*/

#include <Arduino.h>
#include <FS.h>
#include <ArduinoJson.h>

DynamicJsonDocument json(2048);

//Save the current configuration to the file system
bool saveConfig() {
    File file = SPIFFS.open("/hugoConfig.json", "w");

    if(!file) {
        Serial.println("Failed to save the configuration file");
        return false;
    }

    serializeJson(json, file);
    file.close();
    return true;
}

//Read the configuration from the file system
bool readConfig() {
    File file = SPIFFS.open("/hugoConfig.json", "r");

    //If we fail to read the file initalise it
    if(!file) {
        file.close();

        Serial.println("Setting configuration defaults");
        json["ssid"] = "";
        json["wifiPass"] = "";

        json["outPort"] = DEFAULT_UDP_PORT;
        
        json["staticIP"] = "";
        json["staticGateway"] = "";
        json["staticSubnet"] = "";

        return saveConfig();
    }

    DeserializationError error = deserializeJson(json, file.readString());
    file.close();
    return error == DeserializationError::Ok;
}

//Returns a string value
const char* readStringValueFromMemory(String key) {
    return json["key"].as<const char*>();
}