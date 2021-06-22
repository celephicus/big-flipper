#include "project_config.h"
#include "driver.h"
#include "WORD_driver.h"
#include "regs.h"
#include <avr/wdt.h>																			

REGS_CONSOLE_EXTRA

// User commands, these are really examples...
static bool console_cmds_user(char* cmd) {
	switch (hash(cmd)) {
		DRIVER_CONSOLE_COMMANDS
		REGS_CONSOLE_COMMANDS
		WORD_DRIVER_CONSOLE_COMMANDS
		case /** + **/ 0xb58e: binop(+); break;
		case /** - **/ 0xb588: binop(-); break;
		case /** NEGATE **/ 0x7a79: unop(-); break;
		case /** # **/ 0xb586: raise(CONSOLE_RC_SIGNAL_IGNORE_TO_EOL); break;
		default: return false;
	}
	return true;
}

