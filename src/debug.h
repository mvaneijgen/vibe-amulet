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

void printDebugInfo(int buttonState, bool isMelodyPlaying, unsigned long timerStartTime) {
  extern bool startup;  // Reference the startup variable from main.ino

  unsigned long currentMillis = millis();
  if (currentMillis - lastDebugUpdate >= DEBUG_INTERVAL) {
    lastDebugUpdate = currentMillis;

    Serial.print(getFormattedRunningTime());
    Serial.print(" | Button: ");
    Serial.print(buttonState == LOW ? "Pressed" : "Released");
    Serial.print(" | Melody: ");
    Serial.print(isMelodyPlaying ? "Playing" : "Stopped");
    Serial.print(" | Timer: ");
    Serial.println(timerStartTime != 0 ? "Active" : "Inactive");
  }
}

#endif
