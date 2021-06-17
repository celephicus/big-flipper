#ifndef REGS_LOCAL_H__
#define REGS_LOCAL_H__

// This file is included by regs.h and then regs.h is included in most source files.
#ifndef REGS_H__
#error do not include this file directly, include regs.h instead.
#endif

// Define version of regs schema. If you change this file, increment the number to force any existing EEPROM DATA to flag as corrupt. 
#define REGS_VERSION 2

// Registers are defined with 3 parameters, name, default value, and a short comment that is turned into help text. 
#define REGS_DEFS(gen_)																														\
 gen_(FLAGS,						REGS_NO_DEFAULT,					"Various flags, see REGS_FLAGS_MASK_xxx")							\
 gen_(EEPROM_RC,					REGS_NO_DEFAULT,					"Last NV read status")												\
 gen_(RESTART,						REGS_NO_DEFAULT,					"Snapshot of mcusr in low byte, watchdog in high byte.")			\
 gen_(ADC_MON_PWR_VOLTS,			REGS_NO_DEFAULT,					"Raw ADC value from 24V input")										\
																																			\
 /* NV regs. */																																\
 gen_(ENABLES,						0,									"Enable flags")														\
 gen_(DISPLAY_UPDATE_INTERVAL,		50,									"Display update interval in ms")									\
 gen_(DISPLAY_UPDATE_METHOD,		0,									"Display update method, zero is default method.")					\
 gen_(FLIPDISK_PULSE_WIDTH,			500,								"Pulse width for flipdisks (us)")									\
 gen_(ANIMATION,					0,									"Animation index")													\
 gen_(ANIMATION_UPDATE_INTERVAL,	500,								"Animation update (ms)")											\
 
// Define the start of the NV regs. The region is from this index up to the end of the register array. 
#define REGS_START_NV_IDX REGS_IDX_ENABLES

// Define which regs to print in hex.
#define REGS_PRINT_HEX_MASK (_BV(REGS_IDX_FLAGS) | _BV(REGS_IDX_ENABLES))

// Flags are defined with 3 parameters, name, bit position and a short description that is turned into help text. 
#define REGS_FLAGS_DEFS(gen_)																							\
 /* Signal flags assigned up from bit 0. */																				\
																														\
 /* Debug flags. */																										\
																														\
 /* Fault flags, assigned down from bit 15. */																			\
gen_(SD_CARD_FILE_FAIL,	14,	"SD card data file not present")															\
gen_(SD_CARD_INIT_FAIL,	15,	"SD card failed to initialise")																\

// Enables are defined with 3 parameters, name, bit position and a short description that is turned into help text. 
#define REGS_ENABLES_DEFS(gen_)																							\
 gen_(DUMP_REGS,		0,	"Regs values dump to console")																\
 gen_(DUMP_REGS_FAST,	1,	"Dump at 5/s rather than 1/s")																\

// Generate FLAGS defs.
//

// Macro that generates enum items intialised to a mask for a set of bit flags, SO gen_(FLAGS, 99,	"Stuff") => 'REGS_FLAGS_MASK_FLAGS = (int)_BV(99),'. 
#define REGS_FLAGS_GEN_ENUM_MASK(name_, bit_, desc_) \
 REGS_FLAGS_MASK_##name_ = (int)_BV(bit_),

// Declare the flags enum.
enum {
	REGS_FLAGS_DEFS(REGS_FLAGS_GEN_ENUM_MASK)
};

// Macro that expands to a help string for the flag or enable, so gen_(FLAGS, 0,  "Stuff") =>  ==> "\r\n FLAGS: 99: ."
#define REGS_GENERIC_MASK_GEN_HELP_STR(name_, bit_, desc_) \
 "\r\n " #name_ ": " #bit_ " (" desc_ ".)"

// Generate ENABLES defs.
//

// Macro that generates enum items intialised to a mask for a set of bit flags, SO gen_(FLAGS, 99,	"Stuff") => 'REGS_ENABLES_MASK_FLAGS = (int)_BV(99),'. 
#define REGS_ENABLES_GEN_ENUM_MASK(name_, bit_, desc_) \
 REGS_ENABLES_MASK_##name_ = (int)_BV(bit_),

// Declare the enables enum.
enum {
	REGS_ENABLES_DEFS(REGS_ENABLES_GEN_ENUM_MASK)
};

// Generate help string for flags & enables.
#define REGS_EXTRA_HELP_STR "\r\nFlags:" REGS_FLAGS_DEFS(REGS_GENERIC_MASK_GEN_HELP_STR) "\r\nEnables:" REGS_ENABLES_DEFS(REGS_GENERIC_MASK_GEN_HELP_STR)

#endif // REGS_LOCAL_H__


