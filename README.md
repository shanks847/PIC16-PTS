# PIC16-PTS
Note: This was a project for ECNG2006 at The UWI. St. Augutine Campus

A position tracking system that uses an HC-SR04 sensor and a 2-bit gray encoder to track the distance and orientatation of a vertical target. The project uses a 5V Axial BLDC Fan to blow across the HC-SR04 path. If the target on the encoder system is light enough, it may be rotated by the force of the fan. All circuit schematics as well as the breadboard layout is in the docs folder. The motor speed is PWM controlled by the 4-bit DIPSW. The micrcontroller used in this project is the PIC16F18877. The serial connection is configured for 1200 baud rate and the fan is driven via a 2n4401 transistor. The MCU uses a external  2MHz crystal oscillator.
