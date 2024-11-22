/* 
Header & source to ease the use of timer 1 of Arduino's processor
v1.0, 19.11.2023 - Jenna Salmela
*/

#include <Arduino.h>
#include "timer1.h"

// Function called when the timer interrupt triggers
void (*timerOneCallableFunction)();

// No. of clock ticks in the timer counter register at the time of timer1 pause, used to unpause the timer
uint16_t timerOnePauseValue;

/*Preps and starts timer 1
Max timer duration is a touch under 4.2 seconds
Function parameters:
  uint32_t runtime = timer duration in microseconds (0.000001s)
  void *function = what function gets called with the timer interrupt
*/
int SetTimerOne(uint32_t runtime, void (*function)()) 
{
	// Disable interrupts so as not to disturb the setup
	cli();
	
	// Check if the wanted runtime can be measured by the timer without overflow
	// (= smaller than max register size * max prescaler value)
	if (runtime > 67108864) {
		sei();
		return 1;
	}

	// Backups in case something goes wrong and the old state should be recovered instead
	uint8_t r1A = TCCR1A;
	uint8_t r1B = TCCR1B;
	uint8_t r1C = TCCR1C;
	uint8_t rTimer = TCNT1;

	// Alter control registers to set the timer's mode to CTC
	TCCR1A = 0;
	TCCR1B = 0b00001000;
	TCCR1C = 0;
	
	// Reset the timer's countdown
	TCNT1 = 0;
	
	// Assign function to be called to the interrupt
	timerOneCallableFunction = function;

	// Calculate the required prescaler value
	uint16_t prescaler;

	// If the wanted runtime can't fit in the register with prescaling of 256
	if (runtime > 1048576) {
		// Set the last 3 bits to "101" for prescaling of 1024
		TCCR1B |= 0b00000101;
		TCCR1B &= 0b11111101;
		prescaler = 1024;
	}
	else if (runtime > 262144) {
		// Set the last 3 bits to "100" for prescaling of 256
		TCCR1B |= 0b00000100;
		TCCR1B &= 0b11111100;
		prescaler = 256;
	}
	else if (runtime > 32768) {
		// Set the last 3 bits to "011" for prescaling of 64
		TCCR1B |= 0b00000011;
		TCCR1B &= 0b11111011;
		prescaler = 64;
	}
	else if (runtime > 4096) {
		// Set the last 3 bits to "010" for prescaling of 8
		TCCR1B |= 0b00000010;
		TCCR1B &= 0b11111010;
		prescaler = 8;
	}
	else if (runtime > 0) {
		// Set the last 3 bits to "001" for no prescaling
		TCCR1B |= 0b00000001;
		TCCR1B &= 0b11111001;
		prescaler = 1;
	}
	else {
		// Something's wrong, restore previous state, quit and return a 0
		TCCR1A = r1A;
		TCCR1B = r1B;
		TCCR1C = r1C;
		TCNT1 = rTimer;
		sei();
		return 1;
	}

	// Set the timer counter value that triggers an interrupt
	// Clock frequency and runtime are both in microseconds
	OCR1A = ((16 * runtime) / prescaler);

	// Tell the timer to activate an interrupt when TCNT (timer cycle counter) reaches 
	// OCR1A (the given delay), aka set the interrupt machine in motion
	TIMSK1 |= 0b00000010;
	
	// Re-enable interrupts
	sei();

	// Timer set successfully
	return 0;
}

// Pauses the execution of timer interrupts
int PauseTimerOne()
{
	cli();
	timerOnePauseValue = TCNT1;
	TIMSK1 &= 0b11111101;
	sei();
	return 0;
}

/*
Restarts timer 1 with the same timer countdown value as was present before PauseTimerOne was used
(e.g. with 0.9 seconds of a 2 second timer elapsed)
*/
int UnpauseTimerOne()
{
	cli();
	TCNT1 = timerOnePauseValue;
	TIMSK1 |= 0b00000010;
	sei();
	return 0;
}

/*
Restarts timer one with the countdown beginning from 0
(e.g. with 0.0 seconds of a 2 second timer elapsed)
*/
int RestartTimerOne()
{
	cli();
	TCNT1 = 0;
	TIMSK1 |= 0b00000010;
	sei();
	return 0;
}

// The interrupt service routine of timer 1, points to the function passed down through SetTimerOne
ISR (TIMER1_COMPA_vect)
{
	timerOneCallableFunction();
}