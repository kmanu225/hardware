#include <EEPROM.h>
#include <LiquidCrystal.h>

// EEPROM & counter setup
int counterAddr = 0x0;
int initCounterValue = 0;
int MAX_COUNTER = 99999;
int MIN_COUNTER = 0;
int currentCounterValue;

// Button pins
int buttonApin = 11;
int buttonBpin = 12;

// LCD setup
const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Debounce setup
unsigned long lastPressA = 0;
unsigned long lastPressB = 0;
const unsigned long debounceTime = 50; // ms

void setup() {
  EEPROM.get(counterAddr, currentCounterValue);
  lcd.begin(16, 2);

  // Initialize EEPROM if value is out of range
  if (currentCounterValue < MIN_COUNTER || currentCounterValue > MAX_COUNTER) {
    currentCounterValue = initCounterValue;
    EEPROM.put(counterAddr, currentCounterValue);
  }

  pinMode(buttonApin, INPUT_PULLUP);
  pinMode(buttonBpin, INPUT_PULLUP);

  // Initial display
  display_screen();
}

void display_screen() {
  // --- First line: Zikr + MIN ---
  lcd.setCursor(0, 0);
  lcd.print("Zikr");

  lcd.setCursor(7, 0);  // position MIN
  char minBuf[12];
  sprintf(minBuf, "MIN %05d", MIN_COUNTER);
  lcd.print(minBuf);

  // --- Second line: Counter + MAX ---
  lcd.setCursor(0, 1);
  char counterBuf[6];
  sprintf(counterBuf, "%05d", currentCounterValue);
  lcd.print(counterBuf);

  lcd.setCursor(7, 1);  // position MAX
  char maxBuf[12];
  sprintf(maxBuf, "MAX %05d", MAX_COUNTER);
  lcd.print(maxBuf);
}

void getCounter() {
  EEPROM.get(counterAddr, currentCounterValue);

  // Print counter with leading zeros (always 5 digits)
  lcd.setCursor(0, 1);
  char buffer[6];
  sprintf(buffer, "%05d", currentCounterValue);
  lcd.print(buffer);

  // Blink cursor after counter
  lcd.setCursor(4, 1);
  lcd.blink();
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
  getCounter();
}

void loop() {
  unsigned long now = millis();

  // Button A (increment)
  if (digitalRead(buttonApin) == LOW && now - lastPressA > debounceTime) {
    lastPressA = now;
    incrementCounter();
  }

  // Button B (reset)
  if (digitalRead(buttonBpin) == LOW && now - lastPressB > debounceTime) {
    lastPressB = now;
    resetCounter();
  }
}
