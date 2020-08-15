/*
Hugo ESP8266 Plus by Hayden Donald 2020
https://github.com/haydendonald/Hugo-ESP8266-Plus

This project is forked from Hugo ESP8266 by NiceThings (Martin Cerny) https://github.com/mcer12/Hugo-ESP8266
It uses the Hugo 4 button wifi remote which can be purchased from https://www.tindie.com/products/nicethings/hugo-esp8266-4-button-wifi-remote/

configUtility.h - Responsible for the webserver to configure the device

Configuration is at 10.10.10.1:80
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);
#define CONFIG_HTML "<!DOCTYPE html>\n<html>\n    <head>\n        <title>Hugo Plus Configuration</title>\n\n        <style>\n            html {\n                background-color: black;\n                color: white;\n            }\n            section {\n                margin-bottom: 5vh;\n            }\n        </style>\n    </head>\n    <body>\n        <center>\n            <h1>Hugo Plus Configuration</h1>\n            <form method=\"post\" action=\"/\">\n                <section>\n                    <h2>Wifi</h2>\n                    <label for=\"ssid\">SSID</label></br><input type=\"text\" id=\"ssid\" name=\"ssid\" value=\"\"></br>\n                    <label for=\"wifiPass\">Password</label></br><input type=\"password\" id=\"wifiPass\" name=\"wifiPass\" value=\"\"></br>\n                </section>\n                <section>\n                    <h2>Static IP</h2>\n                    <label for=\"staticIP\">IP Address</label></br><input type=\"text\" id=\"staticIP\" name=\"staticIP\" value=\"\"></br>\n                    <label for=\"staticGateway\">Gateway</label></br><input type=\"text\" id=\"staticGateway\" name=\"staticGateway\" value=\"\"></br>\n                    <label for=\"staticSubnet\">Subnet</label></br><input type=\"text\" id=\"staticSubnet\" name=\"staticSubnet\" value=\"\"></br>\n                </section>\n                <section>\n                    <h2>Device</h2>\n                    <label for=\"deviceName\">Name</label></br><input type=\"text\" id=\"deviceName\" name=\"deviceName\" value=\"\"></br>\n                </section>\n                <section>\n                    <h2>UDP</h2>\n                    <label for=\"outPort\">Outgoing Port</label></br><input type=\"number\" id=\"outPort\" name=\"outPort\" value=\"\"></br>\n                </section>\n                <button type=\"submit\">Save settings</button>\n            </form>\n        </center>\n    </body>\n</html>"
#define SAVE_HTML "<!DOCTYPE html>\n<html>\n    <head>\n        <title>Hugo Plus Configuration</title>\n\n        <style>\n            html {\n                background-color: black;\n                color: white;\n            }\n            section {\n                margin-bottom: 5vh;\n            }\n        </style>\n    </head>\n    <body>\n        <center>\n            <h1>Hugo Plus Configuration</h1>\n            <h2>Saved settings. Going back to sleep</h2>\n        </center>\n    </body>\n</html>"
#define ERROR_HTML "<!DOCTYPE html>\n<html>\n    <head>\n        <title>Hugo Plus Configuration</title>\n\n        <style>\n            html {\n                background-color: black;\n                color: white;\n            }\n            section {\n                margin-bottom: 5vh;\n            }\n        </style>\n\n        <meta http-equiv=\"refresh\" content=\"5\">\n    </head>\n    <body>\n        <center>\n            <h1>Hugo Plus Configuration</h1>\n            <h2>An error occured. Please check your settings and try again</h2>\n        </center>\n    </body>\n</html>"

void handleClient() {
    if(server.args()) {
        Serial.print("Got settings update request ");
        char *keys[] = {
            "ssid",
            "wifiPass",
            "staticIP",
            "staticGateway",
            "staticSubnet",
            "deviceName",
            "outPort"
        };

        for(int i = 0; i < 6; i++) {
            if(server.hasArg(keys[i])) {
                if(!(keys[i] == "wifiPass" && server.arg(keys[i]) == "")) {
                    Serial.print((String)keys[i] + ",");
                    json[keys[i]] = server.arg(keys[i]);
                }
            }
            else {
                server.send(200, "text/html", ERROR_HTML);
                return;
            }
        }

        server.send(200, "text/html", SAVE_HTML);
        saveConfig();
        Serial.println(" changed going to sleep..");
        sleep();
    }
    else {
        char *keys[] = {
            "ssid",
            "wifiPass",
            "staticIP",
            "staticGateway",
            "staticSubnet",
            "deviceName",
            "outPort"
        };

        //Add the current values to their inputs
        String html = CONFIG_HTML;
        String newHTML = "";
        int index = 0;
        int lastIndex = 0;
        int i = 0;
        while(true) {
            index = html.indexOf("value", index + 1);
            if(index == -1){
                newHTML += html.substring(lastIndex);
                break;
            }
            else {
                String add = "value='" + (String)json[keys[i]].as<const char*>() + "'></br>";
                if(keys[i] == "wifiPass"){add = "value=''></br>";} //Don't add the password value
                newHTML += html.substring(lastIndex, index);
                newHTML += add;
                lastIndex = index + add.length();
            }

            i++;
        }

        server.send(200, "text/html", newHTML);
    }
}

void setupConfigUtility() {
    json["configUtilityWasOpened"] = true;
    saveConfig();
    WiFi.disconnect();
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(IPAddress(10, 10, 10, 1), IPAddress(10, 10, 10, 1), IPAddress(255, 255, 255, 0));
    WiFi.softAP(((String)AP_SSID).c_str());

    Serial.print("Configuration AP open at " + (String)AP_SSID + ", "); Serial.println(WiFi.softAPIP());
    server.on("/", handleClient);
    server.begin();


    //Stay in configuration mode until a button is clicked or settings are updated
    unsigned long timeout = millis();
    unsigned long enableExitTimeout = millis() + 5000;
    while(true) {
        server.handleClient();

        if(timeout < millis()){
            digitalWrite(STATUS_LED, !digitalRead(STATUS_LED));
            timeout = millis() + 1000;
        }

        //If any button is held restart
        if(enableExitTimeout < millis()) {
            for(int i = 0; i < TOTAL_BUTTONS; i++) {
                if(isButtonPressed(i) && isButtonHeld(i)) {
                    Serial.println("Button was held going back to sleep");
                    sleep();
                }
            }
        }
    }
}