#include <Arduino.h>
#include <SPI.h>
#include <avr/pgmspace.h>

#include "project_config.h"
#include "debug.h"
#include "utils.h"
//#include "types.h"
//#include "event.h"
#include "regs.h"
#include "flipdot_display.h"
#include "console.h"

FILENUM(78);

// Bitmaps for full ASCII set. Borrowed from https://github.com/Ameba8195/Arduino/blob/master/hardware_v2/cores/arduino/font5x7.h
#define FONT_SEGDATA_20_3F								\
	0x00, 0x00, 0x00, 0x00, 0x00,  /* <SPACE> */		\
	0x00, 0x00, 0x5F, 0x00, 0x00,  /* ! */				\
	0x00, 0x07, 0x00, 0x07, 0x00,  /* " */				\
	0x14, 0x7F, 0x14, 0x7F, 0x14,  /* # */				\
	0x24, 0x2A, 0x7F, 0x2A, 0x12,  /* $ */				\
	0x23, 0x13, 0x08, 0x64, 0x62,  /* % */				\
	0x36, 0x49, 0x56, 0x20, 0x50,  /* & */				\
	0x00, 0x08, 0x07, 0x03, 0x00,  /* ' */				\
	0x00, 0x1C, 0x22, 0x41, 0x00,  /* ( */				\
	0x00, 0x41, 0x22, 0x1C, 0x00,  /* ) */				\
	0x2A, 0x1C, 0x7F, 0x1C, 0x2A,  /* * */				\
	0x08, 0x08, 0x3E, 0x08, 0x08,  /* + */				\
	0x00, 0x80, 0x70, 0x30, 0x00,  /* , */				\
	0x08, 0x08, 0x08, 0x08, 0x08,  /* - */				\
	0x00, 0x00, 0x60, 0x60, 0x00,  /* . */				\
	0x20, 0x10, 0x08, 0x04, 0x02,  /* / */				\
	0x3E, 0x51, 0x49, 0x45, 0x3E,  /* 0 */				\
	0x00, 0x42, 0x7F, 0x40, 0x00,  /* 1 */				\
	0x72, 0x49, 0x49, 0x49, 0x46,  /* 2 */				\
	0x21, 0x41, 0x49, 0x4D, 0x33,  /* 3 */				\
	0x18, 0x14, 0x12, 0x7F, 0x10,  /* 4 */				\
	0x27, 0x45, 0x45, 0x45, 0x39,  /* 5 */				\
	0x3C, 0x4A, 0x49, 0x49, 0x31,  /* 6 */				\
	0x41, 0x21, 0x11, 0x09, 0x07,  /* 7 */				\
	0x36, 0x49, 0x49, 0x49, 0x36,  /* 8 */				\
	0x46, 0x49, 0x49, 0x29, 0x1E,  /* 9 */				\
	0x00, 0x00, 0x14, 0x00, 0x00,  /* : */				\
	0x00, 0x40, 0x34, 0x00, 0x00,  /* ; */				\
	0x00, 0x08, 0x14, 0x22, 0x41,  /* < */				\
	0x14, 0x14, 0x14, 0x14, 0x14,  /* = */				\
	0x00, 0x41, 0x22, 0x14, 0x08,  /* > */				\
	0x02, 0x01, 0x59, 0x09, 0x06,  /* ? */				\
    0x3E, 0x41, 0x5D, 0x59, 0x4E   /* @ */

