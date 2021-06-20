#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#define FASTLED_USE_GLOBAL_BRIGHTNESS 0
#include <FastLED.h>

#include "project_config.h"
#include "debug.h"
FILENUM(1);

#include "utils.h"
#include "regs.h"
#include "console.h"
#include "driver.h"
#include "gpio.h"
#include "animation.h"
#include "word_driver.h"

//
// Console....

static void print_console_prompt() { consolePrintValueStrProgmem(PSTR("\r\n> ")); }
static void print_console_seperator() {	consolePrintValueStrProgmem(PSTR(" -> "));}
static void console_init() {
	GPIO_SERIAL_CONSOLE.begin(CFG_CONSOLE_BAUDRATE);
	consoleInit(&GPIO_SERIAL_CONSOLE);								// Setup console to write to default serial port.
	consolePrintValueStrProgmem(PSTR("\r\n\r\nBig Flipper"));		// Console signon message.
	print_console_prompt();											// Start off with a prompt.
}
static void console_error(console_rc_t rc) {
	consolePrintValueStrProgmem(PSTR("Error: "));					// Print error code:(
	consolePrintValueStrProgmem(consoleGetErrorDescription(rc));	// Print description.
	consolePrintValueStrProgmem(PSTR(": "));
	consolePrintValueSignedDecimal(rc);								// Print value as well.
}
static void console_service() {
	if (GPIO_SERIAL_CONSOLE.available()) {							// If a character is available...
		const char c = GPIO_SERIAL_CONSOLE.read();
		console_rc_t rc = consoleAccept(c);							// Add it to the input buffer.
		if (CONSOLE_RC_ERROR_ACCEPT_BUFFER_OVERFLOW == rc) {		// Check for overflow...
			consolePrintValueStr(consoleAcceptBuffer());			// Echo input line back to terminal. 
			console_error(rc);
		}
		else if (CONSOLE_RC_OK == rc) {								// Check for a newline...
			consolePrintValueStr(consoleAcceptBuffer());			// Echo input line back to terminal. 
			print_console_seperator();								// Print separator before output.
			rc = consoleProcess(consoleAcceptBuffer());				// Process input string and record error code. 
			if (CONSOLE_RC_OK != rc) 								// If all went well then we get an OK status code
				console_error(rc);
			print_console_prompt();									// In any case print a prompt ready for the next line of input. 
		}
	}
}

static void debug_init() {
	watchdog_module_mask_t wdt_mask;
	const uint16_t mcusr = debugWatchdogInit(&wdt_mask);
	REGS[REGS_IDX_RESTART] = mcusr | ((uint16_t)wdt_mask << 8);
}
static void regs_init() {
	regsInit();
}	

void setup() {
	debug_init();
	gpioInit();
	regs_init();
	console_init();
	driverInit();
	wordDriverInit();
}

void loop() {
	console_service();
	driverService();

	// Updates display & animation from registers every so often.
	runEveryU16(500) {
		driverUpdateDisplaySettings();
		animation_set(REGS[REGS_IDX_ANIMATION]);		// No-op if no change.
	}
	
	// Service animation.
	runEveryU16(REGS[REGS_IDX_ANIMATION_UPDATE_INTERVAL]) {
		animation_service();
	}
	
	debugKickWatchdog(DEBUG_WATCHDOG_MASK_MAINLOOP);
}
