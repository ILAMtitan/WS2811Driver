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
 
#include "Energia.h"
#include "WS2811Driver.h"

extern "C" void disableWatchDog();
extern "C" void enableWatchDog();

/** @brief Initalizes a driver element
 *
 *  @param n The number of LEDs to be used in the strip
 *  @param t The type of pixels used and their element order
				NEO_RGB
				NEO_GRB
				NEO_BRG
 *  @return Void.
 */
WS2811Driver::WS2811Driver(uint8_t n, uint8_t t) : _led_cnt(n) ,_pin_mask(WS2811_BITMASK)
{
	//Allocate RAM for pixel buffer
	if((pixels = (uint8_t *)malloc(_led_cnt * 3))) {
		memset(pixels, 0, _led_cnt * 3);
	}
	if(t & NEO_GRB) { // GRB vs RGB; might add others if needed
		rOffset = 1;
		gOffset = 0;
		bOffset = 2;
	} else if (t & NEO_BRG) {
		rOffset = 1;
		gOffset = 2;
		bOffset = 0;
	} else {
		rOffset = 0;
		gOffset = 1;
		bOffset = 2;
	}
}

/** @brief Destroy the driver and the pixel allocation space
 *
 *  @return Void.
 */
WS2811Driver::~WS2811Driver() {
  if(pixels) free(pixels);
}

/** @brief Setup hardware for output.
 *
 *  @return Void.
 */
void WS2811Driver::begin()
{
    WS2811_PORTDIR |= _pin_mask;
}

/** @brief End hardware output.
 *
 *  @return Void.
 */
void WS2811Driver::end()
{
    WS2811_PORTDIR &= ~_pin_mask;
}

/** @brief Function to be used by application to update LED strip
 *
 *  @return Void.
 */
void WS2811Driver::show(void){
	disableWatchDog();
    write_ws2811_hs(pixels, _led_cnt * 3, _pin_mask);
    enableWatchDog();
}

/** @brief Set pixel color from separate R,G,B components
 *
 *  @param n Number of LEDs modules in the strip
 *  @param r 8bit red value
 *  @param g 8bit green value
 *  @param b 8bit blue value
 *
 *  @return Void.
 */
void WS2811Driver::setPixelColor(
 uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
    uint8_t *p = &pixels[n * 3];
    p[rOffset] = r;
    p[gOffset] = g;
    p[bOffset] = b;
}

/** @brief Set pixel color from 'packed' 32-bit RGB color
 *
 *  @param n LED in strip to update (indexed at 0)
 *  @param c 32bit packed color value
 *
 *  @return Void.
 */
void WS2811Driver::setPixelColor(uint16_t n, uint32_t c) {
  if(n < _led_cnt) {
    uint8_t
      r = (uint8_t)(c >> 16),
      g = (uint8_t)(c >>  8),
      b = (uint8_t)c;
    uint8_t *p = &pixels[n * 3];
    p[rOffset] = r;
    p[gOffset] = g;
    p[bOffset] = b;
  }
}

/** @brief Query color from previously-set pixel
 *
 *  @param n LED in strip to query (indexed at 0)
 *
 *  @return 32bit packed RGB value.
 */
uint32_t WS2811Driver::getPixelColor(uint16_t n) {

  if(n < _led_cnt) {
    uint8_t *p = &pixels[n * 3];
    return ((uint32_t)p[rOffset] << 16) |
           ((uint32_t)p[gOffset] <<  8) |
            (uint32_t)p[bOffset];
  }

  return 0; // Pixel # is out of bounds
}


/** @brief Convert separate R,G,B into packed 32-bit RGB color.
 *         Packed format is always RGB, regardless of LED strand color order.
 *
 *  @param r 8bit red value
 *  @param g 8bit green value
 *  @param b 8bit blue value
 *
 *  @return 32bit packed RGB value.
 */
uint32_t WS2811Driver::Color(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
}

/** @brief Fetches the set numbers of LEDs in the strip
 *
 *  @return number of LEDs in strip
 */
uint16_t WS2811Driver::numPixels(void) {
  return _led_cnt;
}