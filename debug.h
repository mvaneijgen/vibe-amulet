#ifndef DEBUG_H
#define DEBUG_H

#include <Arduino.h>

unsigned long lastDebugUpdate = 0;
const unsigned long DEBUG_INTERVAL = 2000;

String getFormattedRunningTime() {
  unsigned long currentMillis = millis();
  unsigned long totalSeconds = currentMillis / 1000;
  unsigned long minutes = totalSeconds / 60;
  unsigned long seconds = totalSeconds % 60;

  char timeStr[6];
  sprintf(timeStr, "%02lu:%02lu", minutes, seconds);
  return String(timeStr);
}

void printDebugInfo(int buttonState, bool isMelodyPlaying,
                    unsigned long timerStartTime) {
  extern bool systemReset; // Reference the systemReset variable from main.ino
  extern bool buttonHeld;  // Reference the buttonHeld variable from main.ino
  extern unsigned long
      buttonPressStart; // Reference the buttonPressStart variable from main.ino

  unsigned long currentMillis = millis();
  if (currentMillis - lastDebugUpdate >= DEBUG_INTERVAL) {
    lastDebugUpdate = currentMillis;

    Serial.print(getFormattedRunningTime());
    Serial.print(" | Button: ");
    Serial.print(
        buttonState == LOW
            ? "Pressed"
            : "Released"); // LOW = pressed (closed), HIGH = released (open)
    Serial.print(" | Melody: ");
    Serial.print(isMelodyPlaying ? "Playing" : "Stopped");
    Serial.print(" | Timer: ");
    Serial.print(timerStartTime != 0 ? "Active" : "Inactive");
    Serial.print(" | Reset: ");
    Serial.print(systemReset ? "Yes" : "No");

    // Show button hold progress
    if (buttonHeld && isMelodyPlaying) {
      unsigned long holdTime = currentMillis - buttonPressStart;
      Serial.print(" | Hold: ");
      Serial.print(holdTime);
      Serial.print("/");
      Serial.print(3000); // buttonHoldTime
    }

    Serial.println();
  }
}

#endif
