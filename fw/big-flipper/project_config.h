// project-config.h -- Configuration file for shared libraries.

#ifndef PROJECT_CONFIG_H__
#define PROJECT_CONFIG_H__

// Output to 14 segment display or Flipdots.
// #define CFG_WANT_DISPLAY_FLIPDOT
#define CFG_WANT_DISPLAY_14_SEGMENT_LED

// For lc2.h
#define CFG_LC2_USE_SWITCH 0

// Watchdog. 
#define CFG_WATCHDOG_TIMEOUT WDTO_2S   
#define CFG_WATCHDOG_ENABLE 1
#define CFG_WATCHDOG_MODULE_COUNT 0

// Console speed.
#define CFG_CONSOLE_BAUDRATE 115200
#define CFG_WANT_CONSOLE_ECHO 1
#define CFG_CONSOLE_INPUT_ACCEPT_BUFFER_SIZE (32)

/*
// runEvery()
typedef uint16_t RUN_EVERY_TIME_T;
#define RUN_EVERY_GET_TIME() ((RUN_EVERY_TIME_T)millis())


// Assign timers for SMs. Make sure that there is enough room in the timer array. See EVENT_TIMER_COUNT.
enum {
	CFG_TIMER_COUNT_SM_MAIN = 3,
	CFG_TIMER_COUNT_SM_LEDS = 1,
};
enum {
    CFG_TIMER_SM_MAIN = 0,
    CFG_TIMER_SM_LEDS = CFG_TIMER_COUNT_SM_MAIN,
};

// Extra help text for console.
#define CFG_WANT_VERBOSE_CONSOLE 0

// Which sort of printf for serial port. 
#define CFG_SERIAL_PRINTF_USING_MYPRINTF

// Console speed.
#define CFG_CONSOLE_BAUDRATE 115200
#define CFG_WANT_CONSOLE_ECHO 1
#define CFG_CONSOLE_INPUT_ACCEPT_BUFFER_SIZE (32)
*/
#endif  // PROJECT_CONFIG_H__

