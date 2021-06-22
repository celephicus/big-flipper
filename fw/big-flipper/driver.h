#ifndef DRIVER_H__
#define DRIVER_H__

// Always need this.
#include "gpio.h"

// Call first off to initialise the driver. 
void driverInit();

// Call in mainloop to perform services.
void driverService();

// The scanner controls various flags in the flags register. This function causes the events associated with a flags mask to be rescanned, and the event to be resent if the value is above/below a threshold. 
void driverRescan(uint16_t mask);

// Action timers -- do an action on arm, do another action on timeout. Rearming extends the current timeout by the delay given. Used only for timed outputs.
//
enum {
    DRIVER_ACTION_TIMER_IDX_DEBUG_LED,
};

// Exposed as we might change register values, but we still have to send these to the display driver.
void driverUpdateDisplaySettings();

#if defined(CFG_WANT_DISPLAY_14_SEGMENT_LED)

#include "ht16k33_display.h"
HT16K33Display& driverGetDisplay();

#define DRIVER_CONSOLE_COMMANDS_2																										\
	case /** DISP-MODE **/ 0xd645: driverGetDisplay().setDisplayMode(u_pop()); break; 													\
	case /** DISP-BRIGHT **/ 0xc884: driverGetDisplay().setBrightness(u_pop()); break;

#elif defined(CFG_WANT_DISPLAY_FLIPDOT)
#include "flipdot_display.h"
FlipdotDisplay& driverGetDisplay();

#define DRIVER_CONSOLE_COMMANDS_2																										\
	case /** FLIP **/ 0x2836: 																											\
	{ const bool hl = !!u_pop(); const uint8_t row = u_pop(); driverGetDisplay().doFlip(u_pop(), row, hl); } break; 

#else
# error Need CFG_WANT_DISPLAY_xxx
#endif

// Commands to access driver from console. 
#define DRIVER_CONSOLE_COMMANDS_1																										\
	case /** DISP **/ 0x1e2b: driverGetDisplay().write((const char*)u_pop()); break; 													\
	case /** DISP-B **/ 0x5824: { const uint8_t pos = u_pop(); driverGetDisplay().writeBitmap((const uint8_t*)u_pop(), pos); } break; 	\
	case /** DISP-C **/ 0x5825: { const uint8_t end = u_pop(); driverGetDisplay().clear(u_pop(), end); } break; 						\

#define DRIVER_CONSOLE_COMMANDS			\
 DRIVER_CONSOLE_COMMANDS_1		\
 DRIVER_CONSOLE_COMMANDS_2
 
 
#endif // DRIVER_H__
