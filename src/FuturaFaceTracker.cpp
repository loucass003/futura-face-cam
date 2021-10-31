#include <Arduino.h>
#include "ESPmDNS.h"
#include "configuration.h"
#include "FuturaFaceTracker.h"

void FuturaFaceTracker::init() {
    Serial.begin(115200);
    delay(3000);
   
    this->portal = new AutoConnect(this->portalServer);
    this->apConfig = new AutoConnectConfig(AP_NAME, AP_PASSWORD);
    this->apConfig->preserveAPMode = false;
    this->apConfig->ota = AC_OTA_EXTRA;
    this->portal->config(*this->apConfig);
    if (this->portal->begin()) {
        Serial.println("WiFi connected: " + WiFi.localIP().toString());
        this->configureCamera();

        while (!this->initCamera()) {
            delay(1000);
        }
        this->initMDNS();
        this->initStreamServer();
        this->initServer();
        this->initOTA();
    }
}


void FuturaFaceTracker::initOTA() {
    ArduinoOTA.setHostname(this->getDeviceName().c_str());
    // No authentication by default
    //ArduinoOTA.setPassword(OTA_PASSWORD);

    ArduinoOTA.onStart([]() {
        Serial.println("Start");
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    ArduinoOTA.begin();
}

void FuturaFaceTracker::initMDNS() {
    if (MDNS.begin(this->getDeviceName().c_str())) {
        MDNS.addService("futura", "tcp", SERVER_PORT);
        MDNS.addServiceTxt("futura", "tcp", "stream_port", STR(STREAM_PORT));
        MDNS.addServiceTxt("futura", "tcp", "api_port", STR(SERVER_PORT));
        MDNS.addServiceTxt("futura", "tcp", "version", VERSION);
        MDNS.addServiceTxt("futura", "tcp", "id", String(this->getDeviceId(), HEX));
        MDNS.addServiceTxt("futura", "tcp", "type", "FuturaFaceTracker");
    }
    
}

void FuturaFaceTracker::loop() {
    this->portal->handleClient();
    ArduinoOTA.handle();
}