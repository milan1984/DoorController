
#ifndef IO_LIST_H
#define IO_LIST_H

//#define SIMULATION

#ifdef SIMULATION

// Pin definitions
#define PIN_UP_BTN 2
#define PIN_DOWN_BTN 3
#define PIN_RFID_BTN 4
#define PIN_FIRE_SIGNAL 5
#define PIN_RELAY_UP 6
#define PIN_RELAY_DOWN 7
#define PIN_STATUS_LED 13
#define ERROR_STATUS_LED 11

// Define DIP switch pins
#define DIP1 8
#define DIP2 9
#define DIP3 10

#else

// Pin definitions
#define PIN_UP_BTN PA1
#define PIN_DOWN_BTN PA0
#define PIN_RFID_BTN PB9
#define PIN_FIRE_SIGNAL PB9
#define PIN_RELAY_UP PA5
#define PIN_RELAY_DOWN PB12
#define PIN_STATUS_LED PB15
#define ERROR_STATUS_LED PB14

// Define DIP switch pins
#define DIP1 PC13
#define DIP2 PC14
#define DIP3 PC15

#endif

#endif // IO_LIST_H