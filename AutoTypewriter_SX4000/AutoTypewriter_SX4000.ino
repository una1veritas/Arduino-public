#include <ctype.h>
#include <avr/pgmspace.h>

#include <SPI.h>

#include <MCP23S1x.h>

#include <Tools.h>

//#include "keycodes.h"

#define SCAN_BIT(x)  (1<<(x & 0x07))
#define SIGNAL_BIT(x) (1<<(x >> 4 & 0x07))

boolean scan_catch(const uint8_t sbits, const uint16_t comp, long wait = 20);
boolean modifierkey(const uint16_t kcode, long wait = 20);
boolean typekey(const uint16_t kcode, long wait = 20);

MCP23S1x iox(10);

static const uint16_t KEYCODE_NOKEY = 0x00ff;
static const uint16_t KEYCODE_SHIFT = ((uint16_t)1)<<8;
static const uint16_t KEYCODE_CODE = ((uint16_t)1)<<9;
static const uint16_t KEYCODE_ALT = ((uint16_t)1)<<10;
static const uint16_t KEYCODE_MODIFIERS = KEYCODE_SHIFT | KEYCODE_CODE | KEYCODE_ALT;
static const uint16_t KEYCODE_MATRIXMASK = 0xff;

static const uint8_t MATRIX_SHIFT = 0x07;
static const uint8_t MATRIX_ALT = 0x46;
static const uint8_t MATRIX_CORRECT = 0x66;
static const uint8_t MATRIX_CODE = 0x76;


enum {
	NC16	= 0xff00,
	XON 	= 0x1013,
	XOFF	= 0x1011,
};

#define NC          0x00
#define ERR			0xBF /* �� */

// from the ISO-8859-1 Character Set
#define CENT       0xa2  /* �� */
#define SECTION    0xa7  /* �� */
#define DEGREE     0xb0  /* �� */
#define PLUSMINUS  0xb1  /* �� */
#define SQUARE     0xb2  /* �� */
#define CUBE       0xb3  /* �� */
#define PARAGRAPH  0xb6  /* �� */
#define QUARTER    0xbc  /* �� */
#define HALF       0xbd  /* �� */

#define EXPR        0xC0
#define RELOC       0xC1
#define CORRECT     0xC2
#define W_OUT       0xC3
#define BSHALF      0xC4
#define L_OUT       0xC5
#define RESET       0xC6
#define _SWITCH     0xC7
#define REV         0xC8
#define INDEX       0xC9
#define P_INS       0xCA
#define VIEW        0xCB
#define _DEMO       0xCC
#define W_SPELL     0xCD
#define PITCH       0xCE
#define LINE        0xCF
#define M_REL       0xD0
#define L_MAR       0xD1
#define R_MAR       0xD2
#define T_SET       0xD3
#define DT_SET      0xD4
#define T_CLR       0xD5
#define CTR         0xD6
#define RMF         0xD7
#define UNDLN       0xD8
#define BOLD        0xD9
#define OPERATE     0xDA

uint16_t code_table[] PROGMEM = {
		// 0xMETA SIGNAL SCAN
		0x006, /*(sp)*/   0x120, /* ! */    0x105, /* " */    0x130, /* # */    0x131, /* $ */    0x150, /* % */    0x140, /* & */    0x005, /* ' */
		0x170, /* ( */    0x171, /* ) */    0x141, /* * */    0x161, /* + */    0x000, /* , */    0x060, /* - */    0x001, /* . */    0x010, /* / */
		0x071, /* 0 */    0x020, /* 1 */    0x021, /* 2 */    0x030, /* 3 */    0x031, /* 4 */    0x050, /* 5 */    0x051, /* 6 */    0x040, /* 7 */
		0x041, /* 8 */    0x070, /* 8 */    0x103, /* : */    0x003, /* ; */    0x214, /* < */    0x061, /* = */    0x252, /* > */    0x110, /* ? */
		0x121, /* @ */    0x115, /* A */    0x174, /* B */    0x173, /* C */    0x145, /* D */    0x122, /* E */    0x123, /* F */    0x125, /* G */
		0x133, /* H */    0x154, /* I */    0x135, /* J */    0x153, /* K */    0x155, /* L */    0x164, /* M */    0x162, /* N */    0x142, /* O */
		0x144, /* P */    0x112, /* Q */    0x124, /* R */    0x143, /* S */    0x132, /* T */    0x152, /* U */    0x172, /* V */    0x114, /* W */
		0x163, /* X */    0x134, /* Y */    0x113, /* Z */    0x104, /* [ */    NC16, /* \ */     0x004, /* ] */    0x011, /*^(pm)*/  0x160, /* _ */
		0x111,/* `(deg) */0x015, /* a */    0x074, /* b */    0x073, /* c */    0x045, /* d */    0x022, /* e */    0x023, /* f */    0x025, /* g */
		0x033, /* h */    0x054, /* i */    0x035, /* j */    0x053, /* k */    0x055, /* l */    0x064, /* m */    0x062, /* n */    0x042, /* o */
		0x044, /* p */    0x012, /* q */    0x024, /* r */    0x043, /* s */    0x032, /* t */    0x052, /* u */    0x072, /* v */    0x014, /* w */
		0x063, /* x */    0x034, /* y */    0x013, /* z */    0x002, /*{(hf)*/  0x213, /*|(sect)*/0x102, /*}(qt)*/  NC16, /* ~ */     NC16, /*(del)*/
};

