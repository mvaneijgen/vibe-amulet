# Goal
My partner has narcolepsy and thus through out the day falls asleep. If we're together and I notices I usually take note of the time and wake them up about 17 minutes later, but of course we aren't always together. So that made me think of this concept: make a small neckless that when pressed once, sets a timer for X amount of minutes an then gently wakes you.

# About
This is based on the TinyPico a small ESP32 board with a lot of features! All the components are a main board (TinyPico) small coin cell motor a battery, button and some lights. Everything is contained within a 3D printed case. When the amulet is activated it will let you know with a small pulse and lights will glow (if preferred), then a timer of the preferred amount of minutes will start counting down, near the end of the timer the motor will start pulsing gently. When nearing the timer it will increase its pulses ending in the selected melody looping until pressed again and stopping everything. Then the device will be ready again for the next power nap 💪

# Todo's

- [x] Test setup using a ESP32 and WHADA vibration motor module
- [x] Led pulsing on the ESP32 blue
- [x] Melodies logic
- [x] Disabling on button hold
- [x] Led pulsing on the ESP32 blue
- [ ] 3D model 
  - [ ] Back
  - [ ] Front
  - [ ] Custom desing
  - [ ] Test print 
- [ ] Some