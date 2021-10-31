#include "ESPmDNS.h"
#include "configuration.h"
#include "FuturaFaceTracker.h"

void FuturaFaceTracker::initServer() {
    this->webServer = new AsyncWebServer(SERVER_PORT);

    this->webServer->on("/status", HTTP_GET, [this] (AsyncWebServerRequest *req) { this->statusHandler(req); });
    
    Serial.println("Starting web server");
    this->webServer->begin();
}

void FuturaFaceTracker::statusHandler(AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hello, GET: " + this->getDeviceName());
}