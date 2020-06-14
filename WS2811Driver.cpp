/**
 *  WS2811Driver.cpp - driver class implementation for the ws2811 rgb led controller IC.
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
 
#include "Energia.h"
#include "WS2811Driver.h"

extern "C" void disableWatchDog();
extern "C" void enableWatchDog();

/**
 * ws2811 high speed write routine for rgb controller for 16MHz MSP430
 * 
 * void write_ws2811_hs_16(uint8_t *data, uint16_t length, uint8_t pinmask, uint16_t portmask);
 *                         data - R15,    length - R14,    pinmask - R13    portmask - R12
 *
 * NOTE: assumes interrupts are disabled as cycle counts are used for timing
 *
 * --- High Speed Mode 800KHz
 *        High / Low us   High / Low cycles @ 16 MHz
 * Zero:  0.25 / 1.00        4 / 16
 * One:   0.60 / 0.65        9 / 11
 * Reset:    0 / 50+         0 / 800+
 *
 */
#if defined (__MSP430G2553__) || defined (__MSP430F5529__)
void write_ws2811_hs_16(uint8_t *data, uint16_t length, uint8_t pinmask, uint16_t portmask)
{
    asm volatile(
      "write_ws2811_hs_16:"             "\n\t"
      "push    R10"                     "\n\t"
      "push    R11"                     "\n\t"
      "byte_loop_hs:"                   "\n\t"
      "mov     #7, R10"                 "\n\t"
      "mov.b   @R15+, R11"              "\n\t"
      "bit_loop_hs:"                    "\n\t"
      "rla.b   R11"                     "\n\t"
      "jc      one_hs"                  "\n\t"
      "bis.b   R13, @R12"               "\n\t"
      "bic.b   R13, @R12"               "\n\t"
      "bic.b   R13, @R12"               "\n\t"
      "jmp     next_bit_hs"             "\n\t"
      "one_hs:"                         "\n\t"
      "bis.b   R13, @R12"               "\n\t"
      "bis.b   R13, @R12"               "\n\t"
      "jmp     $ + 2"                   "\n\t"
      "bic.b   R13, @R12"               "\n\t"
      "next_bit_hs:"                    "\n\t"
      "dec     R10"                     "\n\t"
      "jne     bit_loop_hs"             "\n\t"
      "rla.b   R11"                     "\n\t"
      "jc      last_one_hs"             "\n\t"
      "bis.b   R13, @R12"               "\n\t"
      "bic.b   R13, @R12"               "\n\t"
      "jmp     $ + 2"                   "\n\t"
      "dec     R14"                     "\n\t"
      "jne     byte_loop_hs"            "\n\t"
      "jmp     reset_hs"                "\n\t"
      "last_one_hs:"                    "\n\t"
      "bis.b   R13, @R12"               "\n\t"
      "jmp     $ + 2"                   "\n\t"
      "mov     #7, R10"                 "\n\t"
      "mov.b   @R15+, R11"              "\n\t"
      "bic.b   R13, @R12"               "\n\t"
      "dec     R14"                     "\n\t"
      "jne     bit_loop_hs"             "\n\t"
      "reset_hs:"                       "\n\t"
      "mov     #800 / 3, R11"           "\n\t"
      "dec     R11"                     "\n\t"
      "jne     $ - 2"                   "\n\t"
      "pop     R11"                     "\n\t"
      "pop     R10"                     "\n\t"
      "ret"                             "\n\t"
    );
}

/**
 * ws2811 high speed write routine for rgb controller for 25MHz MSP430
 * 
 * void write_ws2811_hs_25(uint8_t *data, uint16_t length, uint8_t pinmask, uint16_t portmask);
 *                         data - R15,    length - R14,    pinmask - R13    portmask - R12
 *
 * NOTE: assumes interrupts are disabled as cycle counts are used for timing
 *
 * --- High Speed Mode 800KHz
 *        High / Low us   High / Low cycles @ 25 MHz
 * Zero:  0.25 / 1.00        6.25 / 25
 * One:   0.60 / 0.65        15 / 16.25
 * Reset:    0 / 50+         0 / 800+
 *
 */
