#include <Arduino.h>
#include <SD.h>
#include <Wire.h>
#include <SPI.h>

#include "project_config.h"
#include "debug.h"
#include "driver.h"
//#include "types.h"
//#include "event.h"
#include "utils.h"
#include "thread.h"
//#include "adc_driver.h"
#include "regs.h"

FILENUM(3);

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
