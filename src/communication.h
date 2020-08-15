/*
Hugo ESP8266 Plus by Hayden Donald 2020
https://github.com/haydendonald/Hugo-ESP8266-Plus

This project is forked from Hugo ESP8266 by NiceThings (Martin Cerny) https://github.com/mcer12/Hugo-ESP8266
It uses the Hugo 4 button wifi remote which can be purchased from https://www.tindie.com/products/nicethings/hugo-esp8266-4-button-wifi-remote/

communication.h - Responsible for sending messages to the network
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

//Send out a packet
void sendOut(DynamicJsonDocument msg) {
    int port = json["outPort"].as<int>();
    IPAddress broadcast = WiFi.localIP(); broadcast[3] = 255;
    Serial.print("Sending message to "); Serial.print(broadcast); Serial.print(":"); Serial.println(port);

    udp.beginPacket(broadcast, port);
    serializeJson(msg, udp);
    udp.endPacket();
}

//Decide what action should be taken based on the button states when booted
void decideAction() {
    DynamicJsonDocument msg(2048);
    msg["device"] = "Hugo_" + macLastThreeSegments(mac);
    msg["ipAddress"] = (String)WiFi.localIP()[0] + "." + (String)WiFi.localIP()[1] + "." + (String)WiFi.localIP()[2] + "." + (String)WiFi.localIP()[3];
    msg["mac"] = (String)mac[0] + ":" + (String)mac[1] + ":" + (String)mac[2] + ":" + (String)mac[3] + ":" + (String)mac[4] + ":" + (String)mac[5];
    msg["batteryLevel"] = batteryLevel();
    msg["frendlyBatteryLevel"] = friendlyBatteryLevel();
    msg["timeTaken"] = millis() - startTime;


    sendOut(msg);
}






