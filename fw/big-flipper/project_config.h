// project-config.h -- Configuration file for shared libraries.

#ifndef PROJECT_CONFIG_H__
#define PROJECT_CONFIG_H__

// Output to 14 segment display or Flipdots.
#define CFG_WANT_DISPLAY_FLIPDOT
//#define CFG_WANT_DISPLAY_14_SEGMENT_LED

// Console speed.
#define CFG_CONSOLE_BAUDRATE 115200

// For lc2.h
#define CFG_LC2_USE_SWITCH 0

// Watchdog. 
#define CFG_WATCHDOG_TIMEOUT WDTO_2S   
#define CFG_WATCHDOG_ENABLE 0
#define CFG_WATCHDOG_MODULE_COUNT 0

// Console speed.
#define CFG_CONSOLE_BAUDRATE 115200
#define CFG_WANT_CONSOLE_ECHO 1
#define CFG_CONSOLE_INPUT_ACCEPT_BUFFER_SIZE (32)

/*

// Extra help text for console.
#define CFG_WANT_VERBOSE_CONSOLE 0

// Which sort of printf for serial port. 
#define CFG_SERIAL_PRINTF_USING_MYPRINTF

// Console speed.
#define CFG_WANT_CONSOLE_ECHO 1
#define CFG_CONSOLE_INPUT_ACCEPT_BUFFER_SIZE (32)
*/
#endif  // PROJECT_CONFIG_H__