#define FONT_SEGDATA_40_5A								\
	0x7C, 0x12, 0x11, 0x12, 0x7C,  /* A */				\
	0x7F, 0x49, 0x49, 0x49, 0x36,  /* B */				\
	0x3E, 0x41, 0x41, 0x41, 0x22,  /* C */				\
	0x7F, 0x41, 0x41, 0x41, 0x3E,  /* D */				\
	0x7F, 0x49, 0x49, 0x49, 0x41,  /* E */				\
	0x7F, 0x09, 0x09, 0x09, 0x01,  /* F */				\
	0x3E, 0x41, 0x41, 0x51, 0x73,  /* G */				\
	0x7F, 0x08, 0x08, 0x08, 0x7F,  /* H */				\
	0x00, 0x41, 0x7F, 0x41, 0x00,  /* I */				\
	0x20, 0x40, 0x41, 0x3F, 0x01,  /* J */				\
	0x7F, 0x08, 0x14, 0x22, 0x41,  /* K */				\
	0x7F, 0x40, 0x40, 0x40, 0x40,  /* L */				\
	0x7F, 0x02, 0x1C, 0x02, 0x7F,  /* M */				\
	0x7F, 0x04, 0x08, 0x10, 0x7F,  /* N */				\
	0x3E, 0x41, 0x41, 0x41, 0x3E,  /* O */				\
	0x7F, 0x09, 0x09, 0x09, 0x06,  /* P */				\
	0x3E, 0x41, 0x51, 0x21, 0x5E,  /* Q */				\
	0x7F, 0x09, 0x19, 0x29, 0x46,  /* R */				\
	0x26, 0x49, 0x49, 0x49, 0x32,  /* S */				\
	0x03, 0x01, 0x7F, 0x01, 0x03,  /* T */				\
	0x3F, 0x40, 0x40, 0x40, 0x3F,  /* U */				\
	0x1F, 0x20, 0x40, 0x20, 0x1F,  /* V */				\
	0x3F, 0x40, 0x38, 0x40, 0x3F,  /* W */				\
	0x63, 0x14, 0x08, 0x14, 0x63,  /* X */				\
	0x03, 0x04, 0x78, 0x04, 0x03,  /* Y */				\
	0x61, 0x59, 0x49, 0x4D, 0x43   /* Z */

#define FONT_SEGDATA_5B_7F								\
	0x00, 0x7F, 0x41, 0x41, 0x41,  /* [ */				\
	0x02, 0x04, 0x08, 0x10, 0x20,  /* | */				\
	0x00, 0x41, 0x41, 0x41, 0x7F,  /* ] */				\
	0x04, 0x02, 0x01, 0x02, 0x04,  /* ^ */				\
	0x40, 0x40, 0x40, 0x40, 0x40,  /* _ */				\
	0x00, 0x03, 0x07, 0x08, 0x00,  /* ` */				\
	0x20, 0x54, 0x54, 0x78, 0x40,  /* a */				\
	0x7F, 0x28, 0x44, 0x44, 0x38,  /* b */				\
	0x38, 0x44, 0x44, 0x44, 0x28,  /* c */				\
	0x38, 0x44, 0x44, 0x28, 0x7F,  /* d */				\
	0x38, 0x54, 0x54, 0x54, 0x18,  /* e */				\
	0x00, 0x08, 0x7E, 0x09, 0x02,  /* f */				\
	0x18, 0xA4, 0xA4, 0x9C, 0x78,  /* g */				\
	0x7F, 0x08, 0x04, 0x04, 0x78,  /* h */				\
	0x00, 0x44, 0x7D, 0x40, 0x00,  /* i */				\
	0x20, 0x40, 0x40, 0x3D, 0x00,  /* j */				\
	0x7F, 0x10, 0x28, 0x44, 0x00,  /* k */				\
	0x00, 0x41, 0x7F, 0x40, 0x00,  /* l */				\
	0x7C, 0x04, 0x78, 0x04, 0x78,  /* m */				\
	0x7C, 0x08, 0x04, 0x04, 0x78,  /* n */				\
	0x38, 0x44, 0x44, 0x44, 0x38,  /* o */				\
	0xFC, 0x18, 0x24, 0x24, 0x18,  /* p */				\
	0x18, 0x24, 0x24, 0x18, 0xFC,  /* q */				\
	0x7C, 0x08, 0x04, 0x04, 0x08,  /* r */				\
	0x48, 0x54, 0x54, 0x54, 0x24,  /* s */				\
	0x04, 0x04, 0x3F, 0x44, 0x24,  /* t */				\
	0x3C, 0x40, 0x40, 0x20, 0x7C,  /* u */				\
	0x1C, 0x20, 0x40, 0x20, 0x1C,  /* v */				\
	0x3C, 0x40, 0x30, 0x40, 0x3C,  /* w */				\
	0x44, 0x28, 0x10, 0x28, 0x44,  /* x */				\
	0x4C, 0x90, 0x90, 0x90, 0x7C,  /* y */				\
	0x44, 0x64, 0x54, 0x4C, 0x44,  /* z */				\
	0x00, 0x08, 0x36, 0x41, 0x00,  /* { */				\
	0x00, 0x00, 0x77, 0x00, 0x00,  /* | */				\
	0x00, 0x41, 0x36, 0x08, 0x00,  /* } */				\
	0x02, 0x01, 0x02, 0x04, 0x02,  /* ~ */				\
	0x7f, 0x7f, 0x7f, 0x7f, 0x7f   /* <DEL> */

