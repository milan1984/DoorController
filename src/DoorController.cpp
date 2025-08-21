#include "DoorController.h"
#include "config.h"
#include "Edge.h"
#include "LEDBlinker.h"
#include "IO_List.h"

// Serial setup for AVR or STM32
#if defined(__AVR__)
#define DEBUG_SERIAL Serial
#elif defined(STM32F1xx) || defined(STM32F4xx)
// Create a HardwareSerial instance for USART2
HardwareSerial debugSerial(USART2);
#define DEBUG_SERIAL debugSerial
#else
#define DEBUG_SERIAL Serial
#endif

#ifdef SIMULATION
#define INPUT_TYPE INPUT_PULLUP
#else
#define INPUT_TYPE INPUT
#endif

LEDBlinker statusLed(PIN_STATUS_LED);
LEDBlinker errorLed(ERROR_STATUS_LED);

Edge e1;
Edge e2;
Edge e3;
Edge e4;

DoorController::DoorController(uint8_t upBtn, uint8_t downBtn, uint8_t rfidBtn, uint8_t fireSignal,
                               uint8_t relayUp, uint8_t relayDown)
    : upBtnPin(upBtn), downBtnPin(downBtn), rfidBtnPin(rfidBtn), fireSignalPin(fireSignal),
      relayUpPin(relayUp), relayDownPin(relayDown), baseTime(DEFAULT_BASE_TIME), doorUpDuration(baseTime),
      currentState(STATE_IDLE), stateStartTime(0) {}

void DoorController::begin()
{
    // Initialize serial
    DEBUG_SERIAL.begin(115200);
    pinMode(upBtnPin, INPUT_TYPE);
    pinMode(downBtnPin, INPUT_TYPE);
    pinMode(rfidBtnPin, INPUT_TYPE);
    pinMode(fireSignalPin, INPUT_TYPE);
    pinMode(DIP1, INPUT_TYPE);
    pinMode(DIP2, INPUT_TYPE);
    pinMode(DIP3, INPUT_TYPE);
    
    pinMode(relayUpPin, OUTPUT);
    pinMode(relayDownPin, OUTPUT);

    setRelay(false, false);

    baseTime = getTimeBaseFromDIP(DEFAULT_BASE_TIME);
    doorUpDuration = getTimeFromDIP(baseTime);

    DEBUG_SERIAL.print("DIP sw BASE value selected -> ");
    DEBUG_SERIAL.println(baseTime);
    DEBUG_SERIAL.print("DIP sw TIME value selected -> ");
    DEBUG_SERIAL.println(doorUpDuration);
}

void DoorController::update()
{
    unsigned long now = millis();

    bool upPressed = readButton(upBtnPin);
    bool downPressed = readButton(downBtnPin);
    bool rfidPressed = readButton(rfidBtnPin);
    bool fireActive = readButton(fireSignalPin);

    bool upEdge = e1.rTrigDebounce(upPressed, BUTTON_UP_DEBOUNCE_TIME);
    bool downEdge = e2.rTrigDebounce(downPressed, BUTTON_DWN_DEBOUNCE_TIME);
    bool rfidEdge = e3.rTrigDebounce(rfidPressed, RFID_DEBOUNCE_TIME);
    bool fireEdge = e4.rTrigDebounce(fireActive, FIRE_ALERT_SIGNAL_DEBOUNCE_TIME);

    switch (currentState)
    {
    case STATE_IDLE:
        setRelay(false, false);

        if (fireEdge)
        {
            currentState = STATE_GOING_UP_EMERGENCY;
            stateStartTime = now;
            setRelay(true, false);
        }
        else if (upEdge || rfidEdge)
        {
            currentState = STATE_GOING_UP;
            stateStartTime = now;
            setRelay(true, false);
        }
        else if (downPressed && !fireActive)
        {
            currentState = STATE_GOING_DOWN;
            setRelay(false, true);
        }
        break;

    case STATE_GOING_UP:
        if (downEdge)
        {
            currentState = STATE_IDLE;
            setRelay(false, false);
        }
        else if (now - stateStartTime >= doorUpDuration)
        {
            currentState = STATE_DOOR_IS_UP;
            setRelay(false, false);
        }
        break;

    case STATE_DOOR_IS_UP:
        setRelay(false, false);
        if (downPressed)
        {
            currentState = STATE_GOING_DOWN;
            setRelay(false, true);
        }
        break;

    case STATE_GOING_DOWN:
        if (!downPressed || fireActive)
        {
            currentState = STATE_IDLE;
            setRelay(false, false);
        }
        break;

    case STATE_GOING_UP_EMERGENCY:
        if (now - stateStartTime >= doorUpDuration)
        {
            setRelay(false, false);
            doneLifting = true;
        }
        if (doneLifting && !downPressed && !upPressed && !rfidPressed && !fireActive)
        {
            currentState = STATE_IDLE;
            doneLifting = false;
        }
        break;
    }

    // LED that indicates that sotware is running (always blink)
    statusLed.blink(true, 1, STATUS_LED_BLINK_INTERVAL, 0);

    // Led that idicates fireActive input
    errorLed.blink(fireActive, 1, ERROR_LED_BLINK_INTERVAL, 0);

    DEBUG_SERIAL.println(currentState);
}

bool DoorController::readButton(uint8_t pin)
{
    return !digitalRead(pin);
}

void DoorController::setRelay(bool up, bool down)
{
    digitalWrite(relayUpPin, up ? HIGH : LOW);
    digitalWrite(relayDownPin, down ? HIGH : LOW);
}

unsigned long DoorController::getTimeFromDIP(unsigned long baseTime)
{
    // Read DIP switches (ON = LOW, OFF = HIGH)
    uint8_t dipValue = 0;
    dipValue |= (digitalRead(DIP2) == LOW ? 1 : 0) << 0;
    dipValue |= (digitalRead(DIP3) == LOW ? 1 : 0) << 1;

    // Calculate time based on DIP combination
    return baseTime * (1 + dipValue);
}

unsigned long DoorController::getTimeBaseFromDIP(unsigned long baseTime)
{
    // Read DIP1 switch (ON = LOW, OFF = HIGH)
    uint8_t dipValue = (digitalRead(DIP1) == LOW) ? 1 : 0;

    // Return baseTime if DIP is 0, else baseTime * 10
    if (dipValue == 0)
    {
        return baseTime;
    }
    else
    {
        return baseTime * 10;
    }
}
