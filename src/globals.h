/*
Hugo ESP8266 Plus by Hayden Donald 2020
https://github.com/haydendonald/Hugo-ESP8266-Plus

This project is forked from Hugo ESP8266 by NiceThings (Martin Cerny) https://github.com/mcer12/Hugo-ESP8266
It uses the Hugo 4 button wifi remote which can be purchased from https://www.tindie.com/products/nicethings/hugo-esp8266-4-button-wifi-remote/

globals.h - Global definitions
*/

#include <Arduino.h>

//Device settings
#define TOTAL_BUTTONS 4
const int buttonPins[TOTAL_BUTTONS] =  {14, 4, 12, 13};
#define STATUS_LED 5
#define OTA_PIN 16
#define HELD_BUTTON_DELAY 500
#define BATTERY_PIN A0
#define TIME_TO_SLEEP 20000

#define AP_SSID "HugoConfig_" + macLastThreeSegments(mac)
#define DEFAULT_UDP_PORT 4922

#define WIFI_CONNECTION_ERROR 3
#define WIFI_CONNECTION_ERROR_INVALID_STATIC 8
#define FILE_SYSTEM_ERROR_FLASHES 5

bool buttonStates[TOTAL_BUTTONS];
bool buttonWasHeld = false;

enum DeviceMode {
    Boot,
    NormalOperation,
    Configuration
};

//Goto sleep
void sleep() {
    yield();
    delay(5);
    ESP.deepSleep(0);
    yield();
}

//Flash the led for a critical error. This is blocking and will reset the processor after flashing
void critialErrorFlasherBlocking(int flashes) {
    digitalWrite(STATUS_LED, LOW);

    //Flash the led count 3 times then reboot
    for(int i = 0; i < 3; i++) {
        for(int i = 0; i < flashes; i++) {
            digitalWrite(STATUS_LED, HIGH);
            delay(300);
            digitalWrite(STATUS_LED, LOW);
            delay(300);
        }
        delay(2000);
    }

    sleep();
}

String macLastThreeSegments(const uint8_t* mac) {
  String result;
  for (int i = 3; i < 6; ++i) {
    if (mac[i] < 0x10) result += "0";
    result += String(mac[i], HEX);
  }
  result.toUpperCase();
  return result;
}

//Was a button pressed when we woke up?
bool wasButtonPressed(int button) {
    return buttonStates[button];
}

//Is a button pressed?
bool isButtonPressed(int button) {
    return digitalRead(buttonPins[button]);
}

//Check if a button is being held down
bool isButtonHeld(int button) {
    delay(HELD_BUTTON_DELAY);
    return isButtonPressed(button);
}

//Return the battery level in %. 255% means charging
float batteryLevel() {
    int levelRaw = 0;
    for(int i = 0; i < 10; i++){
        levelRaw += analogRead(BATTERY_PIN);
        delay(10);
    }
    levelRaw = levelRaw / 10;

    //Charging
    if(levelRaw > 1000){return 255.0;}
    if(levelRaw > 868){
        return ((1000 - 868) / 132) * 100.0;
    }
    else {return 0.0;}
}

String friendlyBatteryLevel() {
    return batteryLevel() == 255.0 ? "Charging" : (String)batteryLevel() + "%";
}