#define FONT_SEGDATA_40_5A_REVERSED						\
	0x7c, 0x12, 0x11, 0x12, 0x7c,   /* A */             \
	0x36, 0x49, 0x49, 0x49, 0x7f,   /* B */             \
	0x22, 0x41, 0x41, 0x41, 0x3e,   /* C */             \
	0x3e, 0x41, 0x41, 0x41, 0x7f,   /* D */             \
	0x41, 0x49, 0x49, 0x49, 0x7f,   /* E */             \
	0x01, 0x09, 0x09, 0x09, 0x7f,   /* F */             \
	0x73, 0x51, 0x41, 0x41, 0x3e,   /* G */             \
	0x7f, 0x08, 0x08, 0x08, 0x7f,   /* H */             \
	0x00, 0x41, 0x7f, 0x41, 0x00,   /* I */             \
	0x01, 0x3f, 0x41, 0x40, 0x20,   /* J */             \
	0x41, 0x22, 0x14, 0x08, 0x7f,   /* K */             \
	0x40, 0x40, 0x40, 0x40, 0x7f,   /* L */             \
	0x7f, 0x02, 0x1c, 0x02, 0x7f,   /* M */             \
	0x7f, 0x10, 0x08, 0x04, 0x7f,   /* N */             \
	0x3e, 0x41, 0x41, 0x41, 0x3e,   /* O */             \
	0x06, 0x09, 0x09, 0x09, 0x7f,   /* P */             \
	0x5e, 0x21, 0x51, 0x41, 0x3e,   /* Q */             \
	0x46, 0x29, 0x19, 0x09, 0x7f,   /* R */             \
	0x32, 0x49, 0x49, 0x49, 0x26,   /* S */             \
	0x03, 0x01, 0x7f, 0x01, 0x03,   /* T */             \
	0x3f, 0x40, 0x40, 0x40, 0x3f,   /* U */             \
	0x1f, 0x20, 0x40, 0x20, 0x1f,   /* V */             \
	0x3f, 0x40, 0x38, 0x40, 0x3f,   /* W */             \
	0x63, 0x14, 0x08, 0x14, 0x63,   /* X */             \
	0x03, 0x04, 0x78, 0x04, 0x03,   /* Y */             \
	0x43, 0x4d, 0x49, 0x59, 0x61    /* Z */             \
	
const FlipdotDisplayFontDefault FLIPDOT_DISPLAY_FONT_DEFAULT PROGMEM = {
	' ', '\x7f', 5,
	{
		FONT_SEGDATA_20_3F,
		FONT_SEGDATA_40_5A,
		FONT_SEGDATA_5B_7F,
	}
};

const FlipdotDisplayFontCaps FLIPDOT_DISPLAY_FONT_CAPS PROGMEM = {
	'A', 'Z', 5,
	{
		FONT_SEGDATA_40_5A,
	}
};

