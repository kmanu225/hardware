#include <IRremote.h>

const int RECV_PIN_1 = 9;
unsigned long lastCode = 0;

void setup() {
  Serial.begin(9600);
  IrReceiver.begin(RECV_PIN_1, ENABLE_LED_FEEDBACK);
  Serial.println(F("\nIR Receiver ready"));
}

void loop() {
  if (IrReceiver.decode()) {
    unsigned long code;

    // Vérifie si c'est une répétition
    if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
      code = lastCode;
      Serial.println(F("Repeat signal"));
    } else {
      code = IrReceiver.decodedIRData.command;  // code du bouton
      lastCode = code;

      Serial.print(F("Protocol: "));
      Serial.print(IrReceiver.getProtocolString());
      Serial.print(F(" | Address: 0x"));
      Serial.print(IrReceiver.decodedIRData.address, HEX);
      Serial.print(F(" | Command: 0x"));
      Serial.print(code, HEX);
      Serial.print(F(" | Raw data: 0x"));
      Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);

    }

    IrReceiver.resume(); 
  }
}
