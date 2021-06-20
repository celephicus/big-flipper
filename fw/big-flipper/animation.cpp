#include <Arduino.h>

#include "project_config.h"
#include "debug.h"
FILENUM(25);

#include "driver.h"		// Includes header for display.
#include "utils.h"
#include "word_driver.h"
#include "thread.h"
#include "animation.h"


// NULL thread, does nothing.
int8_t thread_null(void* arg) {
	(void)arg;
	THREAD_BEGIN();
	THREAD_END();
}

// Simple animation thread. 
int8_t thread_test_pattern(void* arg) {
	(void)arg;
	
	static const uint8_t IMAGES[][5] PROGMEM = {
		{ 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x01, 0x00, 0x00, 0x00, 0x00 },
		{ 0x03, 0x01, 0x00, 0x00, 0x00 },
		{ 0x07, 0x03, 0x01, 0x00, 0x00 },
		{ 0x0f, 0x07, 0x03, 0x01, 0x00 },
		{ 0x1f, 0x0f, 0x07, 0x03, 0x01 },
		{ 0x3f, 0x1f, 0x0f, 0x07, 0x03 },
		{ 0x7f, 0x3f, 0x1f, 0x0f, 0x07 },
		{ 0x7f, 0x7f, 0x3f, 0x1f, 0x0f },
		{ 0x7f, 0x7f, 0x7f, 0x3f, 0x1f },
		{ 0x7f, 0x7f, 0x7f, 0x7f, 0x3f },
		{ 0x7f, 0x7f, 0x7f, 0x7f, 0x7f },
		{ 0x7f, 0x7f, 0x7f, 0x7f, 0x7f },
		{ 0x7f, 0x7f, 0x7f, 0x7f, 0x7f },
		{ 0x7f, 0x7f, 0x7f, 0x7f, 0x3f },
		{ 0x7f, 0x7f, 0x7f, 0x3f, 0x1f },
		{ 0x7f, 0x7f, 0x3f, 0x1f, 0x0f },
		{ 0x7f, 0x3f, 0x1f, 0x0f, 0x07 },
		{ 0x3f, 0x1f, 0x0f, 0x07, 0x03 },
		{ 0x1f, 0x0f, 0x07, 0x03, 0x01 },
		{ 0x0f, 0x07, 0x03, 0x01, 0x00 },
		{ 0x07, 0x03, 0x01, 0x00, 0x00 },
		{ 0x03, 0x01, 0x00, 0x00, 0x00 },
		{ 0x01, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0x00, 0x00 },
	};
	
	THREAD_BEGIN();
	while (1) {
		static uint8_t i; 
		for (i = 0; i < ELEMENT_COUNT(IMAGES); i += 1) {
			forj (driverGetDisplay().getCharCount())
				driverGetDisplay().writeBitmap_P(IMAGES[i], j);
			THREAD_YIELD();
		}
	}
	THREAD_END();
}

int8_t thread_all_set_clear(void* arg) {
	(void)arg;
	
	THREAD_BEGIN();
	while (1) {
		static uint8_t d; 
		uint8_t bitmap[5];
		memset(bitmap, d, sizeof(bitmap));			// Build character.
		fori (driverGetDisplay().getCharCount()) 
			driverGetDisplay().writeBitmap(bitmap, i);
		d ^= 0x7f;			// Flip all bits.
		THREAD_YIELD();
	}
	THREAD_END();
}

int8_t thread_words(void* arg) {
	(void)arg;
	
	THREAD_BEGIN();
	if (wordDriverGetCount() <= 0)
		THREAD_EXIT(1);
	
	wordDriverSetCurrentWordByIndex(0); // random(wordDriverGetCount());
	while (1) {
		driverGetDisplay().write(wordDriverGetCurrentWordStr());
		const uint8_t link_idx = random(wordDriverGetLinkCount());
		const uint16_t wp = wordDriverGetLink(link_idx);
		//eventPublish(EV_NEW_WORD, link_idx, wp);
		wordDriverSetCurrentWord(wp);
		THREAD_YIELD();
	}
	THREAD_END();
}

static const thread_t ANIMATION_THREADS[] PROGMEM = {
	thread_null, 
	thread_test_pattern,
	thread_all_set_clear,
	thread_words,
};

static uint8_t f_animation_idx;
thread_control_t f_animation_tcb;

void animation_set(uint8_t idx) {
	const uint8_t new_idx = (idx >= ELEMENT_COUNT(ANIMATION_THREADS)) ? 0 : idx;
	if (new_idx != f_animation_idx) {
		f_animation_idx = new_idx;
		animation_reset();
		driverGetDisplay().clear();
	}
}

void animation_reset() { 
	threadInit(&f_animation_tcb);
}

void animation_service() { 
	threadRun(&f_animation_tcb, (thread_t)pgm_read_word(&ANIMATION_THREADS[f_animation_idx]), NULL);
}
