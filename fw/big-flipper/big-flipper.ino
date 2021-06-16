#include <Arduino.h>

#include "console.h"

#include <Stream.h>
#include <stdarg.h>
#include <stdio.h>

static void print_console_prompt() {
	Serial.println();
	Serial.print(F("> "));
}
static void print_console_seperator() {
	Serial.print(F(" -> "));	
}

void setup() {
	Serial.begin(115200);
	
	// Signon message.
	Serial.println();
	Serial.print(F("Big Flipper"));
	
	consoleInit(&Serial);										// Setup console to write to default serial port.
	print_console_prompt();										// Start off with a prompt.
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
}
