/*
Hugo ESP8266 Plus by Hayden Donald 2020
https://github.com/haydendonald/Hugo-ESP8266-Plus

This project is forked from Hugo ESP8266 by NiceThings (Martin Cerny) https://github.com/mcer12/Hugo-ESP8266
It uses the Hugo 4 button wifi remote which can be purchased from https://www.tindie.com/products/nicethings/hugo-esp8266-4-button-wifi-remote/

main.cpp - The entry point
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include "globals.h"


#define SOFTWARE_VERSION_MAJOR 0
#define SOFTWARE_VERSION_MINOR 1

DeviceMode deviceMode = DeviceMode::Boot;
byte mac[6];

#include "fileSystem.h"
#include "configUtility.h"

bool buttonStates[TOTAL_BUTTONS];
bool buttonWasHeld = false;

//Updates the button states array
void readButtons() {
    for(int i = 0; i < TOTAL_BUTTONS; i++) {
        buttonStates[i] = isButtonPressed(i);
    }
}

//Check if a button is pressed
bool isAButtonHeld() {
  for(int i = 0; i < TOTAL_BUTTONS; i++) {
    if(buttonStates[i]){return isButtonHeld(i);}
  }
}

void setup() {
  //Initialize hardware
  for(int i = 0; i < TOTAL_BUTTONS; i++) {pinMode(buttonPins[i], INPUT);}
  pinMode(OTA_PIN, OUTPUT);
  digitalWrite(OTA_PIN, LOW);
  pinMode(STATUS_LED, OUTPUT);

  //Read the button states as fast as possible when we wake up
  readButtons();
  buttonWasHeld = isAButtonHeld();

  //Flash to show we got it
  digitalWrite(STATUS_LED, HIGH);
  delay(buttonWasHeld ? 1000 : 100);
  digitalWrite(STATUS_LED, LOW);

  Serial.begin(115200);
  Serial.println("Hugo ESP8266 Plus By Hayden Donald 2020");
  Serial.println(String("Version: ") + SOFTWARE_VERSION_MAJOR + String(".") + SOFTWARE_VERSION_MINOR);
  Serial.println(String("Last Compile Date: ") + __DATE__);
  Serial.println("");
  Serial.print("Button(s) ");
  bool aButtonWasClicked = false;
  for(int i = 0; i < TOTAL_BUTTONS; i++){
    if(buttonStates[i]){Serial.print((String)i + ", "); aButtonWasClicked = true;}
  }
  if(!aButtonWasClicked){Serial.print("(none) ");}
  if(buttonWasHeld){Serial.println("were held");} else {Serial.println("were clicked");}

  //Initialize file system and read the settings
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    critialErrorFlasherBlocking(FILE_SYSTEM_ERROR_FLASHES);
  }
  WiFi.macAddress(mac);
  if(!readConfig()){
    Serial.println("An error occured reading the configuration from the file system");
    critialErrorFlasherBlocking(FILE_SYSTEM_ERROR_FLASHES);
  }

  //If both the top and botton buttons are pressed at boot enter configuration
  if(isButtonHeld(0) && isButtonPressed(3)) {
    Serial.println("Configuration mode requested");
    deviceMode = DeviceMode::Configuration;
    digitalWrite(STATUS_LED, HIGH);
  }

  //If we're at boot attempt to connect to wifi
  if(deviceMode == DeviceMode::Boot) {
    const char* ssid = json["ssid"].as<const char*>();
    const char* pass = json["wifiPass"].as<const char*>();
    if(ssid[0] != '\0' && pass[0] != '\0') {
      //Attempt to connect
      WiFi.mode(WIFI_STA);

      //Serial.println(ssid);
          ESP.deepSleep(0);

      const char* staticIP = readStringValueFromMemory("staticIP");
      const char* staticGateway = readStringValueFromMemory("staticGateway");
      const char* staticSubnet = readStringValueFromMemory("staticSubnet");
      if(staticIP[0] != '\0' && staticGateway[0] != '\0' && staticSubnet[0] != '\0') {
        //Set the static ip
        IPAddress ipAddress, gateway, subnet;
        if(ipAddress.fromString(staticIP) && gateway.fromString(staticGateway) && subnet.fromString(staticSubnet)) {
          WiFi.config(ipAddress, gateway, subnet);
        }
        else {
          Serial.println("Invalid static ip");
        }
      }

      //Ok attempt to connect
      Serial.print("Attempting to connect to " + (String)ssid);
      WiFi.begin(ssid, pass);
      unsigned long timeout = millis() + 10000;
      while(timeout > millis() && WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
      }
      if(WiFi.status() == WL_CONNECTED) {

      }
      else {
        Serial.println(" Failed");
        delay(10000);
        ESP.restart();
      }





    }
    else {
      //No config set enter configuration
      deviceMode = DeviceMode::Configuration;
      Serial.println("Invalid wifi settings enabling configuration mode");
    }
  }

  //Go into config mode if required or if we fail to connect to wifi
  if(deviceMode == DeviceMode::Configuration) {
    setupConfigUtility();
  }





}

void loop() {
  Serial.println("working");
  delay(1000);
}