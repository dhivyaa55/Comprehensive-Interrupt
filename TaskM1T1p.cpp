#include <avr/interrupt.h>
#include <Arduino.h>

// Pins
constexpr uint8_t LED_A = 6;
constexpr uint8_t LED_B = 7;
constexpr uint8_t BTN1  = 8;   // PCINT0
constexpr uint8_t BTN2  = 9;   // PCINT1
constexpr uint8_t BTN3  = 10;  // PCINT2

// Debounce
constexpr uint16_t DEBOUNCE_MS = 30;
unsigned long lastPressMs[3] = {0,0,0};

// ISR-shared
volatile uint8_t last_pb = 0;          // PB0..PB2 snapshot
volatile uint8_t pendingPressMask = 0; // bit0=BTN1, bit1=BTN2, bit2=BTN3

volatile bool tick = false;

void setup() {
  pinMode(LED_A, OUTPUT);
  pinMode(LED_B, OUTPUT);

  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(BTN3, INPUT_PULLUP);

  Serial.begin(9600);
  Serial.println(F("PCINT D8..D10, Timer1 1Hz"));

  // PCINT for PORTB (D8..D13)
  last_pb = PINB & 0b00000111;
  PCICR  |= (1 << PCIE0);
  PCMSK0 |= (1 << PCINT0) | (1 << PCINT1) | (1 << PCINT2);

  // Timer1 CTC 1 Hz
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  OCR1A  = 15624;                 // 16MHz/1024 - 1
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS12) | (1 << CS10);
  TIMSK1 |= (1 << OCIE1A);
  interrupts();
}

void loop() {
  noInterrupts();
  uint8_t mask = pendingPressMask;
  pendingPressMask = 0;
  interrupts();

  if (mask) {
    const unsigned long now = millis();

    if (mask & 0b001) {
      if (now - lastPressMs[0] > DEBOUNCE_MS) {
        lastPressMs[0] = now;
        Serial.println(F("BTN1: LED_B ON"));
        digitalWrite(LED_B, HIGH);
      }
    }
    if (mask & 0b010) {
      if (now - lastPressMs[1] > DEBOUNCE_MS) {
        lastPressMs[1] = now;
        Serial.println(F("BTN2: LED_B OFF"));
        digitalWrite(LED_B, LOW);
      }
    }
    if (mask & 0b100) {
      if (now - lastPressMs[2] > DEBOUNCE_MS) {
        lastPressMs[2] = now;
        Serial.println(F("BTN3: TOGGLE A+B"));
        digitalWrite(LED_A, !digitalRead(LED_A));
        digitalWrite(LED_B, !digitalRead(LED_B));
      }
    }
  }
}

ISR(PCINT0_vect) {
  uint8_t pb = PINB & 0b00000111;
  uint8_t changed = pb ^ last_pb;

  if ((changed & (1 << 0)) && !(pb & (1 << 0))) pendingPressMask |= 0b001;
  if ((changed & (1 << 1)) && !(pb & (1 << 1))) pendingPressMask |= 0b010;
  if ((changed & (1 << 2)) && !(pb & (1 << 2))) pendingPressMask |= 0b100;

  last_pb = pb;
}

ISR(TIMER1_COMPA_vect) {
  tick = !tick;
  digitalWrite(LED_A, tick);
  Serial.println(F("tick"));
}
