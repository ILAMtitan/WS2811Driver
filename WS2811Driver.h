/**
 *  WS2811Driver.h - driver class implementation for the ws2811 rgb led controller IC.
 *  Supports MSP430G2/MSP430F5529 at 16MHz/25MHz and MSP432P401R LaunchPads at 48MHz.
 *
 *  Copyright (C) 2020  Shuyang Zhong
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
 *  Sections of code for MSP430 are forked from 
 *  https://github.com/ILAMtitan/WS2811Driver
 *  with thanks to oPossum and Rickta59 from 43oh.com
 *
 *  Sections of code used from the AdaFruit NeoPixel library:
 *
 *  Written by Phil Burgess / Paint Your Dragon for Adafruit Industries,
 *  contributions by PJRC and other members of the open source community.

 *  Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products
 *  from Adafruit!
 *
 *  06-14-2020
 */

#ifndef WS2811DRIVER_H_
#define WS2811DRIVER_H_

#if defined (__MSP430G2553__) || defined (__MSP430F5529__)
#include <msp430.h>
#include <stdint.h>
#include "ws2811.h"

#elif defined (__MSP432P401R__)
#include <msp.h>
#include <ti/drivers/gpio/GPIOMSP432.h>
#include <driverlib/gpio.h>

static const uint32_t GPIO_PORT_TO_BASE[] =
{   0x00,
    0x40004C02,  // P1OUT
    0x40004C03,  // P2OUT
    0x40004C22,  // P3OUT
    0x40004C23,
    0x40004C42,
    0x40004C43,
    0x40004C62,
    0x40004C63,
    0x40004C82,
    0x40004C83,
    0x40004D22
};

extern const GPIOMSP432_Config GPIOMSP432_config;
#define PIN_TO_PORT(pin) (GPIOMSP432_config.pinConfigs[pin] & 0xff00) >> 8
#define PIN_TO_BASEREG(pin) ((volatile uint32_t*)(GPIO_PORT_TO_BASE[(GPIOMSP432_config.pinConfigs[pin] & 0xff00) >> 8]))
#define PIN_TO_BITMASK(pin) (GPIOMSP432_config.pinConfigs[pin] & 0xff)
#define IO_REG_TYPE uint32_t
#define IO_REG_ASM
#define DIRECT_READ(base, mask)         (HWREG((uint32_t)base) & (mask) ? 1 : 0)
#define DIRECT_MODE_INPUT(base, mask)   (HWREG((uint32_t)base+4) &= ~(mask))
#define DIRECT_MODE_OUTPUT(base, mask)  (HWREG((uint32_t)base+4) |= (mask))
#define DIRECT_WRITE_LOW(base, mask)    (HWREG((uint32_t)base+2) &= ~(mask))
#define DIRECT_WRITE_HIGH(base, mask)   (HWREG((uint32_t)base+2) |= (mask))

#else
#error WS2811Driver : Incorrect hardware selected, must be MSP430G2, F5529 or MSP432P401R

#endif

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
