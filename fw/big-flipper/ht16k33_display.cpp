#include <Arduino.h>
#include <Wire.h>
#include <avr/pgmspace.h>

#include "ht16k33_display.h"
#include "alpha_display.h"
#include "utils.h"

// Bitmaps for full ASCII set. Borrowed from Adafruit LEDBackpack.
#define FONT_SEGDATA_20_3F					\
	0b0000000000000000, /* <SPACE> */		\
    0b0100000000000110, /* ! */				\
    0b0000001000100000, /* " */				\
    0b0001001011001110, /* # */				\
    0b0001001011101101, /* $ */				\
    0b0000110000100100, /* % */				\
    0b0010001101011101, /* & */				\
    0b0000010000000000, /* ' */				\
    0b0010010000000000, /* ( */				\
    0b0000100100000000, /* ) */				\
    0b0011111111000000, /* * */				\
    0b0001001011000000, /* + */				\
    0b0000100000000000, /* , */				\
    0b0000000011000000, /* - */				\
    0b0100000000000000, /* . */				\
    0b0000110000000000, /* / */				\
    0b0000110000111111, /* 0 */				\
    0b0000000000000110, /* 1 */				\
    0b0000000011011011, /* 2 */				\
    0b0000000010001111, /* 3 */				\
    0b0000000011100110, /* 4 */				\
    0b0010000001101001, /* 5 */				\
    0b0000000011111101, /* 6 */				\
    0b0000000000000111, /* 7 */				\
    0b0000000011111111, /* 8 */				\
    0b0000000011101111, /* 9 */				\
    0b0001001000000000, /* : */				\
    0b0000101000000000, /* ; */				\
    0b0010010000000000, /* < */				\
    0b0000000011001000, /* = */				\
    0b0000100100000000, /* > */				\
    0b0001000010000011, /* ? */				\
    0b0000001010111011  /* @ */				

#define FONT_SEGDATA_40_5A					\
    0b0000000011110111, /* A */				\
    0b0001001010001111, /* B */				\
    0b0000000000111001, /* C */				\
    0b0001001000001111, /* D */				\
    0b0000000011111001, /* E */				\
    0b0000000001110001, /* F */				\
    0b0000000010111101, /* G */				\
    0b0000000011110110, /* H */				\
    0b0001001000000000, /* I */				\
    0b0000000000011110, /* J */				\
    0b0010010001110000, /* K */				\
    0b0000000000111000, /* L */				\
    0b0000010100110110, /* M */				\
    0b0010000100110110, /* N */				\
    0b0000000000111111, /* O */				\
    0b0000000011110011, /* P */				\
    0b0010000000111111, /* Q */				\
    0b0010000011110011, /* R */				\
    0b0000000011101101, /* S */				\
    0b0001001000000001, /* T */				\
    0b0000000000111110, /* U */				\
    0b0000110000110000, /* V */				\
    0b0010100000110110, /* W */				\
    0b0010110100000000, /* X */				\
    0b0001010100000000, /* Y */				\
    0b0000110000001001  /* Z */				
    
#define FONT_SEGDATA_5B_7F					\
	0b0000000000111001, /* [ */				\
    0b0010000100000000, /* | */				\
    0b0000000000001111, /* ] */				\
    0b0000110000000011, /* ^ */				\
    0b0000000000001000, /* _ */				\
    0b0000000100000000, /* ` */				\
    0b0001000001011000, /* a */				\
    0b0010000001111000, /* b */				\
    0b0000000011011000, /* c */				\
    0b0000100010001110, /* d */				\
    0b0000100001011000, /* e */				\
    0b0000000001110001, /* f */				\
    0b0000010010001110, /* g */				\
    0b0001000001110000, /* h */				\
    0b0001000000000000, /* i */				\
    0b0000000000001110, /* j */				\
    0b0011011000000000, /* k */				\
    0b0000000000110000, /* l */				\
    0b0001000011010100, /* m */				\
    0b0001000001010000, /* n */				\
    0b0000000011011100, /* o */				\
    0b0000000101110000, /* p */				\
    0b0000010010000110, /* q */				\
    0b0000000001010000, /* r */				\
    0b0010000010001000, /* s */				\
    0b0000000001111000, /* t */				\
    0b0000000000011100, /* u */				\
    0b0010000000000100, /* v */				\
    0b0010100000010100, /* w */				\
    0b0010100011000000, /* x */				\
    0b0010000000001100, /* y */				\
    0b0000100001001000, /* z */				\
    0b0000100101001001, /* { */				\
    0b0001001000000000, /* | */				\
    0b0010010010001001, /* } */				\
    0b0000010100100000, /* ~ */				\
    0b0011111111111111  /* <DEL> */

