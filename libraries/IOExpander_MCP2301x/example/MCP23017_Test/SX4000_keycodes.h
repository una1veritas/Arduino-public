#ifndef _SX4000_KEYCODES_H_
#define _SX4000_KEYCODES_H_

#define NC          0x00
#define NC16        0xFF00
#define ERR	    0xBF /* �� */

#define XON         0x1013
#define XOFF        0x1011

// 0x1{1..4} in ASCII refers to special device control codes.  perfect.
#define SHIFT       0x11
#define CODE        0x12
#define ALT         0x14
// special case for shift lock
#define SHIFT_LOCK  0x13

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

#include <avr/progmem.h>
const PROGMEM uint8_t keycodes[] = {
// META: B0000
// 0          1           2           3           4           5           6           7
   ',',       '/',        '1',        '3',        '7',        '5',        '-',        '9',        // 0
   '.',       PLUSMINUS,  '2',        '4',        '8',        '6',        '=',        '0',        // 1
   HALF,      'q',        'e',        't',        'o',        'u',        'n',        'v',        // 2
   ';',       'z',        'f',        'h',        's',        'k',        'x',        'c',        // 3
   ']',       'w',        'r',        'y',        'p',        'i',        'm',        'b',        // 4
   '\'',      'a',        'g',        'j',        'd',        'l',        _SWITCH,    '\t',       // 5
   ' ',       '\n',       W_OUT,      RELOC,      ALT,        '\b',       CORRECT,    CODE,       // 6
   SHIFT,     SHIFT_LOCK, NC,         NC,         NC,         NC,         NC,         NC,         // 7
};

#endif

