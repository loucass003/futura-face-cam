#include <Arduino.h>
#include "ESPmDNS.h"
#include "configuration.h"
#include "FuturaFaceTracker.h"

bool wifi_error_blink(FuturaFaceTracker *t)
{
    static bool toggle = false;

    if (WiFi.status() != WL_CONNECTED)
    {
        t->leds[0] = toggle ? CRGB::Red : CRGB::Blue;
        toggle = !toggle;
        FastLED.show();
    }
    return true;
}

void FuturaFaceTracker::onConnect(IPAddress &clientIP)
{
    log_i("WiFi connected: %s", WiFi.localIP().toString());
    this->configureCamera();

    while (!this->initCamera())
    {
        delay(1000);
    }
    this->initMDNS();
    this->initStreamServer();
    this->initServer();
    this->initOTA();
    this->loadEprom();
}

void FuturaFaceTracker::init()
{
    Serial.begin(115200);

    delay(3000); // DO NOT REMOVE ! NEVER !

    FastLED.addLeds<WS2811, 13, GRB>(this->leds, 1).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(255);

    this->wifiTimer.every(1000, wifi_error_blink, this);
    this->portal = new AutoConnect(this->portalServer);
    this->apConfig = new AutoConnectConfig(AP_NAME, AP_PASSWORD);
    this->apConfig->preserveAPMode = true;
    this->apConfig->ota = AC_OTA_EXTRA;
    this->portal->config(*this->apConfig);
    this->portal->onConnect([this](IPAddress &clientIP)
                            { this->onConnect(clientIP); });
    this->portal->whileCaptivePortal([this]()
                                     { this->wifiTimer.tick(); return true; });
    this->portal->begin();
}

void FuturaFaceTracker::initOTA()
{
    ArduinoOTA.setHostname(this->getDeviceName().c_str());
    // No authentication by default
    // ArduinoOTA.setPassword(OTA_PASSWORD);

    ArduinoOTA.onStart([]()
                       { log_i("Start"); });
    ArduinoOTA.onEnd([]()
                     { log_i("\nEnd"); });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                          { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });
    ArduinoOTA.onError([](ota_error_t error)
                       {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) log_i("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) log_i("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) log_i("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) log_i("Receive Failed");
        else if (error == OTA_END_ERROR) log_i("End Failed"); });
    ArduinoOTA.begin();

    log_i("OTA initialized");
}

void FuturaFaceTracker::initMDNS()
{
    if (MDNS.begin(this->getDeviceName().c_str()))
    {
        MDNS.addService("futura", "tcp", SERVER_PORT);
        MDNS.addServiceTxt("futura", "tcp", "stream_port", STR(STREAM_PORT));
        MDNS.addServiceTxt("futura", "tcp", "api_port", STR(SERVER_PORT));
        MDNS.addServiceTxt("futura", "tcp", "version", VERSION);
        MDNS.addServiceTxt("futura", "tcp", "id", String(this->getDeviceId(), HEX));
        MDNS.addServiceTxt("futura", "tcp", "type", "FuturaFaceTracker");
        log_i("MDNS initialized");
    }
}

void FuturaFaceTracker::loop()
{
    this->portal->handleClient();
    ArduinoOTA.handle();

    if (WiFi.status() == WL_CONNECTED)
    {
        this->leds[0] = CRGB::White;
        FastLED.setBrightness(this->flash);
    }
    this->wifiTimer.tick();
    FastLED.show();
}