// TODO: check against fonts in Noiasca_ht16k33.
#if 0
	0,																		// ' '	32
	S14_B | S14_C | S14_DP,													// !	33
	S14_B | S14_L,															// "	34
	S14_B | S14_C | S14_D | S14_G | S14_H | S14_L | S14_O,					// #	35
	S14_A | S14_C | S14_D | S14_F | S14_G | S14_H | S14_L | S14_O,			// $	36
	S14_A | S14_B | S14_F | S14_G | S14_H,									// %	37
	S14_A | S14_C | S14_D | S14_E | S14_G | S14_K| S14_L  | S14_N,			// &	38   still needs improvement
	S14_L,																	// '	39
	S14_M | S14_P,															// (	40
	S14_K | S14_N,															// )	41
	S14_G | S14_H | S14_K | S14_L | S14_M | S14_N | S14_O | S14_P,			// *	42   updated
	S14_G | S14_H | S14_O | S14_L,											// +	43
	S14_DP,																	// ,	44  
	S14_G | S14_H,															// -	45   updated
	S14_DP,																	// .	46  
	S14_M | S14_N,															// /	47
	S14_A | S14_B | S14_C | S14_D | S14_E | S14_F,							// 0	48
	S14_B | S14_C | S14_M,													// 1	49
	S14_A | S14_B | S14_D | S14_E | S14_G | S14_H,							// 2	50
	S14_A | S14_B | S14_C | S14_D | S14_H,									// 3	51
	S14_B | S14_C | S14_F | S14_G | S14_H,									// 4	52
	S14_A | S14_C | S14_D | S14_F | S14_G | S14_H,							// 5	53
	S14_A | S14_C | S14_D | S14_E | S14_F | S14_G | S14_H,					// 6	54
	S14_A | S14_M | S14_N,													// 7	55
	S14_A | S14_B | S14_C | S14_D | S14_E | S14_F | S14_G | S14_H,			// 8	56
	S14_A | S14_B | S14_C | S14_D | S14_F | S14_G | S14_H,					// 9	57
	S14_B | S14_DP,															// :	58   
	S14_B | S14_DP,															// ;	59   
	S14_M | S14_P,															// <	60
	S14_D | S14_G | S14_H,													// =	61	 updated
	S14_K | S14_N,															// >	62
	S14_A | S14_B | S14_H | S14_O | S14_DP,									// ?	63
	S14_D | S14_E | S14_G | S14_O | S14_N,									// @	64
	S14_A | S14_B | S14_C | S14_E | S14_F | S14_G | S14_H,					// A	65
	S14_A | S14_B | S14_C | S14_D | S14_H | S14_L | S14_O,					// B	66
	S14_A | S14_D | S14_E | S14_F,											// C	67
	S14_A | S14_B | S14_C | S14_D | S14_L | S14_O,							// D	68
	S14_A | S14_D | S14_E | S14_F | S14_G,									// E	69
	S14_A | S14_E | S14_F | S14_G,											// F	70
	S14_A | S14_C | S14_D | S14_E | S14_F  | S14_H,							// G	71
	S14_B | S14_C | S14_E | S14_F | S14_G  | S14_H,							// H	72
	S14_A | S14_D | S14_L | S14_O,											// I	73
	S14_B | S14_C | S14_D | S14_E,											// J	74
	S14_E | S14_F | S14_G | S14_M | S14_P,									// K	75
	S14_D | S14_E | S14_F,													// L	76
	S14_B | S14_C | S14_E | S14_F | S14_K | S14_M,							// M	77
	S14_B | S14_C | S14_E | S14_F | S14_K | S14_P,							// N	78
	S14_A | S14_B | S14_C | S14_D | S14_E | S14_F,							// O	79
	S14_A | S14_B | S14_E | S14_F | S14_G | S14_H,							// P	80
	S14_A | S14_B | S14_C | S14_D | S14_E | S14_F | S14_P,					// Q	81
	S14_A | S14_B | S14_E | S14_F | S14_G | S14_H | S14_P,					// R	82
	S14_A | S14_C | S14_D | S14_H | S14_K,									// S	83
	S14_A | S14_L | S14_O,													// T	84
	S14_B | S14_C | S14_D | S14_E | S14_F,									// U	85
	S14_E | S14_F | S14_M | S14_N,											// V	86
	S14_B | S14_C | S14_E | S14_F | S14_N | S14_P,							// W	87
	S14_K | S14_M | S14_N | S14_P,											// X	88
	S14_B | S14_F | S14_G | S14_H | S14_O,									// Y	89
	S14_A | S14_D | S14_M | S14_N,											// Z	90
	S14_A | S14_D | S14_E | S14_F,											// [	91
	S14_K | S14_P,															// \	92 backslash 
	S14_A | S14_B | S14_C | S14_D,											// ]	93
	S14_F | S14_K,															// ^	94
	S14_D,																	// _	95 underscore
	S14_K,																	// `	96
	S14_D | S14_E | S14_G | S14_O,											// a	97
	S14_D | S14_E | S14_F | S14_G | S14_P,									// b
	S14_D | S14_E | S14_G | S14_H,											// c
	S14_B | S14_C | S14_D | S14_H | S14_N,									// d
	S14_D | S14_E | S14_G | S14_N,											// e
	S14_G | S14_H | S14_M | S14_O,											// f	updated
	S14_B | S14_C | S14_D | S14_H | S14_M,									// g
	S14_E | S14_F | S14_G | S14_O,											// h
	S14_O,																	// i
	S14_B | S14_C | S14_D,													// j
	S14_L | S14_M | S14_O | S14_P,											// k
	S14_L | S14_O,															// l
	S14_C | S14_E | S14_G | S14_H | S14_O,									// m
	S14_E | S14_G | S14_O,													// n
	S14_C | S14_D | S14_E | S14_G | S14_H,									// o
	S14_A | S14_E | S14_F | S14_G | S14_M,									// p
	S14_A | S14_B | S14_C | S14_H | S14_K,									// q
	S14_E | S14_G,															// r
	S14_D | S14_H | S14_P,													// s
	S14_D | S14_E | S14_F | S14_G,											// t
	S14_C | S14_D | S14_E,													// u
	S14_E | S14_N,															// v
	S14_C | S14_E | S14_N | S14_P,											// w
	S14_K | S14_M | S14_N | S14_P,											// x
	S14_B | S14_C | S14_D | S14_H | S14_L,									// y	updated
	S14_D | S14_G | S14_N,													// z	122
	S14_A | S14_D | S14_G | S14_N | S14_K,									// {	123	 
	S14_B | S14_C,															// |	124
	S14_A | S14_D | S14_H | S14_P | S14_M,									// }	125	 
	S14_G,																	// ~	126
	S14_P | S14_O | S14_N | S14_M | S14_L | S14_K | S14_H | S14_G | 
	  S14_H | S14_E | S14_D | S14_C | S14_B | S14_A | S14_DP,				// 127 all segments
