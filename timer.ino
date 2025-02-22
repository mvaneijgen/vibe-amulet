#include "melody/tetris.h"
#include "melody/mario.h"
#include "melody/starwars.h"

const int buttonPin = 0;  // GPIO 0 (BOOT button on most ESP32 boards)
const int vibrationPin = 13; // Set connection pin to signal pin of vibration module

#include "melody/pitches.h" // Include pitches.h in the main script

// Array of melodies
const int* melodies[] = {
  tetris::melody,
  mario::melody,
  starwars::melody
};

const int tempos[] = {
  tetris::tempo,
  mario::tempo,
  starwars::tempo
};

const int notes[] = {
  sizeof(tetris::melody) / sizeof(tetris::melody[0]) / 2,
  sizeof(mario::melody) / sizeof(mario::melody[0]) / 2,
  sizeof(starwars::melody) / sizeof(starwars::melody[0]) / 2
};

// Variables for state management
bool isMelodyPlaying = false; // Tracks if the melody is currently playing
bool buttonPressed = false;   // Tracks if the button is pressed
unsigned long timerStartTime = 0; // Tracks when the timer starts
const unsigned long oneMinute = 60000; // One minute in milliseconds

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Set the button pin as input
  pinMode(buttonPin, INPUT);
  pinMode(vibrationPin, OUTPUT); // Set pin as output
  digitalWrite(vibrationPin, LOW); // Set pin low to make sure vibration motor is off
}

void playMelody(const int melody[], int notes, int tempo) {
  int wholenote = (60000 * 4) / tempo;
  int divider = 0, noteDuration = 0;

  for (int thisNote = 0; thisNote < notes * 2; thisNote += 2) {
    divider = melody[thisNote + 1];
    if (divider > 0) {
      noteDuration = wholenote / divider;
    } else if (divider < 0) {
      noteDuration = wholenote / abs(divider);
      noteDuration *= 1.5;
    }

    // Vibrate the motor for notes (pulses)
    if (melody[thisNote] != REST) {
      digitalWrite(vibrationPin, HIGH);
    }
    delay(noteDuration * 0.9); // Play the note for 90% of the duration
    digitalWrite(vibrationPin, LOW); // Turn off the motor
    delay(noteDuration * 0.1); // Pause for 10% of the duration
  }
}

void loop() {
  // Read the button state
  int buttonState = digitalRead(buttonPin);

  // Check if the button is pressed (LOW because the button is active-low)
  if (buttonState == LOW && !buttonPressed) {
    buttonPressed = true; // Mark button as pressed

    if (!isMelodyPlaying) {
      // Give a small pulse to the motor
      digitalWrite(vibrationPin, HIGH);
      delay(100); // Small pulse duration
      digitalWrite(vibrationPin, LOW);

      // Start the timer for one minute only if the melody is not already playing
      if (timerStartTime == 0) {
        timerStartTime = millis();
        Serial.println("Timer started. Melody will play in one minute.");
      }
    } else {
      // Stop the melody and reset the state
      isMelodyPlaying = false;
      timerStartTime = 0; // Reset the timer
      Serial.println("Melody stopped.");
    }
  }

  // Reset button state when released
  if (buttonState == HIGH && buttonPressed) {
    buttonPressed = false;
  }

  // Check if one minute has passed and the melody is not already playing
  if (!isMelodyPlaying && timerStartTime != 0 && millis() - timerStartTime >= oneMinute) {
    isMelodyPlaying = true; // Start playing the melody
    timerStartTime = 0; // Reset the timer

    // Randomly select a melody
    int melodyIndex = 1; // 0 = Tetris, 1 = Mario, 2 = Star Wars
    Serial.println("One minute passed. Playing melody...");

    // Play the selected melody
    playMelody(melodies[melodyIndex], notes[melodyIndex], tempos[melodyIndex]);

    // Reset the melody state after playing
    isMelodyPlaying = false;
  }

  // Add a small delay to avoid bouncing
  delay(10);
}