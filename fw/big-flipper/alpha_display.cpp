#include <Arduino.h>
#include "alpha_display.h"

AlphaDisplay::AlphaDisplay(const AlphaDisplayFont* font) : _font(font), _connected(false), _seg_buffer(NULL), _disp_buffer(NULL), _props(NULL), _text_dir_rtl(false) {
	setUpdateMethod(0);							// Set default update method. 
};

AlphaDisplay::~AlphaDisplay() {
	end();
}

void AlphaDisplay::end() {
	free(_seg_buffer);
	free(_disp_buffer);
	free(_props);
}
bool AlphaDisplay::begin(bool connected) {
	_seg_buffer =  new uint8_t[getCharCount() * getCharBitmapByteSize()];
	_disp_buffer = new uint8_t[getCharCount() * getCharBitmapByteSize()];
	_props = new uint16_t[getPropertyCount()];
	_connected = connected;
	setProperty(PROP_UPDATE_INTERVAL, 50);
	clear();
	return isConnected();
}

bool AlphaDisplay::isConnected() const {
	return _connected && (NULL != _seg_buffer) && (NULL != _disp_buffer);
}

bool AlphaDisplay::is_update_timeout(uint16_t interval) {
	if (((uint16_t)millis() - _last_update) >= interval) {
		_last_update = millis();
		return true;
	}
	return false;
}

const uint8_t* AlphaDisplay::get_char_bitmap(char c) {
	if (NULL == _font) 
		return NULL;
	
	const char s_c = pgm_read_byte(&_font->start_char);
	if ((c < s_c) || (c > pgm_read_byte(&_font->end_char)))
		return NULL;

	return _font->bitmaps + (c - s_c) * pgm_read_byte(&_font->width) ;
}

// Display write methods. 

void AlphaDisplay::clear(uint8_t start, uint8_t end) {
	if (isConnected()) {
		if (start > getCharCount())
			start = getCharCount();
		if (end > getCharCount())
			end = getCharCount();
		do_clear(start, end);
	}
}

// DEfault clear method, maybe the subclass can do this more effiently...
void AlphaDisplay::do_clear(uint8_t start, uint8_t end) {
	memset(get_seg_buffer(start), 0, (end - start) * getCharBitmapByteSize());
	set_dirty();
}

void AlphaDisplay::write(const char* str, uint8_t pos) {
	while ('\0' != *str) 
		write_helper(*str++, pos++);
	set_dirty();
}
void  AlphaDisplay::write_helper(char c, uint8_t pos) {
	if (isConnected()) {
		const uint8_t* bitmap = get_char_bitmap(c);
		if (NULL != bitmap)		// Is there a char defined?
			memcpy_P(get_seg_buffer(pos), bitmap, getCharBitmapByteSize());
		else
			memset(get_seg_buffer(pos), 0, getCharBitmapByteSize());
	}
}
void  AlphaDisplay::write(char c, uint8_t pos) {
	if (pos < getCharCount()) {
		write_helper(c, pos);
		set_dirty();
	}
}
void  AlphaDisplay::writeBitmap(const uint8_t* bmp, uint8_t pos) {
	if (isConnected()) {
		if (pos < getCharCount()) {
			memcpy(get_seg_buffer(pos), bmp, getCharBitmapByteSize());
			set_dirty();
		}
	}
}
void  AlphaDisplay::writeBitmap_P(const uint8_t* bmp, uint8_t pos) {
	if (isConnected()) {
		if (pos < getCharCount()) {
			memcpy_P(get_seg_buffer(pos), bmp, getCharBitmapByteSize());
			set_dirty();
		}
	}
}
void AlphaDisplay::setUpdateMethod(uint8_t m) { 
	_update_mode.thread_func_idx = m;
	threadInit(&_update_mode.tcb); 
}

void AlphaDisplay::set_dirty() { 
	_dirty = true;
}
/*
bool AlphaDisplay::isUpdateDone() {
	return threadIsFinished(_update_mode.thread_rc);		// Check if thread flagged as running to completion. 
}
*/

uint8_t* AlphaDisplay::get_seg_buffer(uint8_t pos) {
	if (_text_dir_rtl)
		pos = getCharCount() - 1 - pos;
	return &_seg_buffer[pos * getCharBitmapByteSize()];
}
uint8_t* AlphaDisplay::get_disp_buffer(uint8_t pos) {
	return &_disp_buffer[pos * getCharBitmapByteSize()];
}

uint16_t AlphaDisplay::getProperty(uint8_t idx) { return (idx < getPropertyCount()) ? _props[idx] : 0; }
void AlphaDisplay::setProperty(uint8_t idx, uint16_t val) { if (idx < getPropertyCount()) _props[idx] = val; }

void AlphaDisplay::service() { 
	if (isConnected()) {
		if (is_update_timeout(getProperty(PROP_UPDATE_INTERVAL))) {
			const thread_t update_thread = get_update_thread();
			// ASSERT(NULL != update_thread);		// Look before you leap... 
			_update_mode.thread_rc = threadRun(&_update_mode.tcb, update_thread, (void*)this);
		}
	}
}