#endif

// TODO: Check if non-used statics are removed by linker. 

const HT16K33DisplayFontDefault HT16K33_DISPLAY_FONT_DEFAULT PROGMEM = {
	' ', '\x7f', 2,
	{
		FONT_SEGDATA_20_3F,
		FONT_SEGDATA_40_5A,
		FONT_SEGDATA_5B_7F,
	}
};

const HT16K33DisplayFontDefault HT16K33_DISPLAY_FONT_CAPS PROGMEM = {
	'A', 'Z', 2,
	{
		FONT_SEGDATA_40_5A,
	}
};

HT16K33Display::HT16K33Display(const AlphaDisplayFont* font) : AlphaDisplay(font) {
}

bool HT16K33Display::begin(uint8_t address, uint8_t mode) {
	_i2c_address = address; 
	setProperty(PROP_MIN_RANDOM, 30);
    Wire.beginTransmission(_i2c_address);
    Wire.write(HT16K33_CMD_SYSTEM_SETUP(true));				// Turn on oscillator, takes the display out of sleepy time...
    const bool connected = !Wire.endTransmission();
	
	AlphaDisplay::begin(connected);							// Now that we know the display is present we call the base class.
    if (isConnected()) {
		setDisplayMode(mode); 
		setBrightness(MAX_BRIGHTNESS);
		return true;
	}
	return false;
}

