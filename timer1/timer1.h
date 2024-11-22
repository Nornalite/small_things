/* 
Header & source to ease the use of timer 1 of Arduino's processor
v1.0, 19.11.2023 - Jenna Salmela
*/

#ifndef TIMER1_H
#define TIMER1_H

/*
Preps and starts timer 1
Max timer duration is a touch under 4.2 seconds
Parameters to be sent:
runtime = timer duration in microseconds (0.000001s, 1000000 would be one second)
&function = what function gets called with the timer interrupt
*/
int SetTimerOne(uint32_t runtime, void (*function)());

// Pauses the execution of timer interrupts for timer 1
int PauseTimerOne();

/*
Restarts timer 1 with the same timer countdown value as was present before PauseTimerOne was used
(e.g. with 0.9 seconds of a 2 second timer elapsed, 1.1 seconds to next interrupt)
*/
int UnpauseTimerOne();

/*
Restarts timer one with the countdown beginning from 0
(e.g. with 0.0 seconds of a 2 second timer elapsed)
*/
int RestartTimerOne();

#endif