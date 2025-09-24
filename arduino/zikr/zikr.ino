#include <EEPROM.h>

int counterAddr = 0x0;
int initCounterValue = 0;
int MAX_COUNTER = 99999;
int MIN_COUNTER = 0;
int currentCounterValue;

int buttonApin = 8;
int buttonBpin = 9;

void setup() {
  Serial.begin(9600);

  EEPROM.get(counterAddr, currentCounterValue);

  // Initialize EEPROM if value is out of range
  if (currentCounterValue < MIN_COUNTER || currentCounterValue > MAX_COUNTER) {
    currentCounterValue = initCounterValue;
    EEPROM.put(counterAddr, currentCounterValue);
  }

  pinMode(buttonApin, INPUT_PULLUP);
  pinMode(buttonBpin, INPUT_PULLUP);
}

void getCounter() {
  EEPROM.get(counterAddr, currentCounterValue);
  Serial.println(currentCounterValue);
}

void incrementCounter() {
  currentCounterValue++;
  if (currentCounterValue > MAX_COUNTER) currentCounterValue = MIN_COUNTER;
  EEPROM.put(counterAddr, currentCounterValue);
}

void resetCounter() {
  currentCounterValue = initCounterValue;
  EEPROM.put(counterAddr, currentCounterValue);
}

void loop() {
  if (digitalRead(buttonApin) == LOW) {
    incrementCounter();
    getCounter();
    delay(300); // simple debounce
  }

  if (digitalRead(buttonBpin) == LOW) {
    resetCounter();
    getCounter();
    delay(300); // simple debounce
  }
}
