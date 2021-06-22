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

#define WORD_DRIVER_CONSOLE_COMMANDS																			\
	case /** W-SIZE **/ 0x3025: u_push(wordDriverDataFd.size()); break; 										\
	case /* WC */ 0x7a6e: u_push(wordDriverGetCount()); break; 													\
	case /* W-SET-POS */ 0xca7c: *u_tos() = wordDriverSetCurrentWord(*u_tos()); break;							\
	case /* W-SET-IDX */ 0x3345: *u_tos() = wordDriverSetCurrentWordByIndex(*u_tos());  break;					\
	case /* ?W */ 0x688d: 																						\
		{																										\
			const uint16_t fpos = wordDriverGetFpos();															\
			consolePrintValueStr(wordDriverGetCurrentWordStr());												\
			uint8_t link_count = wordDriverGetLinkCount();														\
			for (uint8_t i = 0; i < link_count; i += 1) { 														\
				wordDriverSetCurrentWord(fpos);																	\
				const uint16_t wp = wordDriverGetLink(i); 														\
				consolePrintValueUnsignedDecimal(wp); 															\
				if (wordDriverSetCurrentWord(wp))																\
					consolePrintValueStr(wordDriverGetCurrentWordStr());										\
				else																							\
					consolePrintValueStrProgmem(PSTR("----"));													\
			}																									\
			wordDriverSetCurrentWord(fpos);																		\
		}																										\
		break; 																									\

#endif //WORD_DRIVER_H__
