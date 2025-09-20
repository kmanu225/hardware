# Zikr

![Example](ressources/example.png)

This simple Arduino project implements a digital *zikr* counter.
It currently provides the following features:

* Increment the counter
* Reset the counter
* Save the counter state in EEPROM memory (persistent across resets of the Arduino board)

## Arduino Board Connections

![Circuit Diagram](ressources/electronic_crcuit.png)

## Current Status

At this stage, the counter value is displayed through the Serial Monitor at **9600 baud**.

## Next Steps

The next step will be to integrate a screen to display the counter value directly on the device.

## References

* [Tinkercad Circuits](https://www.tinkercad.com/dashboard/designs/circuits)
* [Arduino EEPROM Guide](https://docs.arduino.cc/learn/programming/eeprom-guide/)