void write_ws2811_hs_25(uint8_t *data, uint16_t length, uint8_t pinmask, uint16_t portmask)
{
    asm volatile(
	  "write_ws2811_hs_25:"             "\n\t"
      "push    R10"                     "\n\t"
      "push    R11"                     "\n\t"
      "byte_loop_hs_25:"                   "\n\t"
      "mov     #7, R10"                 "\n\t"
      "mov.b   @R15+, R11"              "\n\t"
      "bit_loop_hs_25:"                    "\n\t"
      "rla.b   R11"                     "\n\t"
      "jc      one_hs_25"                  "\n\t"
      "bis.b   R13, @R12"    			"\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
      "bic.b   R13, @R12"    			"\n\t"
      "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
      "jmp     next_bit_hs_25"             "\n\t"
      "one_hs_25:"                         "\n\t"
      "bis.b   R13, @R12"    			"\n\t"
      "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
      "bic.b   R13, @R12"    			"\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
      "next_bit_hs_25:"                    "\n\t"
      "dec     R10"                     "\n\t"
      "jne     bit_loop_hs_25"             "\n\t"
      "rla.b   R11"                     "\n\t"
      "jc      last_one_hs_25"             "\n\t"
      "bis.b   R13, @R12"   			"\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
      "bic.b   R13, @R12"    			"\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
      "dec     R14"                     "\n\t"
      "jne     byte_loop_hs_25"            "\n\t"
      "jmp     reset_hs_25"                "\n\t"
      "last_one_hs_25:"                    "\n\t"
      "bis.b   R13, @R12"    			"\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
      "mov     #7, R10"                 "\n\t"
      "mov.b   @R15+, R11"              "\n\t"
      "bic.b   R13, @R12"    			"\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
	  "nop"                             "\n\t"
      "dec     R14"                     "\n\t"
      "jne     bit_loop_hs_25"             "\n\t"
      "reset_hs_25:"                       "\n\t"
      "mov     #1250 / 3, R11"          "\n\t"
      "dec     R11"                     "\n\t"
      "jne     $ - 2"                   "\n\t"
      "pop     R11"                     "\n\t"
      "pop     R10"                     "\n\t"
	  "ret"                             "\n\t"
    );
}
#endif

/** @brief Initalizes a driver element
 *
 *  @param n The number of LEDs to be used in the strip
 *  @param t The type of pixels used and their element order
				NEO_RGB
				NEO_GRB
				NEO_BRG
 *  @return Void.
 */
WS2811Driver::WS2811Driver(uint8_t n, uint8_t p, uint8_t t) : _led_cnt(n) , pin(p)
{
#if defined (__MSP430G2553__) || defined (__MSP430F5529__)
    _pin_mask = digitalPinToBitMask(p);
	_port_mask = digitalPinToPort(p);
#endif
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
  pinMode(pin, INPUT);
  if(pixels) free(pixels);
}

/** @brief Setup hardware for output.
 *
 *  @return Void.
 */
void WS2811Driver::begin()
{
	pinMode(pin, OUTPUT);
	digitalWrite(pin, LOW);

#if defined (__MSP430G2553__) || defined (__MSP430F5529__)
	_port_mask = (uint16_t)portOutputRegister(_port_mask);
#endif
}

/** @brief End hardware output.
 *
 *  @return Void.
 */
void WS2811Driver::end()
{
	pinMode(pin, INPUT);
}

/** @brief Function to be used by application to update LED strip
 *
 *  @return Void.
 */
