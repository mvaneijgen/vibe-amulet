#ifndef DEBUG_H
#define DEBUG_H

#include <Arduino.h>

void printDebugInfo(int buttonState, bool isMelodyPlaying, unsigned long timerStartTime) {
    Serial.print("Button State: ");
    Serial.print(buttonState == LOW ? "Pressed" : "Released");
    Serial.print(" | Melody Playing: ");
    Serial.print(isMelodyPlaying ? "Yes" : "No");
    Serial.print(" | Timer Start: ");
    Serial.println(timerStartTime != 0 ? "Active" : "Inactive");
}

#endif
