#include "FuturaFaceTracker.h"

FuturaFaceTracker *tracker;

void setup() {
    tracker = new FuturaFaceTracker();
    tracker->init();
}

void loop() {
    tracker->loop();
}