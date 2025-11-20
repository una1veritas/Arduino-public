/*
 * LCD_display.cpp
 *
 *  Created on: 2025/10/19
 *      Author: sin
 */
#include <Arduino.h>
#include <LiquidCrystal.h>

struct Terminal {
  const LiquidCrystal & lcd;
  char line_buf[4][24];
  int row, col;

  Terminal(const LiquidCrystal & _lcd) : lcd(_lcd) {
    lcd.begin(20,4);
    lcd.clear();
    col = 0;
    row = 0;
  }

  void print(uint8_t r, uint8_t c, const char * str) {
    row = r % 4;
    snprintf(line_buf[row], 21, "%-20s", str);
    //line_buf[row][20] = '\0';
    lcd.setCursor(0,row);
    lcd.print(line_buf[row]);
  }

  void clear() {
    lcd.clear();
    row = 0;
    col = 0;
  }
};

struct DFR7segarray {
  const static uint8_t numeric7[10] PROGMEM;
  const static uint8_t alpha7[26] PROGMEM;

  static byte ascii7seg(char ch) {
    if ('0' <= ch and ch <= '9') {
      return pgm_read_byte_near(numeric7 + ch - '0');
    }
    switch (ch) {
      case '.':
      case ',':
        return 0x7f;
        break;
      case ' ':
        return 0xff;
        break;
      case 'i':
        return 0x7b;
        break;
      case 'o':
        return 0xa3;
        break;
      case 'u':
        return 0xe3;
        break;
    }
    ch = toupper(ch);
    if ('A' <= ch and ch <= 'Z') {
      return pgm_read_byte_near(alpha7 + ch - 'A');
    }
    return 0xff;
  }

  uint8_t pin_clk, pin_data, pin_latch;

  DFR7segarray(uint8_t clk, uint8_t data, uint8_t latch) :
  pin_clk(clk), pin_data(data), pin_latch(latch) {
    pinMode(pin_clk, OUTPUT);
    pinMode(pin_data, OUTPUT);
    pinMode(pin_latch, OUTPUT);
    digitalWrite(pin_latch, HIGH);
  }

  void clear(const uint8_t c =' ') const {
    digitalWrite(pin_latch, LOW);
    for (int i = 0; i < 8; ++i) {
      shiftOut(pin_data, pin_clk, MSBFIRST, ascii7seg(c));
    }
    digitalWrite(pin_latch, HIGH);
  }

  void show_str(const char * str) {
    digitalWrite(pin_latch, LOW);
    for (int i = 8; i > 0;) {
      shiftOut(pin_data, pin_clk, MSBFIRST, ascii7seg(str[--i]));
    }
    digitalWrite(pin_latch, HIGH);
  }

  void show_digits(uint16_t addr, uint8_t data, char busmode) {
    char buf[10];
    if (busmode == 'o' or busmode == 'i') {
      addr &= 0xff;
      snprintf(buf, 9, "  %02X%c %02X", addr, busmode, data);
    } else {
      snprintf(buf, 9, "%04X%c %02X", addr, busmode, data);
    }
    digitalWrite(pin_latch, LOW);
    for (int i = 8; i > 0;) {
      shiftOut(pin_data, pin_clk, MSBFIRST, ascii7seg(buf[--i]));
    }
    digitalWrite(pin_latch, HIGH);
  }
};


