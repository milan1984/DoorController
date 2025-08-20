#ifndef CONFIG_H
#define CONFIG_H

#define DEFAULT_BASE_TIME 1000 // Basic time in ms for fully raising the door (determined later using DIP switches)

#define STATUS_LED_BLINK_INTERVAL 300 // Time in ms

#define ERROR_LED_BLINK_INTERVAL 100 // Time in ms



//Debouce times
#define BUTTON_UP_DEBOUNCE_TIME 200 // Time in ms
#define BUTTON_DWN_DEBOUNCE_TIME 200 // Time in ms
#define RFID_DEBOUNCE_TIME 200 // Time in ms
#define FIRE_ALERT_SIGNAL_DEBOUNCE_TIME 200 // Time in ms

#endif // CONFIG_H
