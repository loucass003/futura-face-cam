#include "ESPmDNS.h"
#include "configuration.h"
#include "FuturaFaceTracker.h"
#include <math.h>

void FuturaFaceTracker::initServer() {
    this->webServer = new AsyncWebServer(SERVER_PORT);

    this->webServer->on("/status", HTTP_GET, [this] (AsyncWebServerRequest *req) { this->statusHandler(req); });
    this->webServer->on("/set-flash", HTTP_POST, [this] (AsyncWebServerRequest *req) { this->setFlashHandler(req); });
    
    Serial.println("Starting web server");
    this->webServer->begin();
}

void FuturaFaceTracker::statusHandler(AsyncWebServerRequest *request) {
    DynamicJsonDocument doc(200);

    doc["battery"] = this->battery;
    doc["flash"] = this->flash;

    String str;
    serializeJson(doc, str);
    request->send(200, "application/json", str);
}

void FuturaFaceTracker::setFlashHandler(AsyncWebServerRequest *request) {
    DynamicJsonDocument doc(200);
    doc["ok"] = false;
    if(request->hasParam("value", true)) {
        AsyncWebParameter* p = request->getParam("value", true);
        this->flash = atoi(p->value().c_str());
        this->preferences.putUChar("flash", this->flash);
        doc["ok"] = true;
    }
    String str;
    serializeJson(doc, str);
    request->send(200, "application/json", str);
}