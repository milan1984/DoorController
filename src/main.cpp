#include "DoorController.h"
#include "IO_List.h"

DoorController door(PIN_UP_BTN, PIN_DOWN_BTN, PIN_RFID_BTN, PIN_FIRE_SIGNAL,
                    PIN_RELAY_UP, PIN_RELAY_DOWN);

void setup()
{
    door.begin();
}

void loop()
{
    door.update();
}
