#ifndef DEBUG_H
#define DEBUG_H

#include <Arduino.h>

unsigned long lastDebugUpdate = 0;
const unsigned long DEBUG_INTERVAL = 2000; // 2 seconds between updates

String getFormattedTime() {
    // Calculate current time based on millis() and initial time
    unsigned long currentMillis = millis();
    // Start from 11:43 UTC+1
    unsigned long initialHours = 11;
    unsigned long initialMinutes = 43;
    
    unsigned long totalMinutes = (currentMillis / 60000) + initialMinutes; // Convert millis to minutes and add initial minutes
    unsigned long hours = (totalMinutes / 60 + initialHours) % 24; // Add initial hours and wrap around 24
    unsigned long minutes = totalMinutes % 60;
    
    char timeStr[9];
    sprintf(timeStr, "%02lu:%02lu", hours, minutes);
    return String(timeStr);
}

void printDebugInfo(int buttonState, bool isMelodyPlaying, unsigned long timerStartTime) {
    extern bool startup;  // Reference the startup variable from main.ino
    
    unsigned long currentMillis = millis();
    if (currentMillis - lastDebugUpdate >= DEBUG_INTERVAL) {
        lastDebugUpdate = currentMillis;
        
        Serial.print("[");
        Serial.print(getFormattedTime());
        Serial.print("] ");
        Serial.print(" | Button: ");
        Serial.print(buttonState == LOW ? "Pressed" : "Released");
        Serial.print(" | Melody: ");
        Serial.print(isMelodyPlaying ? "Playing" : "Stopped");
        Serial.print(" | Timer: ");
        Serial.println(timerStartTime != 0 ? "Active" : "Inactive");
    }
}

#endif
