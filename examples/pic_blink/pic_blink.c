// Example taken from
// https://github.com/diegoherranz/sdcc-examples/blob/master/pic14/1.blink_led/blink_led.c
// Copyright (C) 2014 Diego Herranz
// GPLv2
//
/*

Copyright (C) 2014 Diego Herranz

This file is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Foobar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar. If not, see <http://www.gnu.org/licenses/>.
*/

#include <pic16regs.h>
#include <stdint.h>


// Uncalibrated delay, just waits a number of for-loop iterations
void delay(uint16_t iterations)
{
	uint16_t i;
	for (i = 0; i < iterations; i++) {
		// Prevent this loop from being optimized away.
		__asm nop __endasm;
	}
}

int main(void) {
  TRISA1 = 0;
 while (1) {
   RA1 = 1;
   delay(1000);
   RA1 = 0;
   delay(1000);
 }
 return 0;
}
