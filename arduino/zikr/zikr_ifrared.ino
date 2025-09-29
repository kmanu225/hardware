#include <EEPROM.h>
#include <IRremote.h>
#include <LiquidCrystal_I2C.h>

// EEPROM & counter setup
int counterAddr = 0x0;
const unsigned long initCounterValue = 0;
const unsigned long MAX_COUNTER = 99999;
const unsigned long MIN_COUNTER = 0;
unsigned long currentCounterValue;

// LCD setup
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Infra Red setup
const int RECV_PIN = 9;
unsigned long lastCode = 0;

void setup() {
  // Load counter from EEPROM
  EEPROM.get(counterAddr, currentCounterValue);

  lcd.init();
  lcd.backlight();

  // Initialize EEPROM if value is out of range
  if (currentCounterValue < MIN_COUNTER || currentCounterValue > MAX_COUNTER) {
    currentCounterValue = initCounterValue;
    EEPROM.put(counterAddr, currentCounterValue);
  }

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
  sprintf(minBuf, "MIN %05lu", MIN_COUNTER);
  lcd.print(minBuf);

  lcd.setCursor(0, 1);
  char counterBuf[6];
  sprintf(counterBuf, "%05lu", currentCounterValue);
  lcd.print(counterBuf);

  lcd.setCursor(7, 1);
  lcd.print("MAX ");
  lcd.print(MAX_COUNTER);
}

void getCounter() {
  EEPROM.get(counterAddr, currentCounterValue);

  lcd.setCursor(0, 1);
  char buffer[6];
  sprintf(buffer, "%05lu", currentCounterValue);
  lcd.print(buffer);
}

void incrementCounter() {

  if (currentCounterValue < MAX_COUNTER) {
    currentCounterValue++;
    EEPROM.put(counterAddr, currentCounterValue);
  }
  getCounter();
}

void decrementCounter() {
  if (currentCounterValue > MIN_COUNTER) {
    currentCounterValue--;
    EEPROM.put(counterAddr, currentCounterValue);
  }
  getCounter();
}


void resetCounter() {
  if (currentCounterValue != MIN_COUNTER) {
    currentCounterValue = initCounterValue;
    EEPROM.put(counterAddr, currentCounterValue);
  }
  getCounter();
}

void loop() {

  if (IrReceiver.decode()) {
    unsigned long code;

    if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
      code = lastCode;
    } else {
      code = IrReceiver.decodedIRData.command;
      lastCode = code;
      switch (lastCode) {
        case 0x46:  // Increment
          incrementCounter();
          break;

        case 0x15:  // Decrement
          decrementCounter();
          break;

        case 0x45:  // Reset
          resetCounter();
          break;
      }
    }
    IrReceiver.resume();
  }
}
