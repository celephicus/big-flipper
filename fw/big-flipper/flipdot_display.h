// Mighty Flipdot driver.

#ifndef FLIPDOT_DISPLAY_H__
#define FLIPDOT_DISPLAY_H__

#include "project_config.h"
#include "thread.h"
#include "alpha_display.h"

// Header for 5 X 7 fonts, each glyph is 5 bytes of 7 bit data.
typedef struct {
	char 			start_char; 				// First decoded character.
	char   			end_char;					// Last decoded character.
	uint8_t			width;						// Width of character bitmap in bytes.
	const uint8_t	bitmaps[5]['\x7f' - ' ' + 1];	// Size = (end - start + 1) * 2.
} FlipdotDisplayFontDefault;
extern const FlipdotDisplayFontDefault FLIPDOT_DISPLAY_FONT_DEFAULT;

typedef struct {
	char 			start_char; 				// First decoded character.
	char   			end_char;					// Last decoded character.
	uint8_t			width;						// Width of character bitmap in bytes.
	const uint8_t	bitmaps[5]['Z' - 'A' + 1];		// Size = (end - start + 1) * 2.
} FlipdotDisplayFontCaps;
extern const FlipdotDisplayFontCaps FLIPDOT_DISPLAY_FONT_CAPS;

typedef struct {
	char 			start_char; 				// First decoded character.
	char   			end_char;					// Last decoded character.
	uint8_t			width;						// Width of character bitmap in bytes.
	const uint8_t	bitmaps[5]['Z' - 'A' + 1];		// Size = (end - start + 1) * 2.
} FlipdotDisplayFontCapsReversed;
extern const FlipdotDisplayFontCapsReversed FLIPDOT_DISPLAY_FONT_CAPS_REVERSED;

class FlipdotDisplay : public AlphaDisplay {
public:
	static const uint8_t ROW_COUNT = 7;
	static const uint8_t CHAR_COUNT = 4;
	static const uint8_t BITMAP_BYTE_SIZE = 5;	 	

	// Initialise with GPIO pins as an array.
	enum { 
		GPIO_PIN_IDX_ROW_0, GPIO_PIN_IDX_ROW_1, GPIO_PIN_IDX_ROW_2, GPIO_PIN_IDX_ROW_3, GPIO_PIN_IDX_ROW_4, GPIO_PIN_IDX_ROW_5, GPIO_PIN_IDX_ROW_6,
		GPIO_PIN_IDX_ROW_DATA, 
		GPIO_PIN_IDX_COL_EN, 
		COUNT_GPIO_PIN_IDX
	};
	bool begin();

	// Default constructor does very little. 
	FlipdotDisplay(const uint8_t* gpio_pins, const AlphaDisplayFont* font=NULL);
	
	// Return number of characters in display.
	virtual uint8_t getCharCount() const { return CHAR_COUNT; } 	

	// Return width of character bitmap in bytes. Really only 15 bits.
	virtual uint8_t getCharBitmapByteSize() const { return BITMAP_BYTE_SIZE; };

	virtual const AlphaDisplayFont* getDefaultFont() { return (const AlphaDisplayFont*)&FLIPDOT_DISPLAY_FONT_DEFAULT; }

	// Public write method for testing the hardware. This uses delay() to do the pulse, so it will block. 
	// Note that row is a bitmask, so you can try flipping multiple rows.
	// Also note that the high_low parameter does not account for alternate rows being inverted as regards set & clear.
	void doFlip(uint8_t col_idx, uint8_t row_mask, bool high_low);
	
	// Update methods used for setUpdateMode().
	enum {
		UPDATE_WIPE_RIGHT,
		UPDATE_WIPE_LEFT,
//		UPDATE_WIPE_DOWN,
//		UPDATE_WIPE_UP,
//		UPDATE_WIPE_LEFT,
		UPDATE_RANDOM,
		COUNT_UPDATE
	};

	// Add any extra properties for this subclass.
	enum { PROP_PULSE_DURATION = COUNT_BASE_PROP, PROP_MIN_RANDOM, COUNT_PROPS };
	virtual uint8_t getPropertyCount() { return COUNT_PROPS; }

protected:
	void set_col(uint8_t col, bool high);
	void write_gpio(uint8_t idx, bool s);
	void set_row(uint8_t row, bool high);
	
	virtual thread_t get_update_thread();

	static int8_t thread_update_horizontal_wipe_helper(void* arg, bool rtl);
	static int8_t thread_update_wipe_right(void* arg);
	static int8_t thread_update_wipe_left(void* arg);
	static int8_t thread_update_random(void* arg);
	
private:
	const uint8_t*	 	_gpio_pins;					// Set by begin(), not by constructor. 
};

#endif	// FLIPDOT_DISPLAY_H__
