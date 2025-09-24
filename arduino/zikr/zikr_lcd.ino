#include <EEPROM.h>
#include <LiquidCrystal.h>

int counterAddr = 0x0;
int initCounterValue = 0;
int MAX_COUNTER = 99999;
int MIN_COUNTER = 0;
int currentCounterValue;

int buttonApin = 11;
int buttonBpin = 12;

const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  EEPROM.get(counterAddr, currentCounterValue);
  lcd.begin(16, 2);

  // Initialize EEPROM if value is out of range
  if (currentCounterValue < MIN_COUNTER || currentCounterValue > MAX_COUNTER) {
    currentCounterValue = initCounterValue;
    EEPROM.put(counterAddr, currentCounterValue);
  }

  // Display Counter at start up
  getCounter();

  pinMode(buttonApin, INPUT_PULLUP);
  pinMode(buttonBpin, INPUT_PULLUP);
}

void getCounter() {
  EEPROM.get(counterAddr, currentCounterValue);
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print(currentCounterValue);
}


void incrementCounter() {
  currentCounterValue++;
  if (currentCounterValue > MAX_COUNTER) currentCounterValue = MIN_COUNTER;
  EEPROM.put(counterAddr, currentCounterValue);
  getCounter();
}

void resetCounter() {
  currentCounterValue = initCounterValue;
  EEPROM.put(counterAddr, currentCounterValue);
  lcd.clear(); 
  getCounter();
}

void loop() {
  if (digitalRead(buttonApin) == LOW) {
    incrementCounter();
    delay(300);
  }

  if (digitalRead(buttonBpin) == LOW) {
    resetCounter();
    delay(300);
  }
}
