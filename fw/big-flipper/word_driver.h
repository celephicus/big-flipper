#ifndef WORD_DRIVER_H__
#define WORD_DRIVER_H__

// Data file.
#include <SD.h>
extern File wordDriverDataFd;

void wordDriverInit();

uint16_t wordDriverGetCount();

bool wordDriverSetCurrentWord(uint16_t pos);
uint16_t wordDriverGetFpos();
bool wordDriverSetCurrentWordByIndex(uint16_t idx);

const char* wordDriverGetCurrentWordStr();
uint8_t wordDriverGetLinkCount();
uint16_t wordDriverGetLink(uint8_t idx);

#endif //WORD_DRIVER_H__
