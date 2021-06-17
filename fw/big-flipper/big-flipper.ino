#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#define FASTLED_USE_GLOBAL_BRIGHTNESS 0
#include <FastLED.h>

#include "console.h"

#include "project_config.h"
#include "debug.h"
FILENUM(1);

//
// Console....

static void print_console_prompt() {
	Serial.println();
	Serial.print(F("> "));
}
static void print_console_seperator() {
	Serial.print(F(" -> "));	
}
static void console_init() {
	Serial.println();
	Serial.print(F("Big Flipper"));								// Console signon message.
	consoleInit(&Serial);										// Setup console to write to default serial port.
	print_console_prompt();										// Start off with a prompt.
}

//
// Display...

#define CFG_WANT_DISPLAY_14_SEGMENT_LED
/*
// 16K33 14 segment display.
#if defined(CFG_WANT_DISPLAY_14_SEGMENT_LED)	
static HT16K33Display f_display((const AlphaDisplayFont*)&HT16K33_DISPLAY_FONT_DEFAULT);
HT16K33Display& driverGetDisplay() { return f_display; }
static void display_init() {
	f_display.begin();
	driverInitDisplay();
}
static void display_init_local() {}

// Flipdot display.
#elif defined(CFG_WANT_DISPLAY_FLIPDOT)
static const uint8_t FLIPDOTS_GPIO_PINS[] PROGMEM = {
	GPIO_PIN_ROW_0, GPIO_PIN_ROW_1, GPIO_PIN_ROW_2, GPIO_PIN_ROW_3, GPIO_PIN_ROW_4, GPIO_PIN_ROW_5, GPIO_PIN_ROW_6,
	GPIO_PIN_ROW_DATA, GPIO_PIN_COL_EN,
};
static FlipdotDisplay f_display(FLIPDOTS_GPIO_PINS, (const AlphaDisplayFont*)&FLIPDOT_DISPLAY_FONT_DEFAULT); //FLIPDOT_DISPLAY_FONT_CAPS_REVERSED
FlipdotDisplay& driverGetDisplay() { return f_display; }
static void display_init() {
	f_display.begin();
	driverInitDisplay();
}
static void display_init_local() {
	f_display.setProperty(FlipdotDisplay::PROP_PULSE_DURATION, REGS[REGS_IDX_FLIPDISK_PULSE_WIDTH]);
	//f_display.setTextDirectionRTL(true);
}

#else
# error Need CFG_WANT_DISPLAY_xxx
#endif

void driverInitDisplay() {
	f_display.setUpdateMethod(REGS[REGS_IDX_DISPLAY_UPDATE_METHOD]);
	f_display.setProperty(AlphaDisplay::PROP_UPDATE_INTERVAL, REGS[REGS_IDX_DISPLAY_UPDATE_INTERVAL]);
	display_init_local();
}
*/
void setup() {
	Serial.begin(CFG_CONSOLE_BAUDRATE);
	console_init();
}

// Runtime error, ooops....
void debugRuntimeError(uint8_t fileno, uint16_t lineno, uint8_t errorno) {
	// Do something!

	while (1)
		/* empty */ ;
}
void loop() {
	if (Serial.available()) {									// If a character is available...
		const char c = Serial.read();
		console_rc_t rc = consoleAccept(c);						// Add it to the input buffer.
		if (CONSOLE_RC_ERROR_ACCEPT_BUFFER_OVERFLOW == rc) {		// Check for overflow...
			Serial.print(consoleAcceptBuffer());				// Echo input line back to terminal. 
			print_console_seperator();		
			Serial.print(F("Input buffer overflow."));
			print_console_prompt();
		}
		else if (CONSOLE_RC_OK == rc) {						// Check for a newline...
			Serial.print(consoleAcceptBuffer());				// Echo input line back to terminal. 
			print_console_seperator();							// Print separator before output.
			rc = consoleProcess(consoleAcceptBuffer());			// Process input string from input buffer filled by accept and record error code. 
			if (CONSOLE_RC_OK != rc) {						// If all went well then we get an OK status code
				Serial.print(F("Error: "));						// Print error code:(
				Serial.print((const __FlashStringHelper *)consoleGetErrorDescription(rc));	// Print description.
				Serial.print(F(": "));
				Serial.print(rc);
			}
			print_console_prompt();								// In any case print a newline and prompt ready for the next line of input. 
		}
	}
	ASSERT(0); 
}
