#ifndef	CONFIG_H
#define	CONFIG_H

// ===================================================================
// === Board detection ===============================================
// ===================================================================

#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)

// Arduino Diecimila, Duemilanove or Uno

#define	P_MOSI	    B,3
#define	P_MISO	    B,4
#define	P_SCK	    B,5

#define	MCP2515_CS	B,2
#define	MCP2515_INT	D,2

#define CAN_INT     0

#elif defined(__AVR_ATmega32U4__)

// Arduino Leonardo

#define	P_MOSI	    B,2
#define	P_MISO	    B,3
#define	P_SCK	    B,1

#define	MCP2515_CS	B,6
#define	MCP2515_INT	D,1

#define CAN_INT     1

#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

// Arduino Mega 1280 or Mega 2560

#define	P_MOSI	    B,2
#define	P_MISO	    B,3
#define	P_SCK	    B,1

#define	MCP2515_CS	B,4
#define	MCP2515_INT E,4

#define CAN_INT     0

#else

// Unsupported board. Please find out which controller
// is driving your Arduino board and define the following
// values:

#define	P_MOSI	    B,2 // ICSP MOSI pin
#define	P_MISO	    B,3 // ICSP MISO pin
#define	P_SCK	    B,1 // ICSP SCK pin

#define	MCP2515_CS	B,4 // Arduino pin 9
#define	MCP2515_INT E,4 // Arduino pin 2

#define CAN_INT     0   // Pin 2 interrupt

#error Unsupported board. Please adjust library.

#endif

#endif	// CONFIG_H
