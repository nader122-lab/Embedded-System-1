/* ------------------------------------------
       ECS642/ECS714 Lab2

   Simulation of a button using random delays
   Toggle blue on every simulated press
  -------------------------------------------- */

#include <MKL25Z4.h>

#include <stdbool.h>

#include "SysTick.h"

#include "triColorLed.h"

/* --------------------------------------
     Documentation
     =============
     This is a cyclic system with a cycle time of 10ms

     The code has two tasks
     1. randomPress: this simulates presses at random intervals between 3 and 13 sec
     2. toggleBlue: this toggles the blue LED on every press

     In addition
     * External functions: 
       - configureLEDs  - sets up the peripherals so the LEDs can be used
       - Init_SysTick(), waitSysTickCounter() - time the cyclic system
       - setRedLED, setGreenLED, setBlueLED - control the LEDs
     * Function to generate random delays 

 -------------------------------------- */

/*----------------------------------------------------------------------------
 * nextRand: get next random number 
 *   Based on https://en.wikipedia.org/wiki/Linear_congruential_generator
 * --------------------------------------------------------------------------- */
uint32_t seed = 0x12345678;

// Returns a 32 bit number which is too long for us
uint32_t nextRand(void) {
  seed = (1103515245 * seed + 12345);
  return seed;
}

// Generate random count in range 3-13 sec 
//    - take top 10 bits - max is 1023
//    - add 277 so max is 1300 (13 sec), min is 277  (2.8 sec)
uint32_t randCount() {
  uint32_t r1023 = (nextRand() & 0xFFC00000) >> 22; // top 10 bits
  return r1023 + 277;
}

/*----------------------------------------------------------------------------
   Task: randomPressTask

   Generate a signal at random intervals between 5 and 15 sec
   DO NOT CHANGE
 *----------------------------------------------------------------------------*/
#define UP 0
#define DOWN 1
#define DOWNCOUNT 20

int stateRPress; // state of the task
uint32_t countRPress; // count for timing

// initialise state
void initRandomPressTask() {
  countRPress = randCount();
  stateRPress = UP;
}

bool signalR = false; // signal to other task

void randomPressTask() {
  if (countRPress > 0) countRPress--;
  switch (stateRPress) {
  case UP:
    if (countRPress == 0) {
      signalR = true;
      stateRPress = DOWN;
      countRPress = DOWNCOUNT;
    }
    break;
  case DOWN:
    if (countRPress == 0) {
      stateRPress = UP;
      countRPress = randCount();
    }
    break;
  }
}

/*----------------------------------------------------------------------------
   Task: toggleBlue

   Toggle blue LED on every signal

   REPLACE THIS TASK
*----------------------------------------------------------------------------*/

#define BLUEON 0
#define REDON 1
#define BOTHOFF 2
#define BOTHPAUSE 3
#define RED 2
#define BLUE 1

int colourState; // this variable holds the current state
int count = 300; // this variable initialises the 3 second count
int countPause; // this variable holds the count before initial press
int colour = BLUE; // this variable holds the current colour

// initial state of task
void initToggleBlueTask() {
  colourState = BLUEON;
  setBlueLED(ON);
}

void toggleColourTask() {
  if (count > 0) count--; // decrement the counter

  if (signalR) { // signal received  
    if (colourState == BOTHPAUSE) {} else {
      colourState = BOTHOFF;
    }
  }

  switch (colourState) { // state names defined above function

  case BLUEON:
    setRedLED(OFF); // set led colour for current state      
    setBlueLED(ON);
    if (count == 0) { // time to change state and reset count 
      colourState = REDON; // the next state
      colour = RED; // the next colour		
      count = 300; // reset count timer		
    }
    break;

  case REDON:
    setRedLED(ON); // set led colour for current state     
    setBlueLED(OFF);
    if (count == 0) { // time to change state and reset count 
      colourState = BLUEON; // the next state
      colour = BLUE; // the next colour
      count = 300; // reset count timer		
    }
    break;

  case BOTHOFF:
    setBlueLED(OFF); // set led colour for current state
    setRedLED(OFF);
    countPause = count; // holds count after initian press
    signalR = false; // acknowledge signal         
    colourState = BOTHPAUSE; // the next state

    break;

  case BOTHPAUSE:

    if (signalR) { // signal received second press
      signalR = false; // acknowledge signal
      count = countPause; // copies count from initial press to current count
      if (colour == RED) { // determines the current state 					
        colourState = REDON; //the next state
      } else {
        colourState = BLUEON; // the next state	 
      }
    }
    break;

  }
}

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main(void) {
  configureLEDs(); // configure the GPIO outputs for the LED
  Init_SysTick(1000); // initialse SysTick every 1 ms
  initRandomPressTask(); // initialise task state
  initToggleBlueTask(); // initialise task state
  waitSysTickCounter(10); // initialise delay counter
  while (1) { // this runs for ever
    randomPressTask(); // Generate signals for a simulated button
    toggleColourTask(); // Toggle blue LED on every press signal
    // delay
    waitSysTickCounter(10); // cycle every 10 ms 
  }
}