void HT16K33Display::send_cmd(uint8_t cmd) {
	if (isConnected()) {
		Wire.beginTransmission(_i2c_address);
		Wire.write(cmd);           
		Wire.endTransmission();
	}
}

void HT16K33Display::setDisplayMode(uint8_t mode) { 
	send_cmd(HT16K33_DISPLAY_SETUP(mode & 7)); 
}

void HT16K33Display::setBrightness(uint8_t b)  { 
	if (b > MAX_BRIGHTNESS) 
		b = MAX_BRIGHTNESS; 
	send_cmd(HT16K33_DISPLAY_DIM(b)); 
}

void HT16K33Display::write_display(uint8_t s, uint8_t e) { 
	// ASSERT(s < e);	// Since this is an internal method we make this condition, not too difficult for the caller.
	Wire.beginTransmission(_i2c_address);
	Wire.write(HT16K33_CMD_DISPLAY_DATA_WRITE(s * BITMAP_BYTE_SIZE)); // Display uses byte addresses.
	while (s < e) {			 
		Wire.write(get_disp_buffer(s)[0]); 
		Wire.write(get_disp_buffer(s)[1]); 
		s += 1;
	}
	Wire.endTransmission();
}



thread_t HT16K33Display::get_update_thread() { 
	static const thread_t UPDATE_THREADS[] PROGMEM = {
		thread_update_immediate,			// UPDATE_IMMEDIATE
		thread_update_wipe_down,			// UPDATE_WIPE_DOWN
		thread_update_wipe_up,				// UPDATE_WIPE_UP
		thread_update_wipe_left,			// UPDATE_WIPE_LEFT
		thread_update_random,				// UPDATE_RANDOM
	};
	return (thread_t)pgm_read_word(&UPDATE_THREADS[get_update_mode(ELEMENT_COUNT(UPDATE_THREADS))]);
}

int8_t HT16K33Display::thread_update_immediate(void* arg) {
    HT16K33Display* display = (HT16K33Display*)arg;
    THREAD_NO_YIELD_BEGIN();
	
	while (1) {
		THREAD_WAIT_UNTIL(display->is_dirty());				// Wait until the display data is flagged as dirty.
		
		// TODO: Optimise!
		memcpy(display->get_disp_buffer(0), display->get_seg_buffer(0), CHAR_COUNT * BITMAP_BYTE_SIZE);	// Copy to display buffer
		display->write_display(0, CHAR_COUNT);												// Write display buffer to display.
		
		display->set_clean();
	}
	
	THREAD_END();		// Always need one of these.
}

int8_t HT16K33Display::thread_update_wipe_left(void* arg) {
	static const uint16_t MASKS_LEFT[] PROGMEM = {
		0b0000000000110000,		// E F
		0b0000100101000000,		// H G1 L
		0b0001001000001001,		// A D J M
		0b0010010010000000,		// K G2 N
		0b0100000000000110,		// B C DP
	};

    HT16K33Display* display = (HT16K33Display*)arg;
    THREAD_BEGIN();
    
	while (1) {
		THREAD_WAIT_UNTIL(display->is_dirty());				// Wait until the display data is flagged as dirty.

		static uint8_t i, j;
		for (i = 0; i < CHAR_COUNT; i += 1) {	
			for (j = 0; j < ELEMENT_COUNT(MASKS_LEFT); j += 1) {	
				const uint16_t mask = pgm_read_word(&MASKS_LEFT[j]);
				*(uint16_t*)display->get_disp_buffer(i) = (*(uint16_t*)display->get_disp_buffer(i) & ~mask) | (*(uint16_t*)display->get_seg_buffer(i) & mask);
				display->write_display(i, i + 1);
				THREAD_YIELD();
			}
		}
		
		display->set_clean();
	} 
	
	THREAD_END();		// Always need one of these.
}

