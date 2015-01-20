;--------------------------------------------------------------------------------------------------------------
; ws2811_hs.S - ws2811 high speed write routine for rgb controller
;--------------------------------------------------------------------------------------------------------------
;
;    Copyright (C) 2012  Kevin Timmerman
;
;   This program is free software: you can redistribute it and/or modify
;   it under the terms of the GNU General Public License as published by
;   the Free Software Foundation, either version 3 of the License, or
;   (at your option) any later version.
;
;   This program is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;   GNU General Public License for more details.
;
;   You should have received a copy of the GNU General Public License
;   along with this program.  If not, see <http://www.gnu.org/licenses/>.
;--------------------------------------------------------------------------------------------------------------
; 11-15-2012 - rick@kimballsoftware.com modified for msp430-gcc
;--------------------------------------------------------------------------------------------------------------

#include <msp430.h>
#include "ws2811.h"

        .file "ws2811_hs.S"
        .global write_ws2811_hs_25

;--------------------------------------------------------------------------------------------------------------
; void write_ws2811_hs_25(uint8_t *data, uint16_t length, uint8_t pinmask, uint8_t portmask);
;                         data - R15,    length - R14,    pinmask - R13,   portmask - @R12
;
; NOTE: assumes interrupts are disabled as cycle counts are used for timing
;
; --- High Speed Mode 800KHz
;        High / Low us   High / Low cycles @ 16 MHz
; Zero:  0.25 / 1.00        4 / 16
; One:   0.60 / 0.65        9 / 11
; Reset:    0 / 50+         0 / 800+
;
;
;--------------------------------------------------------------------------------------------------------------
        .section .text.write_ws2811_hs_25
        .p2align 1,0                    ; align on a word boundary
        .type write_ws2811_hs_25,@function
write_ws2811_hs_25:
        push    R10                     ; Save R10
        push    R11                     ; Save R11
byte_loop_hs:                           ;
        mov     #7, R10                 ; Do 7 bits in a loop
        mov.b   @R15+, R11              ; Get next byte from buffer
bit_loop_hs:                            ; - Bit loop - 20 cycles per bit
        rla.b   R11                     ; Get next bit
        jc      one_hs                  ; Jump if one...
        bis.b   R13, @R12                ; Output high
        nop
        nop
        bic.b   R13, @R12                ; Output low - 4 cycles elapsed
        nop
        nop
        nop
        nop
		nop
        nop
        nop
		nop
        nop
        nop
        jmp     next_bit_hs             ; Next bit...
one_hs:                                 ;
        bis.b   R13, @R12                ; Output high
        nop
        nop
        nop
		nop
		nop
		nop
        nop
        nop
		nop
        bic.b   R13, @R12                ; Output low - 10 cycles elapsed
        nop
        nop
        nop
        nop
        nop
next_bit_hs:                            ;
        dec     R10                     ; Decrement bit count
        jne     bit_loop_hs             ; Do next bit of not zero...
                                        ;
        rla.b   R11                     ; Get final bit of byte
        jc      last_one_hs             ; Jump if one...
        bis.b   R13, @R12                ; Output high
        nop
        nop
        bic.b   R13, @R12                ; Output low - 4 cycles elapsed
        nop
        nop
        nop
        nop
        nop
        nop
        nop
		nop
		nop
		nop
        dec     R14                     ; Decrement byte count
        jne     byte_loop_hs            ; Next byte if count not zero...
        jmp     reset_hs                ; All bytes done, reset...
last_one_hs:                            ;
        bis.b   R13, @R12                ; Output high
        nop
        nop
        nop
		nop
		nop
		nop
		nop
        mov     #7, R10                 ; Reset bit counter
        mov.b   @R15+, R11              ; Get next byte from buffer
        bic.b   R13, @R12                ; Output low - 10 cycles elapsed
        nop
        nop
        nop
        nop
        nop
        nop
        dec     R14                     ; Decrement byte count
        jne     bit_loop_hs             ; Do next byte if count is not zero...
                                        ;
reset_hs:                               ;
        mov     #1250 / 3, R11          ; 800 cycle delay for reset
        dec     R11                     ;
        jne     $ - 2                   ;
        pop     R11                     ; Restore R10
        pop     R10                     ; Restore R11
        ret                             ; Return
