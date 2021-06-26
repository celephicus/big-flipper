#include <Arduino.h>
#include <SD.h>
#include <Wire.h>
#include <SPI.h>

#include "project_config.h"
#include "debug.h"
#include "driver.h"
#include "utils.h"
#include "thread.h"
//#include "adc_driver.h"
#include "regs.h"
#include "console.h"

FILENUM(3);

// Called to initialise display at startup. Also calls driverUpdateDisplaySettings() which updates the settings of the display from the registers.
static void display_init();

// 16K33 14 segment display.
#if defined(CFG_WANT_DISPLAY_14_SEGMENT_LED)	
static HT16K33Display f_display;
HT16K33Display& driverGetDisplay() { return f_display; }
static void display_init() {
	f_display.begin();
	driverUpdateDisplaySettings();
}
static void display_init_local() {
	f_display.setFont((const AlphaDisplayFont*)&HT16K33_DISPLAY_FONT_CAPS);
}

// Flipdot display.
#elif defined(CFG_WANT_DISPLAY_FLIPDOT)
static const uint8_t FLIPDOTS_GPIO_PINS[] PROGMEM = {
	GPIO_PIN_ROW_0, GPIO_PIN_ROW_1, GPIO_PIN_ROW_2, GPIO_PIN_ROW_3, GPIO_PIN_ROW_4, GPIO_PIN_ROW_5, GPIO_PIN_ROW_6,
	GPIO_PIN_ROW_DATA, GPIO_PIN_COL_EN,
};
static FlipdotDisplay f_display(FLIPDOTS_GPIO_PINS); 
FlipdotDisplay& driverGetDisplay() { return f_display; }
static void display_init() {
	f_display.begin();
	driverUpdateDisplaySettings();
}
static void display_init_local() {
	f_display.setProperty(FlipdotDisplay::PROP_PULSE_DURATION, REGS[REGS_IDX_FLIPDISK_PULSE_WIDTH]);
	f_display.setFont((const AlphaDisplayFont*)&FLIPDOT_DISPLAY_FONT_DEFAULT);
}

#else
# error Need CFG_WANT_DISPLAY_xxx
#endif

void driverUpdateDisplaySettings() {
	f_display.setUpdateMethod(REGS[REGS_IDX_DISPLAY_UPDATE_METHOD]);
	f_display.setProperty(AlphaDisplay::PROP_UPDATE_INTERVAL, REGS[REGS_IDX_DISPLAY_UPDATE_INTERVAL]);
	display_init_local();
}


#if 0
// ADC read.
//
const adc_driver_channel_def_t g_adc_def_list[] PROGMEM = {
//	{ ADC_DRIVER_REF_EXTERNAL | ADC_DRIVER_CHAN_FROM_ARDUINO_PIN(GPIO_PIN_VOLUME),			REGS_IDX_ADC_VOLUME_SETTING,	NULL },  
	{ 0,																					ADC_DRIVER_REG_END,				NULL }
};

void adcDriverSetupFunc(void* setup_arg) { (void)setup_arg; }

static void adc_init() {
	adcDriverInit(ADC_DRIVER_CLOCK_PRESCALE_128);  		// 16MHz / 128 = 125kHz.
	// DIDR0 = _BV(ADC1D);	  							// No need to disable for ADC6/7 as they are analogue only.
}
static void adc_start() { adcDriverStartConversions(); }
#endif
// Externals
void driverInit() {
	// adc_init();
	gpioDebugLedSetModeOutput();					// We need a blinky LED.
	display_init();
}

void driverService() {
    runEveryU8(50) {
		//adc_start();  								// Start ADC scan, handled by irupts.
    }
	
	f_display.service();
}

// Runtime error, ooops....
void debugRuntimeError(uint8_t fileno, uint16_t lineno, uint8_t errorno) {
	// Do something!

	while (1)
		/* empty */ ;
}


// This has to go somewhere. 
uint16_t threadGetTicks() { return (uint16_t)millis(); }

// Console output routines.
void consolePrint(uint8_t s, console_cell_t x) {
	switch (s) {
		case CONSOLE_PRINT_NEWLINE:		GPIO_SERIAL_CONSOLE.print(F("\r\n")); (void)x; break;
		case CONSOLE_PRINT_SIGNED:		GPIO_SERIAL_CONSOLE.print(x, DEC); GPIO_SERIAL_CONSOLE.print(' '); break;
		case CONSOLE_PRINT_UNSIGNED:	GPIO_SERIAL_CONSOLE.print('+'); GPIO_SERIAL_CONSOLE.print((console_ucell_t)x, DEC); GPIO_SERIAL_CONSOLE.print(' '); break;
		case CONSOLE_PRINT_HEX:			GPIO_SERIAL_CONSOLE.print('$'); {
											console_ucell_t m = 0xf;
											do {
												if ((console_ucell_t)x <= m) GPIO_SERIAL_CONSOLE.print(0);
												m = (m << 4) | 0xf;
											} while (CONSOLE_UCELL_MAX != m);
										} GPIO_SERIAL_CONSOLE.print((console_ucell_t)x, HEX); GPIO_SERIAL_CONSOLE.print(' '); break;
		case CONSOLE_PRINT_STR:			GPIO_SERIAL_CONSOLE.print((const char*)x); GPIO_SERIAL_CONSOLE.print(' '); break;
		case CONSOLE_PRINT_STR_P:		GPIO_SERIAL_CONSOLE.print((const __FlashStringHelper*)x); GPIO_SERIAL_CONSOLE.print(' '); break;
		default:						/* ignore */; break;
	}
}
