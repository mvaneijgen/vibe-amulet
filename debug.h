#ifndef DEBUG_H
#define DEBUG_H

#include <Arduino.h>

unsigned long lastDebugUpdate = 0;
const unsigned long DEBUG_INTERVAL = 2000; // 2 seconds between updates

String getFormattedTime() {
    unsigned long currentMillis = millis();
    unsigned long seconds = currentMillis / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
    
    char timeStr[9];
    sprintf(timeStr, "%02lu:%02lu", hours, minutes % 60);
    return String(timeStr);
}

void printDebugInfo(int buttonState, bool isMelodyPlaying, unsigned long timerStartTime) {
    unsigned long currentMillis = millis();
    if (currentMillis - lastDebugUpdate >= DEBUG_INTERVAL) {
        lastDebugUpdate = currentMillis;
        
        Serial.print("[");
        Serial.print(getFormattedTime());
        Serial.print("] ");
        Serial.print("Button: ");
        Serial.print(buttonState == LOW ? "Pressed" : "Released");
        Serial.print(" | Melody: ");
        Serial.print(isMelodyPlaying ? "Playing" : "Stopped");
        Serial.print(" | Timer: ");
        Serial.println(timerStartTime != 0 ? "Active" : "Inactive");
    }
}

#endif
