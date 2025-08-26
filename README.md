# Comprehensive-Interrupt


## Summary
I used Timer1 interrupt on Arduino UNO to blink LED_A without delay(). Three debounced buttons control LEDs and speed. Serial shows: “Timer1 tick -> LED_A toggled”.

## Wiring
- LED_A: D6 → 220Ω → LED → GND
- LED_B: D7 → 220Ω → LED → GND
- BTN1/BTN2/BTN3: 5V → D8/D9/D10 with 10k→GND
(Or INPUT_PULLUP to GND.)

## Run
1. Upload (Board: Arduino Uno).
2. Open Serial Monitor @ 9600.
3. Press buttons to test.

Note: Tested on UNO R3. BTN3 needed debounce (~30 ms).