void WS2811Driver::show(void){
// Begin of support for MSP430 based boards  -------------------------
#if defined (__MSP430G2553__) || defined (__MSP430F5529__)
	disableWatchDog();
#if F_CPU == 16000000L
    write_ws2811_hs_16(pixels, _led_cnt * 3, _pin_mask, _port_mask);
#elif F_CPU == 25000000L
	write_ws2811_hs_25(pixels, _led_cnt * 3, _pin_mask, _port_mask);
#else
#error WS2811Driver : Unsupported clock frequency, must be G2/F5529 at 16/25MHz or MSP432P401R at 48MHz
#endif
    enableWatchDog();
// End of support for MSP430 based boards  -------------------------

// Begin of support for MSP432 based boards  -------------------------
#elif defined (__MSP432P401R__)
#include <stddef.h>
#if F_CPU == 48000000
  uint8_t          *p   = pixels,
                    pix, count, dly, portout,
                    bitmask = PIN_TO_BITMASK(pin);
  volatile uint32_t *reg = PIN_TO_BASEREG(pin);
  uint32_t          num = _led_cnt*3;  
  asm volatile(
    "L%=_begin:"                                      "\n\t"
    "ldrb   %[pix], [%[p], #0]"                       "\n\t"
    "lsl    %[pix], #24"                              "\n\t"
    "movs   %[count], #7"                             "\n\t"
    "L%=_loop:"                                       "\n\t"
    "lsls   %[pix], #1"                               "\n\t"
    "bcs    L%=_loop_one"                             "\n\t"
    "L%=_loop_zero:"                                  "\n\t"
    "ldrb   %[portout], [%[reg], #0]"                 "\n\t"
    "orr    %[portout], %[portout], %[bitmask]"       "\n\t"
    "strb   %[portout], [%[reg], #0]"                 "\n\t"
    "movs   %[dly], #1"                               "\n\t"
    "L%=_loop_delay_T0H:"                             "\n\t"
    "subs   %[dly], #1"                               "\n\t"
    "bne    L%=_loop_delay_T0H"                       "\n\t"
    "ldrb   %[portout], [%[reg], #0]"                 "\n\t"
    "bic    %[portout], %[portout], %[bitmask]"       "\n\t"
    "strb   %[portout], [%[reg], #0]"                 "\n\t"
    "movs   %[dly], #7"                               "\n\t"
    "L%=_loop_delay_T0L:"                             "\n\t"
    "subs   %[dly], #1"                               "\n\t"
    "bne    L%=_loop_delay_T0L"                       "\n\t"
    "b  L%=_next"                                     "\n\t"
    "L%=_loop_one:"                                   "\n\t"
    "ldrb   %[portout], [%[reg], #0]"                 "\n\t"
    "orr    %[portout], %[portout], %[bitmask]"       "\n\t"
    "strb   %[portout], [%[reg], #0]"                 "\n\t"
    "movs   %[dly], #5"                               "\n\t"
    "L%=_loop_delay_T1H:"                             "\n\t"
    "subs   %[dly], #1"                               "\n\t"
    "bne    L%=_loop_delay_T1H"                       "\n\t"
    "ldrb   %[portout], [%[reg], #0]"                 "\n\t"
    "bic    %[portout], %[portout], %[bitmask]"       "\n\t"
    "strb   %[portout], [%[reg], #0]"                 "\n\t"
    "movs   %[dly], #3"                               "\n\t"
    "L%=_loop_delay_T1L:"                             "\n\t"
    "subs   %[dly], #1"                               "\n\t"
    "bne    L%=_loop_delay_T1L"                       "\n\t"
    "nop"                                             "\n\t"
    "L%=_next:"                                       "\n\t"
    "subs   %[count], #1"                             "\n\t"
    "bne    L%=_loop"                                 "\n\t"
    "lsls   %[pix], #1"                               "\n\t"
    "bcs    L%=_last_one"                             "\n\t"
    "L%=_last_zero:"                                  "\n\t"
    "ldrb   %[portout], [%[reg], #0]"                 "\n\t"
    "orr    %[portout], %[portout], %[bitmask]"       "\n\t"
    "strb   %[portout], [%[reg], #0]"                 "\n\t"
    "movs   %[dly], #1"                               "\n\t"
    "L%=_last_delay_T0H:"                             "\n\t"
    "subs   %[dly], #1"                               "\n\t"
    "bne    L%=_last_delay_T0H"                       "\n\t"
    "ldrb   %[portout], [%[reg], #0]"                 "\n\t"
    "bic    %[portout], %[portout], %[bitmask]"       "\n\t"
    "strb   %[portout], [%[reg], #0]"                 "\n\t"
    "movs   %[dly], #8"                              "\n\t"
    "L%=_last_delay_T0L:"                             "\n\t"
    "subs   %[dly], #1"                               "\n\t"
    "bne    L%=_last_delay_T0L"                       "\n\t"
    "b  L%=_repeat"                                   "\n\t"
    "L%=_last_one:"                                   "\n\t"
    "ldrb   %[portout], [%[reg], #0]"                 "\n\t"
    "orr    %[portout], %[portout], %[bitmask]"       "\n\t"
    "strb   %[portout], [%[reg], #0]"                 "\n\t"
    "movs   %[dly], #8"                               "\n\t"
    "L%=_last_delay_T1H:"                             "\n\t"
    "subs   %[dly], #1"                               "\n\t"
    "bne    L%=_last_delay_T1H"                       "\n\t"
    "ldrb   %[portout], [%[reg], #0]"                 "\n\t"
    "bic    %[portout], %[portout], %[bitmask]"       "\n\t"
    "strb   %[portout], [%[reg], #0]"                 "\n\t"
    "movs   %[dly], #3"                               "\n\t"
    "L%=_last_delay_T1L:"                             "\n\t"
    "subs   %[dly], #1"                               "\n\t"
    "bne    L%=_last_delay_T1L"                       "\n\t"
    "nop"                                             "\n\t"
    "L%=_repeat:"                                     "\n\t"
    "add    %[p], #1"                                 "\n\t"
    "subs   %[num], #1"                               "\n\t"
    "bne    L%=_begin"                                "\n\t"
    "L%=_done:"                                       "\n\t"
    : [p] "+r" (p),
      [pix] "=&r" (pix),
      [count] "=&r" (count),
      [dly] "=&r" (dly),
      [portout] "=&r" (portout),
      [num] "+r" (num)
    : [bitmask] "r" (bitmask),
      [reg] "r" (reg)
  );
#else
#error WS2811Driver : Unsupported clock frequency, must be G2/F5529 at 16/25MHz or MSP432P401R at 48MHz
#endif 
// End of support for MSP432 based boards  -------------------------
  
#else
#error WS2811Driver : Unsupported hardware, must be MSP430G2/F5529 or MSP432P401R
#endif 
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
   if(n < _led_cnt) {
    if(brightness) { // See notes in setBrightness()
      r = (r * brightness) >> 8;
      g = (g * brightness) >> 8;
      b = (b * brightness) >> 8;
    }
    uint8_t *p = &pixels[n * 3];
    p[rOffset] = r;
    p[gOffset] = g;
    p[bOffset] = b;
  };
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
    if(brightness) { // See notes in setBrightness()
      r = (r * brightness) >> 8;
      g = (g * brightness) >> 8;
      b = (b * brightness) >> 8;
    }
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

/** @brief Adjust output brightness; 0=darkest (off), 255=brightest.  This does
 *  NOT immediately affect what's currently displayed on the LEDs.  The
 *  next call to show() will refresh the LEDs at this level.  However,
 *  this process is potentially "lossy," especially when increasing
 *  brightness.  The tight timing in the WS2811/WS2812 code means there
 *  aren't enough free cycles to perform this scaling on the fly as data
 *  is issued.  So we make a pass through the existing color data in RAM
 *  and scale it (subsequent graphics commands also work at this
 *  brightness level).  If there's a significant step up in brightness,
 *  the limited number of steps (quantization) in the old data will be
 *  quite visible in the re-scaled version.  For a non-destructive
 *  change, you'll need to re-render the full strip data.  C'est la vie.
 *
 *  @param b Brightness value for strip
 *
 *  @return void
 */
void WS2811Driver::setBrightness(uint8_t b) {
  // Stored brightness value is different than what's passed.
  // This simplifies the actual scaling math later, allowing a fast
  // 8x8-bit multiply and taking the MSB.  'brightness' is a uint8_t,
  // adding 1 here may (intentionally) roll over...so 0 = max brightness
  // (color values are interpreted literally; no scaling), 1 = min
  // brightness (off), 255 = just below max brightness.
  uint8_t newBrightness = b + 1;
  if(newBrightness != brightness) { // Compare against prior value
    // Brightness has changed -- re-scale existing data in RAM
    uint8_t  c,
            *ptr           = pixels,
             oldBrightness = brightness - 1; // De-wrap old brightness value
    uint16_t scale;
    if(oldBrightness == 0) scale = 0; // Avoid /0
    else if(b == 255) scale = 65535 / oldBrightness;
    else scale = (((uint16_t)newBrightness << 8) - 1) / oldBrightness;
    for(uint16_t i=0; i<(_led_cnt * 3); i++) {
      c      = *ptr;
      *ptr++ = (c * scale) >> 8;
    }
    brightness = newBrightness;
  }
}

// Set the output pin number
void WS2811Driver::setPin(uint8_t p) {
  pinMode(pin, INPUT);
  pin = p;
#if defined (__MSP430G2553__) || defined (__MSP430F5529__)
  _pin_mask = digitalPinToBitMask(p);
  _port_mask = digitalPinToPort(p);
  
  _port_mask = (uint16_t)portOutputRegister(_port_mask);
#endif

  pinMode(p, OUTPUT);
  digitalWrite(p, LOW);
}
