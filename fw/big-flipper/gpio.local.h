#ifndef GPIO_LOCAL_H__
#define GPIO_LOCAL_H__

/*	PINOUT FOR ATMEGA328P
    =====================
	
TXO/D1	  PD1	TXD/PCINT17             Input from external serial for bootloader
RXI/D0    PD0	RXD/PCINT16             Output to external serial for bootloader
D2        PD2	INT0/PCINT18            
D3        PD3	PCINT19/OC2B/INT1       PWM
D4        PD4	PCINT20/XCK/T0          Timer 0 used for Arduino tick. 
D5        PD5	PCINT21/OC0B/T1         PWM (Timer 0 used for Arduino tick)
D6        PD6	PCINT22/OC0A/AIN0       PWM (Timer 0 used for Arduino tick)
D7        PD7	PCINT23/AIN1            
D8        PB0	PCINT0/CLKO/ICP1        
D9        PB1	PCINT1/OC1A             PWM
D10       PB2	PCINT2/SS/OC1B          PWM
D11/MOSI  PB3	PCINT3/OC2A/MOSI        [PWM] INPUT during SPI programming. ISP/4
D12/MISO  PB4	PCINT4/MISO             OUTPUT during SPI programming.      ISP/1
D13/SCK   PB5	SCK/PCINT5              INPUT during SPI programming.       ISP/3
A0        PC0	ADC0/PCINT8             
A1        PC1	ADC1/PCINT9             
A2        PC2	ADC2/PCINT10            
A3        PC3	ADC3/PCINT11            
A4        PC4	ADC4/SDA/PCINT12        
A5        PC5	ADC5/SCL/PCINT13        
A6        ADC6	ADC6                    Analogue input only.
A7        ADC7	ADC7                    Analogue input only.

RST: ISP/5, GND: ISP/6, VCC: ISP/2.
*/

// Pin Assignments for ATMega328. 
enum {
	// Misc. 
	GPIO_PIN_DEBUG_LED = 			13,		// Yellow 'L' LED on board that we can't see.
	
	// Serial pins (not used directly).
	GPIO_PIN_CONSOLE_TXO = 			1,
	GPIO_PIN_CONSOLE_RXI = 			0,
	
	// SD Card.
	GPIO_PIN_SD_CARD_NSEL = 		A1,
	
	// Column Driver.
	GPIO_PIN_COL_EN = 				10,
	
	// Row drives.
	GPIO_PIN_ROW_0 =				8,
	GPIO_PIN_ROW_1 =				9,
	GPIO_PIN_ROW_2 =				2,
	GPIO_PIN_ROW_3 =				3,
	GPIO_PIN_ROW_4 =				4,
	GPIO_PIN_ROW_5 =				5,
	GPIO_PIN_ROW_6 =				6,
	GPIO_PIN_ROW_DATA =				7,
	
	// I2C interface (setup by Wire/TWI lib).
	GPIO_PIN_SDA = 					A4,
	GPIO_PIN_SCL = 					A5,

	// SPI interface (setup by SPI lib).
	GPIO_PIN_MOSI = 				11,
	GPIO_PIN_MISO = 				12,
	GPIO_PIN_SCK = 					13,
};

#define GPIO_PINS_UNUSED 			A0, A2, A3
							
// Direct pin access for Debug LEDs
#define GPIO_WANT_DEBUG_LED 0

// Serial ports.
#define GPIO_SERIAL_CONSOLE				 Serial

#endif	// GPIO_LOCAL_H__