int8_t HT16K33Display::thread_update_wipe_up_down_helper(void* arg, const uint16_t* mask_list, uint8_t count_masks) {
    HT16K33Display* display = (HT16K33Display*)arg;
    THREAD_BEGIN();
     
	while (1) {
		THREAD_WAIT_UNTIL(display->is_dirty());				// Wait until the display data is flagged as dirty.
		
		static uint8_t i;
		for (i = 0; i < count_masks; i += 1) {		// Loop var must be static since we yield in he loop. 
			const uint16_t mask = pgm_read_word(&mask_list[i]);
			forj (CHAR_COUNT) 
				*(uint16_t*)display->get_disp_buffer(j) = (*(uint16_t*)display->get_disp_buffer(j) & ~mask) | (*(uint16_t*)display->get_seg_buffer(j) & mask);
			display->write_display(0, CHAR_COUNT);
			THREAD_YIELD();
		}
		
		display->set_clean();
	}
	
	THREAD_END();		// Always need one of these.
}
int8_t HT16K33Display::thread_update_wipe_down(void* arg) {
	static const uint16_t MASKS_DOWN[] PROGMEM = {
		0b0000000000000001,		// A
		0b0000011100100010,		// F H J K B
		0b0000000011000000,		// G1 G2
		0b0011100000010100,		// E L M N C
		0b0100000000001000,		// D DP
	};
	return thread_update_wipe_up_down_helper(arg, MASKS_DOWN, ELEMENT_COUNT(MASKS_DOWN));
}
int8_t HT16K33Display::thread_update_wipe_up(void* arg) {
	static const uint16_t MASKS_UP[] PROGMEM = {
		0b0100000000001000,		// D DP
		0b0011100000010100,		// E L M N C
		0b0000000011000000,		// G1 G2
		0b0000011100100010,		// F H J K B
		0b0000000000000001,		// A
	};
	return thread_update_wipe_up_down_helper(arg, MASKS_UP, ELEMENT_COUNT(MASKS_UP));
}

static const uint8_t* memfind(const uint8_t* p, uint8_t size, uint8_t x) {
	while (size-- > 0) {
		if (*p++ == x)
			return p - 1;
	}
	return NULL;
}

int8_t HT16K33Display::thread_update_random(void* arg) {
    HT16K33Display* display = (HT16K33Display*)arg;
    THREAD_BEGIN();
     
	static const uint8_t MINIMUM_TWIDDLES = 30;		// Any less twiddles than this and we make new ones
	static const uint8_t COUNT_BITMAP_BITS = 15;		// Bit 15 is unused, so we can ignore it.
	static uint8_t twiddles[HT16K33Display::CHAR_COUNT * COUNT_BITMAP_BITS];		
	static uint8_t count_twiddles;

	while (1) {
		THREAD_WAIT_UNTIL(display->is_dirty());				// Wait until the display data is flagged as dirty.

		count_twiddles = 0;
		fori (HT16K33Display::CHAR_COUNT) {
			const uint16_t digit_twiddles = *(uint16_t*)display->get_disp_buffer(i) ^ *(uint16_t*)display->get_seg_buffer(i);
			forj (COUNT_BITMAP_BITS) {
				if (digit_twiddles & _BV(j))
					twiddles[count_twiddles++] = i*16+j;		// Note indexes are 16 bits based for efficiency.
			}
		}
		
		while (count_twiddles < MINIMUM_TWIDDLES) {
			static uint8_t fake_twiddle;
			do {
				fake_twiddle = random(CHAR_COUNT) * 16 + random(14);
			} while (NULL != memfind(twiddles, count_twiddles, fake_twiddle));
			fori (2)
				twiddles[count_twiddles++] =  fake_twiddle;
		}
	
		while (count_twiddles > 0) {
			const uint8_t twiddle_idx = random(count_twiddles);
			const uint8_t digit_idx = twiddles[twiddle_idx] / 16;
			const uint16_t digit_mask = _BV(twiddles[twiddle_idx] % 16);
			memmove(&twiddles[twiddle_idx], &twiddles[twiddle_idx + 1], count_twiddles - twiddle_idx - 1);
			count_twiddles -= 1;
			*(uint16_t*)display->get_disp_buffer(digit_idx) ^= digit_mask;		// Flip bit.
			display->write_display(digit_idx, digit_idx + 1);
			THREAD_YIELD();
		}
		
		display->set_clean();
	} 
	
	THREAD_END();		
}

