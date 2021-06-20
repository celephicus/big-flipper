#include <Arduino.h>
#include <SD.h>

#include "project_config.h"
#include "debug.h"
#include "utils.h"
#include "regs.h"
#include "gpio.h"
#include "word_driver.h"

FILENUM(23);

typedef struct {
	char word[4];
	uint8_t count;
	uint8_t offset_link() { return sizeof(word) + 1; }	// word[4] + count[1] 
	uint8_t offset_next_word() { return offset_link() + count*2 + 2; }	// word[4] + count[1] + link[2]*count + terminator[2]
} word_def_t;

// SD card reader. We only read one file.
File wordDriverDataFd;
static void sd_card_init() {
	const bool sd_card_ok = SD.begin(GPIO_PIN_SD_CARD_NSEL);
	regsWriteMaskFlags(REGS_FLAGS_MASK_SD_CARD_INIT_FAIL, !sd_card_ok);
	if (sd_card_ok) {
		wordDriverDataFd = SD.open("FOO.TXT");
		regsWriteMaskFlags(REGS_FLAGS_MASK_SD_CARD_FILE_FAIL, !wordDriverDataFd);
	}
}

static uint16_t f_current_word_fpos;
static word_def_t f_current_word;
static uint16_t f_total_word_count;

static uint16_t get_total_word_count() {
	uint16_t count = 0;
	uint16_t pos = 0;
	
	while (1) {
		if (!wordDriverSetCurrentWord(pos))
			break;
		pos += f_current_word.offset_next_word();	
		count += 1;
	}
	return count;
}

void wordDriverInit() {
	sd_card_init();
	f_total_word_count = (regsGetFlags() & (REGS_FLAGS_MASK_SD_CARD_INIT_FAIL | REGS_FLAGS_MASK_SD_CARD_FILE_FAIL)) ? 0 : get_total_word_count();
}

bool wordDriverSetCurrentWord(uint16_t pos) {
	f_current_word_fpos = pos;
	if (!wordDriverDataFd.seek(f_current_word_fpos))
		return false;
	return (sizeof(f_current_word) == wordDriverDataFd.readBytes((char*)&f_current_word, sizeof(f_current_word)));
}

// This method is used to get a random word quickly by seeking to any valid position in the file, including the last character, and then reading _backwards_.
// If 0xff 0xff is read then the position following these is returned, unless it is 
// If position 0 is reached, then 0 is returned. 
bool wordDriverSetCurrentWordByIndex(uint16_t idx) {
	if (idx >= f_total_word_count)
		return false;
	
	uint16_t pos = 0;
	do {
		if (!wordDriverSetCurrentWord(pos))
			return false;
		pos += f_current_word.offset_next_word();
	} while (idx-- > 0);
	return true;
}

uint16_t wordDriverGetFpos() { return f_current_word_fpos; }

const char* wordDriverGetCurrentWordStr() {
	static char word[5];
	memcpy(word, f_current_word.word, 4);
	word[4] = '\0';
	return word;
}
uint8_t wordDriverGetLinkCount() { return f_current_word.count; }
	
uint16_t wordDriverGetLink(uint8_t idx) {
	if (idx > f_current_word.count)
		return 0xffff; 
	if (!wordDriverDataFd.seek(f_current_word_fpos + 4 + 1 + idx*2))
		return 0xffff;
	uint8_t link[2];
	if (sizeof(link) != wordDriverDataFd.readBytes((char*)&link, sizeof(link)))
		return 0xffff;
	return (uint16_t)link[0]<<8 | (uint16_t)link[1];
}

uint16_t wordDriverGetCount() { return f_total_word_count; }