const FlipdotDisplayFontCapsReversed FLIPDOT_DISPLAY_FONT_CAPS_REVERSED PROGMEM = {
	'A', 'Z', 5,
	{
		FONT_SEGDATA_40_5A_REVERSED,
	}
};

FlipdotDisplay::FlipdotDisplay(const uint8_t* gpio_pins, const AlphaDisplayFont* font) : AlphaDisplay(font), _gpio_pins(gpio_pins) {
}

#ifdef WANT_TIMER
// Set up OC1B (COL_EN) as one shot, clever idea due to Bigjosh, https://github.com/bigjosh/TimerShot/blob/master/TimerShot.ino.
//  Also http://wp.josh.com/2015/03/05/the-perfect-pulse-some-tricks-for-generating-precise-one-shots-on-avr8/.
//  I've modofied it to use timer 1, which is 16 bit, running at 16MHz with a prescale of 8 so clock rate of 2MHz, max time 30ms, resolution 500ns. 

#define OSP_SET_WIDTH(_cycles) (OCR1B = 0xffff-((_cycles)-1))

// Setup the one-shot pulse generator and initialize with a pulse width that is (cycles) clock counts long.
void osp_setup(uint16_t cycles) {
  TCCR1B =  0;      	// Halt counter by setting clock select bits to 0 (No clock source).
						// This keeps anything from happening while we get set up.

  TCNT1 = 0x00;     	// Start counting at bottom. 
  OCR1A = 0;      		// Set TOP to 0. This effectively keeps us from counting becuase the counter just keeps reseting back to 0.
		// We break out of this by manually setting the TCNT higher than 0, in which case it will count all the way up to MAX and then overflow 
		//  back to 0 and get locked up again.
  OSP_SET_WIDTH(cycles);    // This also makes new OCR values get loaded from the buffer on every clock cycle. 

	// Note that Timer1 uses different modes than timer2, we want mode 4 with TOP set by OCR1A, termed "Clear Timer on Compare Match (CTC) Mode" in datasheet. 
  TCCR1A = _BV(COM1B0) | _BV(COM1B1);					// OC1B=Set on Match, clear on BOTTOM. Set mode 4 CTC(TOP in OCR1A).
  TCCR1B = _BV(WGM12)  | _BV(CS11);						// Start counting now, prescale = 8. Set mode 4 CTC(TOP in OCR1A).

  DDRB |= _BV(2);     // Set pin to output (Note that OC1B = GPIO port PB2 = Arduino Digital Pin 10)
}

// Setup the one-shot pulse generator
void osp_setup() {
  osp_setup(1);
}

// Fire a one-shot pulse. Use the most recently set width. 
#define OSP_FIRE() (TCNT1 = OCR1B - 1)

// Test there is currently a pulse still in progress
#define OSP_INPROGRESS() (TCNT1>0)

// Fire a one-shot pusle with the specififed width. 
// Order of operations in calculating m must avoid overflow of the unint8_t.
// TCNT1 starts one count lower than the match value becuase the chip will block any compare on the cycle after setting a TCNT. 
#define OSP_SET_AND_FIRE(_cycles) {uint16_t m=0xffff-(_cycles-1); OCR1B=m;TCNT1 =m-1;}


// End of bigjosh's code.

#endif

// All we do here is set all pins to output and low.
bool FlipdotDisplay::begin() {
	AlphaDisplay::begin(true);									// Since we can't sense connection status we just assume that we are.

	setProperty(PROP_PULSE_DURATION, 500);						// Set a sensible default value.
	fori (COUNT_GPIO_PIN_IDX) {									// Set all driver pins to output.
		const uint8_t pin_no = pgm_read_byte(&_gpio_pins[i]);
		pinMode(pin_no, OUTPUT);
		digitalWrite(pin_no, 0);
	}
	SPI.begin();												// This needs to be called before using SPI. It is reference counted so end() does not deinit until it has gone to zero. 
	
#ifdef WANT_TIMER
	osp_setup();
#endif	
	if (isConnected()) { // The base class might not flag as connected for some reason, so only proceed if it is OK.
		memset(get_disp_buffer(0), 0x7f, CHAR_COUNT * BITMAP_BYTE_SIZE);			// Fake a fully set display, forces the update thread to clear every pixel on clear().
		return true;
	}
	return false;
}

