// HT16K33 Library based off NoiascaHt16k33.

#ifndef HT16K33_DISPLAY_H__
#define HT16K33_DISPLAY_H__

#include "thread.h"
#include "alpha_display.h"

// Header for 14 segment fonts. The bitmap data is the same as Adafruit's used for theit LED backpack, and also other 14 segment libs.
// 14 segment bit allocation appears to be standard: xPONMLKHGFEDCBA or x DP N M L K J H G2 G1 F E D C B A.
// This is a hack as to use this with AlphaDisplay we need to cast this to AlphaDisplayFont.
typedef struct {
	char 			start_char; 				// First decoded character.
	char   			end_char;					// Last decoded character.
	uint8_t			width;						// Width of character bitmap in bytes.
	const uint16_t	bitmaps['\x7f' - ' ' + 1];	// Size = (end - start + 1) * 2.
} HT16K33DisplayFontDefault;
extern const HT16K33DisplayFontDefault HT16K33_DISPLAY_FONT_DEFAULT;

typedef struct {
	char 			start_char; 				// First decoded character.
	char   			end_char;					// Last decoded character.
	uint8_t			width;						// Width of character bitmap in bytes.
	const uint16_t	bitmaps['Z' - 'A' + 1];		// Size = (end - start + 1) * 2.
} HT16K33DisplayFontCaps;
extern const HT16K33DisplayFontCaps HT16K33_FONT_CAPS;

class HT16K33Display : public AlphaDisplay {
public:
	static const uint8_t DEFAULT_I2C_ADDRESS = 0x70;		
	static const uint8_t CHAR_COUNT = 4;		
	static const uint8_t BITMAP_BYTE_SIZE = 2;	 	

	// Initialise with i2c address. Display is set to mode, which defaults to on.
	bool begin(uint8_t address=DEFAULT_I2C_ADDRESS, uint8_t mode=MODE_ON);

	// Default constructor does very little. 
	HT16K33Display(const AlphaDisplayFont* font=NULL);
	
	// Return number of characters in display.
	virtual uint8_t getCharCount() const { return CHAR_COUNT; } 	// Chip can do 8 but the little module that everyone uses is only 4.

	// Return width of character bitmap in bytes. Really only 15 bits.
	virtual uint8_t getCharBitmapByteSize() const { return BITMAP_BYTE_SIZE; };

	// Set display mode, one of MODE_xxx.
	void setDisplayMode(uint8_t mode);

	// Set the brightness of the display  as 4 bit value, note 15 is full, 0 is very dim..
	void setBrightness(uint8_t b);		

	virtual const AlphaDisplayFont* getDefaultFont() { return (const AlphaDisplayFont*)&HT16K33_DISPLAY_FONT_DEFAULT; }

	// Update methods used for setUpdateMode().
	enum {
		UPDATE_IMMEDIATE,
		UPDATE_WIPE_DOWN,
		UPDATE_WIPE_UP,
		UPDATE_WIPE_LEFT,
		UPDATE_RANDOM,
		COUNT_UPDATE
	};

	// Add any extra properties for this subclass.
	enum { PROP_MIN_RANDOM = AlphaDisplay::COUNT_BASE_PROP, COUNT_PROPS };
	virtual uint8_t getPropertyCount() { return COUNT_PROPS; }

protected:
	// Send a single command to the display.
	void send_cmd(uint8_t cmd);

	// Command definitions from datasheet p30.
	static uint8_t HT16K33_CMD_DISPLAY_DATA_WRITE(uint8_t address) { return address; }		// Write display data starting from 4 bit address.
	static uint8_t HT16K33_CMD_SYSTEM_SETUP(bool osc) { return 0x20	| osc; }				// System Setup Register controls oscillator only.
	static uint8_t HT16K33_KEY_DATA_READ(uint8_t address) { return 0x40	| address; }		// Read key scan data starting from 3 bit address.
	static uint8_t HT16K33_INT_FLAG_READ(uint8_t address) { return 0x60	| address; }		// Read INT flag either 0 or 0xff.
	enum {
		MODE_OFF =			0,
		MODE_ON = 			(0 << 1) | 1,
		MODE_BLINK_2HZ = 	(1 << 1) | 1,
		MODE_BLINK_1HZ = 	(2 << 1) | 1,
		MODE_BLINK_0_5HZ =	(3 << 1) | 1
	};
	static uint8_t HT16K33_DISPLAY_SETUP(uint8_t flags) { return 0x80 | flags; }			// Set display on/off & blink.
	static uint8_t HT16K33_ROW_INT_SET(uint8_t flags) { return 0xa0 | flags; }				// Defines INT/ROW output pin select and INT pin output active level status.
	static const uint8_t MAX_BRIGHTNESS = 15;
	static uint8_t HT16K33_DISPLAY_DIM(uint8_t dim) { return 0xe0 | dim; }					// Set display dimming as 4 bit value, note 15 is full, 0 is very dim..
	static uint8_t HT16K33_TESTMODE(uint8_t dim) { return 0xd9; }							// Unused.

	// Copy display data from buffer to display.
	void write_display(uint8_t s, uint8_t e);

	virtual thread_t get_update_thread();

	static int8_t thread_update_immediate(void* arg);
	static int8_t thread_update_wipe_left(void* arg);
	static int8_t thread_update_wipe_up_down_helper(void* arg, const uint16_t* mask_list, uint8_t count_masks);
	static int8_t thread_update_wipe_down(void* arg);
	static int8_t thread_update_wipe_up(void* arg);
	static int8_t thread_update_random(void* arg);
	
private:
	uint8_t	 			_i2c_address;					// Set by begin(), not by constructor. 
};

#endif	// HT16K33_DISPLAY_H__
