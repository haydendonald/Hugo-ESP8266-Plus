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
WiFiUDP udp;
unsigned long startTime = millis();

#include "fileSystem.h"
#include "configUtility.h"
#include "communication.h"

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
  digitalWrite(STATUS_LED, HIGH);

  //Read the button states as fast as possible when we wake up
  readButtons();
  buttonWasHeld = isAButtonHeld();

  Serial.begin(115200);
  Serial.println();
  Serial.println("Hugo ESP8266 Plus By Hayden Donald 2020");
  Serial.println(String("Version: ") + SOFTWARE_VERSION_MAJOR + String(".") + SOFTWARE_VERSION_MINOR);
  Serial.println(String("Last Compile Date: ") + __DATE__);
  Serial.println();
  Serial.println("Battery Level " + friendlyBatteryLevel());
  Serial.print("Button(s) ");
  bool aButtonWasClicked = false;
  for(int i = 0; i < TOTAL_BUTTONS; i++){
    if(buttonStates[i]){Serial.print((String)i + ", "); aButtonWasClicked = true;}
  }
  if(!aButtonWasClicked){Serial.println(" were not clicked");}
  else {
    if(buttonWasHeld){Serial.println("were held");} else {Serial.println("were clicked");}
  }

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
  if(wasButtonPressed(0) && wasButtonPressed(3) && buttonWasHeld) {
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
      if(json["configUtilityWasOpened"].as<bool>()) {
          Serial.println("Reset wifi settings");
          json["configUtilityWasOpened"] = false;
          saveConfig();
          //If the wifi configuration was changed update it
          WiFi.mode(WIFI_STA);
          const char* staticIP = json["staticIP"].as<const char*>();
          const char* staticGateway = json["staticGateway"].as<const char*>();
          const char* staticSubnet = json["staticSubnet"].as<const char*>();
          if(staticIP[0] != '\0' && staticGateway[0] != '\0' && staticSubnet[0] != '\0') {
            //Set the static ip
            IPAddress ipAddress, gateway, subnet;
            if(ipAddress.fromString(staticIP) && gateway.fromString(staticGateway) && subnet.fromString(staticSubnet)) {
              Serial.print("Setting static ip to IP: ");Serial.print(ipAddress);Serial.print(" Gateway: ");Serial.print(gateway);Serial.print(" Subnet: ");Serial.println(subnet);
              WiFi.config(ipAddress, gateway, subnet);
            }
            else {
              Serial.println("Invalid static ip");
              critialErrorFlasherBlocking(WIFI_CONNECTION_ERROR_INVALID_STATIC);
            }
          }
      }

      //Ok attempt to connect
      Serial.print("Attempting to connect to " + (String)ssid);
      if(json["configUtilityWasOpened"].as<bool>()) {
        WiFi.begin(ssid, pass);
        WiFi.persistent(true);
        WiFi.setAutoConnect(true);
        WiFi.setAutoReconnect(true);
      }

      if(WiFi.waitForConnectResult() == WL_CONNECTED) {
        Serial.print(" Connected! IP: ");
        Serial.print(WiFi.localIP());
        Serial.print(" MAC: ");
        Serial.println(WiFi.macAddress());
        udp.begin(json["outPort"].as<int>());
        deviceMode = DeviceMode::NormalOperation;
        Serial.println("Took " + (String)(millis() - startTime) + "ms");
      }
      else {
        Serial.println(" Failed");
        WiFi.disconnect();
        json["configUtilityWasOpened"] = true;
        saveConfig();
        delay(10000);
        critialErrorFlasherBlocking(WIFI_CONNECTION_ERROR);
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

  //Do normal functions
  if(deviceMode == DeviceMode::NormalOperation) {
      decideAction();
  }

  //Flash to show we got it
  digitalWrite(STATUS_LED, LOW);
  delay(buttonWasHeld ? 1000 : 100);
  digitalWrite(STATUS_LED, HIGH);
  delay(buttonWasHeld ? 1000 : 100);
  digitalWrite(STATUS_LED, LOW);

  sleep();
}

void loop() {
}