// Arm the column drives as exactly one set or exactly one clear. Note that they are not enabled yet, so the drive lines are still open.
void FlipdotDisplay::set_col(uint8_t col, bool high) {
	// SR's wired as 17..23, 9..16, 1..8. Bad planning.
	const uint32_t col_data = (1UL << col) ^ (high ? 0UL : 0xffffffffUL);
	fori (3)
		SPI.transfer(((uint8_t*)&col_data)[2-i]);
}

void FlipdotDisplay::write_gpio(uint8_t idx, bool s) { digitalWrite(pgm_read_byte(&_gpio_pins[idx]), s); }

void FlipdotDisplay::set_row(uint8_t row, bool high) {
	fori (ROW_COUNT)
		write_gpio(GPIO_PIN_IDX_ROW_0 + i, !!(row & _BV(i)));
	write_gpio(GPIO_PIN_IDX_ROW_DATA, high);
}

thread_t FlipdotDisplay::get_update_thread() {
	static const thread_t UPDATE_THREADS[] PROGMEM = {
		thread_update_wipe_right,			// UPDATE_WIPE_RIGHT
		thread_update_wipe_left,			// UPDATE_WIPE_LEFT
		thread_update_random,				// UPDATE_RANDOM
	};
	return (thread_t)pgm_read_word(&UPDATE_THREADS[get_update_mode(ELEMENT_COUNT(UPDATE_THREADS))]);
}

static void dump_flip(uint8_t col, uint8_t row, bool f) {
	if (REGS[REGS_IDX_ENABLES] & REGS_ENABLES_MASK_LOGGING) {
		consolePrint(CONSOLE_PRINT_STR_P, (console_cell_t)PSTR("(")); 
		consolePrint(CONSOLE_PRINT_SIGNED, col);
		consolePrint(CONSOLE_PRINT_SIGNED, row);
		consolePrint(CONSOLE_PRINT_SIGNED, f);
//		consolePrint(CONSOLE_PRINT_NEWLINE, 0);
		consolePrint(CONSOLE_PRINT_STR_P, (console_cell_t)PSTR(")")); 
	}
}

int8_t FlipdotDisplay::thread_update_horizontal_wipe_helper(void* arg, bool rtl) {
    FlipdotDisplay* display = (FlipdotDisplay*)arg;
    THREAD_BEGIN();

	while (1) {
		THREAD_WAIT_UNTIL(display->is_dirty());				// Wait until the display data is flagged as dirty.
		// display->set_clean();			// Now set clean, so if the display is set dirty again we will cycle through this thread again and set the state.

		static uint8_t col, row;
		const uint8_t COL_COUNT = CHAR_COUNT * BITMAP_BYTE_SIZE;
		for (col = rtl ? COL_COUNT : 0; rtl ? (col >= 0) : (col < COL_COUNT); col += rtl ? -1 : +1) { // Iterate over all COLUMNS, we ignore character divisions...
			static uint8_t *p_col_disp, *p_col_seg;
			p_col_disp = display->get_disp_buffer(0) + col;
			p_col_seg =  display->get_seg_buffer(0) + col;
			
			if (*p_col_disp != *p_col_seg) {													// If the row data for this column needs updating...
				for (row = 0; row < ROW_COUNT; row += 1) {										// Iterate over all ROWS...
					const bool current_flipstate = !!(*p_col_disp & _BV(row));					// Get CURRENT state of dot.
					const bool new_flipstate = !!(*p_col_seg & _BV(row));						// Get REQUIRED state of dot.
					if (current_flipstate != new_flipstate) {									// If they differ a flip is required...
						dump_flip(col, row, new_flipstate);
						const bool high_low = new_flipstate ^ !(row & 1);			// Decide which way to flip, odd numbered rows are INVERTED.
						display->doFlip(col, _BV(row), high_low);
						THREAD_YIELD();					
					}
				}
				*p_col_disp  = *p_col_seg;														// Don't forget to update the display data.
			}
		}
		display->set_clean();			
	}

	THREAD_END();
}

