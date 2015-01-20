/**
 *  WS2811Driver.cpp - driver class implementation for the ws2811 rgb led controller IC.
 *  Supports MSP430G2 and MSP430F5529 Launchpads set t 16MHz.
 *
 *  Copyright (C) 2012  Rick Kimball rick@kimballsoftware.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Sections of code used from the AdaFruit NeoPixel library:
 *
 *  Written by Phil Burgess / Paint Your Dragon for Adafruit Industries,
 *  contributions by PJRC and other members of the open source community.

 *  Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products
 *  from Adafruit!
 *
 * 09-02-2014
 */

#ifndef WS2811DRIVER_H_
#define WS2811DRIVER_H_
#include <msp430.h>
#include <stdint.h>
#include "ws2811.h"

//Assembly call to strip update function
extern "C" void write_ws2811_hs_16(const uint8_t *data, uint16_t length, uint8_t pinmask, uint16_t portmask);
extern "C" void write_ws2811_hs_25(const uint8_t *data, uint16_t length, uint8_t pinmask, uint16_t portmask);

// 'type' flags for LED pixels (third parameter to constructor):
#define NEO_RGB     0x00 // Wired for RGB data order
#define NEO_GRB     0x01 // Wired for GRB data order
#define NEO_BRG     0x04  // Wired for BRG data order

class WS2811Driver {
private:
    uint16_t _led_cnt; // how many leds in the strip
    uint8_t _pin_mask; // bitmask for example, PORT P1.7 would be BIT7
	uint16_t _port_mask; // port mask
	uint8_t pin;
	uint8_t *pixels;   // pointer to array holding all color information
	uint8_t brightness;
	uint8_t rOffset,   // Index of red byte within each 3-byte pixel
		gOffset,       // Index of green byte
		bOffset;       // Index of blue byte

public:
    WS2811Driver(uint8_t n, uint8_t p, uint8_t t=NEO_GRB);
	~WS2811Driver();

    void begin();
    void end();
    void setPinMask(uint8_t mask) { _pin_mask = mask; }
	void show(void);
	void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
	void setPixelColor(uint16_t n, uint32_t c);
	uint32_t getPixelColor(uint16_t n);
	uint32_t Color(uint8_t r, uint8_t g, uint8_t b);
	uint16_t numPixels(void);
	void setBrightness(uint8_t);
	void setPin(uint8_t p);
};

#endif /* WS2811DRIVER_H_ */
