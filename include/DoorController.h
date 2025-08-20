#ifndef DOORCONTROLLER_H
#define DOORCONTROLLER_H

#include <Arduino.h>

enum DoorState
{
    STATE_IDLE,
    STATE_GOING_UP,
    STATE_DOOR_IS_UP,
    STATE_GOING_DOWN,
    STATE_GOING_UP_EMERGENCY
};

class DoorController
{
public:
    DoorController(uint8_t upBtn, uint8_t downBtn, uint8_t rfidBtn, uint8_t fireSignal,
                   uint8_t relayUp, uint8_t relayDown);

    void begin();
    void update();

private:
    // Pins
    uint8_t upBtnPin;
    uint8_t downBtnPin;
    uint8_t rfidBtnPin;
    uint8_t fireSignalPin;
    uint8_t relayUpPin;
    uint8_t relayDownPin;

    // State
    DoorState currentState;
    unsigned long stateStartTime;
    unsigned long doorUpDuration;
    unsigned long baseTime;

    bool doneLifting;

    // Helpers
    bool readButton(uint8_t pin);
    void setRelay(bool up, bool down);

    unsigned long getTimeFromDIP(unsigned long baseTime);
    unsigned long getTimeBaseFromDIP(unsigned long baseTime);
};

#endif
