// Example taken from
// https://github.com/diegoherranz/sdcc-examples/blob/master/pic14/1.blink_led/blink_led.c
// Copyright (C) 2014 Diego Herranz
// GPLv2
//

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