uint16_t ASCIIToKeycode(const unsigned char c) {
	uint16_t keycode = 0;
	if ( c < 0x20 ) {
		switch(c) {
		case 0x07: // BELL
			keycode = 0x723; // unknown
			break;
		case 0x08: // BS
			keycode = 0x066; // CORRECT
			break;
		case 0x09: // TAB
			keycode = 0x075;
			break;
		case 0x0a: //nl
		case 0x0d: //cr
			keycode = 0x016;
			break;
		case 0x0b: // V tab
			keycode = 0x244; // INDEX
			break;
		case 0x0c: // form feed
			keycode = 0x215; // P_INS; new paper
			break;
		default:
			keycode = 0;
			break;
		}
	} else {
		keycode = pgm_read_word(code_table + (c - 0x20));
	}
	return keycode;
}


word keyPressed(const byte detect, const byte scan);

void setup() {
  Serial.begin(19200);

  SPI.begin();
  iox.begin();
  iox.portAMode(iox.IODIR_INPUT);
  iox.portBMode(iox.IODIR_INPUT);

  Serial.println("Hello.");
  /*
  byte val = ioxt.read(MCP2301x::IOCON);
   val &= ~(0x01<<6);
   ioxt.write(MCP23Sxx::IOCON, val);
   ioxt.write(MCP23Sxx::GPINTENB, 0x00);
   ioxt.write(MCP23Sxx::INTCONB, 0x00);
   pinMode(14, INPUT);
   digitalWrite(14, HIGH);
   */
}

void loop() {
  long watch;
  char c;
  int count = 0;
  uint8_t scan, signal, t;
  uint16_t keycode = KEYCODE_NOKEY;

  if ( Serial.available() ) {
	c = (char) Serial.read();
    keycode = ASCIIToKeycode(c);

    Serial.print("Typing ");
    printByte(Serial, keycode>>8);
    Serial.print('.');
    printByte(Serial, keycode & 0xff);

    // shift/modifiers in
    modifierkey(keycode);
    typekey(keycode);
    modifierkey(keycode);
    typekey(keycode);
    modifierkey(keycode);

    Serial.println();
    delay(150);
  }

}

void nop10(uint16_t n) {
	for ( ; n ; n--) {
		asm volatile ("NOP");
		asm volatile ("NOP");
		asm volatile ("NOP");
		asm volatile ("NOP");
		asm volatile ("NOP");
		asm volatile ("NOP");
		asm volatile ("NOP");
		asm volatile ("NOP");
		asm volatile ("NOP");
	}
}


boolean scan_catch(const uint8_t sbits, const uint16_t comp, long wait) {
	uint16_t count = 0;

	wait += millis();
	if ( iox.read(iox.GPIOB) == sbits )
		count++;
	while ( count < comp ) {
		nop10(32);
		if ( millis() > wait )
			break;
		if ( iox.read(iox.GPIOB) == sbits ) {
			count++;
		} else {
			count = 0;
		}
	}
	if ( count < comp )
		return false;
	else
		return true;
}

boolean modifierkey(const uint16_t keycode, long wait) {
    switch ( keycode & KEYCODE_MODIFIERS ) {
    case KEYCODE_SHIFT:
    	typekey(MATRIX_SHIFT);
    	break;
    case KEYCODE_CODE:
    	typekey(MATRIX_CODE);
    	break;
    case KEYCODE_ALT:
    	typekey(MATRIX_ALT);
    	break;
    }
}

boolean typekey(const uint16_t keycode, long wait) {
	uint16_t count = 0;
	uint8_t nscan = ~SCAN_BIT(keycode);
	uint8_t nsig = ~SIGNAL_BIT(keycode);

	if ( !scan_catch(~(SCAN_BIT(keycode)>>1), 32, wait) )
		return false;

	if ( scan_catch(nscan, 2, wait) ) {
		iox.write(iox.OLATA, nsig);
		iox.portAMode(iox.IODIR_OUTPUT);
		delayMicroseconds(1500);
		wait += millis();
		while ( iox.read(iox.GPIOB) == nscan ) {
			nop10(10);
			if ( millis() > wait)
				break;
		}
		iox.write(iox.GPIOA, 0xff);
		iox.portAMode(iox.IODIR_INPUT);
	}

	return true;
}

word keyPressed(const byte scan, const byte detect) {
  word code = 0;
  byte val;
  val = ~scan;
  switch (val) {
  case 0x01:
    code = 0 << 4;
    break;
  case 0x02:
    code = 1 << 4;
    break;
  case 0x04:
    code = 2 << 4;
    break;
  case 0x08:
    code = 3 << 4;
    break;
  case 0x10:
    code = 4 << 4;
    break;
  case 0x20:
    code = 5 << 4;
    break;
  case 0x40:
    code = 6 << 4;
    break;
  case 0x80:
    code = 7 << 4;
    break;
  default:
    code = 8 << 4;
    break;
  }
  val = ~detect;
  switch (val) {
  case 1:
    code |= 0;
    break;
  case 2:
    code |= 1;
    break;
  case 4:
    code |= 2;
    break;
  case 8:
    code |= 3;
    break;
  case 16:
    code |= 4;
    break;
  case 32:
    code |= 5;
    break;
  case 64:
    code |= 6;
    break;
  case 128:
    code |= 7;
    break;
  default:
    code |= 8;
    break;
  }
  return code;
}