int8_t FlipdotDisplay::thread_update_wipe_right(void* arg) { return thread_update_horizontal_wipe_helper(arg, false); }
int8_t FlipdotDisplay::thread_update_wipe_left(void* arg) { return thread_update_horizontal_wipe_helper(arg, true); }

int8_t FlipdotDisplay::thread_update_random(void* arg) {
    FlipdotDisplay* display = (FlipdotDisplay*)arg;
    THREAD_BEGIN();

	static const uint8_t COUNT_BITMAP_BITS = 7;		// Bit 7 is unused, so we can ignore it.
	static uint8_t twiddles[CHAR_COUNT * BITMAP_BYTE_SIZE * COUNT_BITMAP_BITS];		
	static uint8_t s_count_twiddles;

	while (1) {
		THREAD_WAIT_UNTIL(display->is_dirty());				// Wait until the display data is flagged as dirty.
		//display->set_clean();			// Now set clean, so if the display is set dirty again we will cycle through this thread again and set the state.

		// Find every difference in pixel state.
		s_count_twiddles = 0;
		for (uint8_t col = 0; col < CHAR_COUNT * BITMAP_BYTE_SIZE; col += 1) {							// Iterate over all COLUMNS...
			const uint8_t col_twiddles = ((display->get_seg_buffer(0))[col]) ^ ((display->get_disp_buffer(0))[col]);
			for (uint8_t row = 0; row < COUNT_BITMAP_BITS; row += 1) {		// Bit 7 is unused.
				if (col_twiddles & _BV(row))
					twiddles[s_count_twiddles++] = col * 8 + row;		// Note indexes are 8 bit based for efficiency.
			}
		}

	// Choose one twiddle at random, remove from list and flip it, then yield.
		while (s_count_twiddles > 0) {
			const uint8_t twiddle_idx = random(s_count_twiddles);
			const uint8_t col = twiddles[twiddle_idx] / 8;
			const uint8_t row = twiddles[twiddle_idx] % 8;
			memmove(&twiddles[twiddle_idx], &twiddles[twiddle_idx + 1], s_count_twiddles - twiddle_idx - 1);
			s_count_twiddles -= 1;

			uint8_t* p_col_disp = display->get_disp_buffer(0) + col;
			uint8_t* p_col_seg =  display->get_seg_buffer(0) + col;
			
			const bool new_flipstate = !!(*p_col_seg & _BV(row));						// Get REQUIRED state of dot from display buffer.
			dump_flip(col, row, new_flipstate);
			const bool high_low = new_flipstate ^ !(row & 1);			// Decide which way to flip, odd numbered rows are INVERTED.
			display->doFlip(col, _BV(row), high_low);
			if (new_flipstate)
				*p_col_disp |= _BV(row);														// Don't forget to update the display data.
			else
				*p_col_disp &= ~_BV(row);
			
			THREAD_YIELD();
		}
		display->set_clean();	
	}

	THREAD_END();
}

// Raw flip function. Blocks but only for a few hundred us.
void FlipdotDisplay::doFlip(uint8_t col_idx, uint8_t row_mask, bool high_low) {
	set_col(col_idx, high_low);
	set_row(row_mask, !high_low);
#ifdef WANT_TIMER
	OSP_SET_AND_FIRE(display->getProperty(PROP_PULSE_DURATION));
	while (!OSP_INPROGRESS())
		; 
#else
	write_gpio(GPIO_PIN_IDX_COL_EN, true);
	delayMicroseconds(getProperty(PROP_PULSE_DURATION));
	write_gpio(GPIO_PIN_IDX_COL_EN, false);
#endif						
}


