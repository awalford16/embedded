## Counter and Timer Vectors

There are 3 timer/counter vectors in ATMega; 2 8-bit and 1 16-bit.

One is already used by Arduino for the Millis() functionality

This project uses the main system clock of the microcontroller

It comes with 2 compare registers. If a counter value matches with the compare register then it will send an interupt.

### Timer Modes

Clear timer on Compare Match (CTC) mode. Sets the counter to 0 when TCNT1 matches either OCR1A or ICR1.

### Features

MCU has an external 16MHz resounator

The Timer1 count clock = 16mHz/prescaler

The prescaler can be configured with the TCCR1B register so the clock can be divided by 8/64/256

16Mhz/256 = 62.5kHz

A count will happen every 16us

### Output Compare Match Value

To determine when to send an interupt to update the current_time in 100ms intervals:

100ms/16us = 6250 (ticks)

Output compare match value = 6250 - 1

Minus 1 because the count starts from 0