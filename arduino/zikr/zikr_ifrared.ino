#include <EEPROM.h>
#include <IRremote.h>
#include <LiquidCrystal_I2C.h>

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
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Debounce setup
unsigned long lastPressA = 0;
unsigned long lastPressB = 0;
const unsigned long debounceTime = 50;  // ms

// Infra Red setup
const int RECV_PIN = 9;
unsigned long lastCode = 0;

void setup() {
  Serial.begin(9600);

  // Load counter from EEPROM
  EEPROM.get(counterAddr, currentCounterValue);

  lcd.init();
  lcd.backlight();

  // Initialize EEPROM if value is out of range
  if (currentCounterValue < MIN_COUNTER || currentCounterValue > MAX_COUNTER) {
    currentCounterValue = initCounterValue;
    EEPROM.put(counterAddr, currentCounterValue);
  }

  pinMode(buttonApin, INPUT_PULLUP);
  pinMode(buttonBpin, INPUT_PULLUP);

  // Start IR receiver
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);

  // Initial display
  display_screen();
}

void display_screen() {
  lcd.setCursor(0, 0);
  lcd.print("Zikr");

  lcd.setCursor(7, 0);
  char minBuf[12];
  sprintf(minBuf, "MIN %05d", MIN_COUNTER);
  lcd.print(minBuf);

  lcd.setCursor(0, 1);
  char counterBuf[6];
  sprintf(counterBuf, "%05d", currentCounterValue);
  lcd.print(counterBuf);

  lcd.setCursor(7, 1);
  char maxBuf[12];
  sprintf(maxBuf, "MAX %05d", MAX_COUNTER);
  lcd.print(maxBuf);
}

void getCounter() {
  EEPROM.get(counterAddr, currentCounterValue);

  lcd.setCursor(0, 1);
  char buffer[6];
  sprintf(buffer, "%05d", currentCounterValue);
  lcd.print(buffer);

  lcd.setCursor(4, 1);
  lcd.blink();
}

void incrementCounter() {
  currentCounterValue++;
  if (currentCounterValue > MAX_COUNTER) currentCounterValue = MAX_COUNTER;
  EEPROM.put(counterAddr, currentCounterValue);
  getCounter();
}

void decrementCounter() {
  currentCounterValue--;
  if (currentCounterValue < MIN_COUNTER) currentCounterValue = MIN_COUNTER;
  EEPROM.put(counterAddr, currentCounterValue);
  getCounter();
}

void resetCounter() {
  currentCounterValue = initCounterValue;
  EEPROM.put(counterAddr, currentCounterValue);
  getCounter();
}

void loop() {

  if (IrReceiver.decode()) {
    unsigned long code = IrReceiver.decodedIRData.decodedRawData;

    if (code == 0) {
      code = lastCode;  // Repeat last code
    } else {
      lastCode = code;
      switch (lastCode) {
        case 0xB946FF00:  // Increment
          incrementCounter();
          break;

        case 0xEA15FF00:  // Decrement
          decrementCounter();
          break;

        case 0xBA45FF00:  // Reset
          resetCounter();
          break;
      }
    }
    IrReceiver.resume();
  }
}
