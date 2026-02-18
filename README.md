# LED-Binary-Counter
Public Files for the LED Binary Counter.  Modify, Code, Change, on your own.

There are two versions.  The original one had a MSP430 G2553 as it's controller.  The new version uses an ATSAMD21E15B with added usb utility
Both chips have 10 LEDs that they control.

ATSAMD21E15B Documentation---------------------------------------------------------------------------


Pinout
PA00 - LED 0
PA01 - LED 1
PA02 - LED 2
PA03 - LED 3
PA04 - LED 4
PA05 - LED 5
PA06 - LED 6
PA07 - LED 7
PA08 - LED 8
PA09 - LED 9
PA16 - PushButton



MSP430 Documentation---------------------------------------------------------------------------------


This respository contains all of the files used to create the Montana Tech LED Bar Graph Counter
The bar graph counter is a piece of SWAG intended for potential students.
There are 10 LEDs controlled by an MSP430 G2553 Microcontroller (TI Look it up)

A PCB was created for use with the QFN-32 package of the G2553
Code was created using code composer studio in C programming language. All files are available
More instructions can be found for new users via the web page:
https://www.mtech.edu/electrical-engineering/circuit-handout/

Potential students can request a free complete system
Pinout of the MSP430 are the following
* 1.0 -
 * 1.1 - RX0  UART Receive
 * 1.2 - TX0  UART Transmit
 * 1.3 - SW1  Pushbutton
 * 1.4 -
 * 1.5 -
 * 1.6 -
 * 1.7 -
 * 2.0 - L8  LED 8
 * 2.1 - L9  LED 9
 * 2.2 -
 * 2.3 -
 * 2.4 -
 * 2.5 -
 * 2.6 -
 * 2.7 -
 * 3.0 - L0  LED 0
 * 3.1 - L1  LED 1
 * 3.2 - L2  LED 2
 * 3.3 - L3  LED 3
 * 3.4 - L4  LED 4
 * 3.5 - L5  LED 5
 * 3.6 - L6  LED 6
 * 3.7 - L7  LED 7
