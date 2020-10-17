# Lab 2 Starter Code

The code flashes between both of the on-board red and blue LED's on the KL25Z development at intervals of 3 seconds with a random button press generated which turns off the led's for a random interval at which point the led's continue flashing. 

There are two tasks:

  1. The 'randomPress' tasks simulates a button press at random intervals.
  1. The 'toogleColour' task toggles the blue and red LED's at 3 second intervals.

The behaviour is described by the following state-transition diagram:  

![state transition models of the tasks in lab 2](stm.png)
<img width="534" alt="LAB2STATE" src="https://media.github.research.its.qmul.ac.uk/user/2315/files/3cc41b80-10a7-11eb-9c44-448f1497c438">
