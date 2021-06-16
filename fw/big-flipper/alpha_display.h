// Base class for alpha display.

#ifndef ALPHA_DISPLAY_H__
#define ALPHA_DISPLAY_H__

#include "thread.h"

// Header for constant size bitmap fonts. Only a character range is decoded, else a blank character is displayed.
// To make a null font, set end = start-1. Stupid, but possible.
// Always in Flash and accessed via a pointer.
// 14 segment bit allocation appears to be standard: xPONMLKHGFEDCBA or x DP N M L K J H G2 G1 F E D C B A
typedef struct {
	char 			start_char; 		// First decoded character.
	char			end_char;			// Last decoded character.
	uint8_t			width;				// Width of character bitmap in bytes.
	const uint8_t	bitmaps[0];			// Size = (end - start + 1) * width.
} AlphaDisplayFont;


// TODO: Font should be a const reference. 
class AlphaDisplay {
public:
	// Default constructor does very little. Font can be set to NULL, but nothing will be printed. 
	AlphaDisplay(const AlphaDisplayFont* font);

	// Destructor calls end().
	virtual ~AlphaDisplay();
	
	// There is expected to be a begin method in subclasses possibly taking some arguments. It should call this base class begin() with the
	//  connection success status. It returns the connection status (same as isConnected(). Note that it calls clear(), so if this method is
	//  implemented in subclasses it will cause action on the display device. So make sure that it is configured before calling this. 
	bool begin(bool connected);

	// Just frees allocated memory and clears connected state..
	void end();
	
	// Set a font. If you don't do this, nothing will be displayed!
	void setFont(const AlphaDisplayFont* font) { _font = font; }
	
	// Set text direction. Note I am not an Arabic speaker, this is for easy implementation of a mirrored dusplay with a custom font.
	void setTextDirectionRTL(bool en) { _text_dir_rtl = en; }
	
	// Return number of characters in display.
	virtual uint8_t getCharCount() const = 0;
	
	// Return width of character bitmap in bytes.
	virtual uint8_t getCharBitmapByteSize() const = 0;
	
	// Check if the display is connected.
	bool isConnected() const;
		
	// Clear the display. Called on a successful begin(). Subclasses can override do_clear if there is a display specific method of clearing. 
	void clear(uint8_t start=0, uint8_t end=0xff);

	// Write a string in RAM to the display at the given position.
	void write(const char* str, uint8_t pos=0);
	
	// Write a single character to the display at the given position.
	void write(char c, uint8_t pos);
	
	// Write a bitmap to the display at the given position.
	void writeBitmap(const uint8_t* bmp, uint8_t pos);
	void writeBitmap_P(const uint8_t* bmp, uint8_t pos);

	// Set an update method. Subclasses define these. Index 0 is expected to be a default "vanilla" method, and is selected if the index is out of range.
	void setUpdateMethod(uint8_t m);
	
	// Call frequently in mainloop to service display.
	void service();
	
	// Generic property interface. Allows subclasses to add properties if required.
	enum { PROP_UPDATE_INTERVAL, COUNT_BASE_PROP };
	uint16_t getProperty(uint8_t idx);
	void setProperty(uint8_t idx, uint16_t val);
	virtual uint8_t getPropertyCount() = 0;

protected:
	// Return bitmap for character, or NULL if none.
	const uint8_t* get_char_bitmap(char c);		

	// Flag display as needing an update. All this does is set a flag. There are two situations regarding the update thread:
	//  If the update thread is waiting on this then it starts running and clears the flag.
	//  If it is in the process of updating then it can either:
	//    Run to completion, see that the flag is now set again and restart.
	//    Check the flag every time it runs and do something smarter.
	void set_dirty();

	void set_clean() { _dirty = false; }
	bool is_dirty() { return _dirty; }

	virtual void do_clear(uint8_t start, uint8_t end);
	
	// Return thread function to call for updating display. Typically this indexes into a static table. Returns NULL on out of range. 
	virtual thread_t get_update_thread() = 0;
	
	uint8_t get_update_mode(uint8_t mode_count) { return (_update_mode.thread_func_idx >= mode_count) ? 0 : _update_mode.thread_func_idx; }
	
	// Check if update timer timed out and schedule a new one if it is.
	bool is_update_timeout(uint16_t interval);
	
	// Return segment buffer address for specified position.
	uint8_t* get_seg_buffer(uint8_t pos);
	uint8_t* get_disp_buffer(uint8_t pos);
	
	// Helper function to write a character to the display with no check on pos. 
	void write_helper(char c, uint8_t pos);
/*
	// Random bit picker - to assist in random updates of bitmaps
	class RandomBitPicker {
		RandomBitPicker(uint8_t size) : _size(size) { _set_bit_counts = new uint8_t[_size]; reset(); }
		~RandomBitPicker() { delete _set_bit_counts; }
		void reset() { memset(_set_bit_counts, 0, _size); _set_bit_total = 0; }
		void add(uint8_t idx, uint8_t bitmap) { uint8_t m = 1; fori (8) { if (bitmap & m) _set_bit_counts[i]++, _set_bit_total++; m <,= 1; } }
		uint8_t _size;
		uint8_t* _set_bit_counts;			// Array of counts of set bits for each byte in the bitmap.
		uint8_t  _set_bit_total;				// Total count of array.
	};
*/	
private:
	const AlphaDisplayFont*	_font;			// In progmem.
	bool					_connected;		// Flags if begin() found a device. 
	bool					_dirty;			// Signal to update thread to do something.
	uint16_t				_last_update;	// Schedule next update. 
	uint8_t*				_seg_buffer;
	uint8_t* 				_disp_buffer;
	uint16_t*				_props;			// Array of property values, size set by subclass. 
	bool _text_dir_rtl;
	
	struct {
		uint8_t thread_func_idx;						// Index to update thread.
		thread_control_t tcb;
		int8_t thread_rc ;								// Records result of last run of thread. 
	} _update_mode;
};

#endif	// ALPHA_DISPLAY_H__
