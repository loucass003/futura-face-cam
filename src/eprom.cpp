#include "FuturaFaceTracker.h"


void FuturaFaceTracker::loadEprom() {
    this->preferences.begin("fftracker", false);

    this->flash = this->preferences.getUChar("flash", 255);
    log_i("EPROM settings loaded");
}