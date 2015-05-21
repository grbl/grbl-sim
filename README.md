# GRBL SIM 

: by Jens Geisler, Adam Shelly  


This repository contains an experimental Grbl simulator that compiles the main Grbl source code into a wrapped executable for use on a computer. No Arduino required. When the executable is run, the user should be able to interact with the Grbl simulator as if connected to an Arduino with Grbl.

*WARNING: Grbl Sim is under heavy development.* So many things may not work, or respond in ways unexpected. At the moment, this code is a proof-of-concept.

## What can you do with Grbl Sim? 

 - Simply check out how Grbl works without needing an Arduino.
 - Visualize a g-code program by having the simulator parse and execute to a GUI. Fluctuations in feed rates by the acceleration planner can be viewed as well.
 - A powerful debugging tool for development.
 - Each of the AVR functions are replaced with dummy functions, like the stepper ISR. These could be written to do whatever you need. For example, output simulated step pulses over time and examine its performance.
 - On Linux, hook it to a fake serial port (/dev/ttyFAKE) and use it to test your Grbl interface software:

  -  `> socat PTY,raw,link=/dev/ttyFAKE,echo=0 "EXEC:'./grbl_sim.exe -n -s step.out -b block.out',pty,raw,echo=0" `

 
### Realtime modifications:

  Now simulates Atmel hardware in separate thread.  Runs in *aproximate* realtime.  Emphasis on  * **Approximate** *.  Work is underway to speed it up.

## How do you compile Grbl Sim?

- Clone this repository into the directory containing the Grbl source code.  (should be `<repo>/grbl`).  

- Edit the Grbl-Sim Makefile to select the correct `PLATFORM =`  line.  LINUX and WINDOWS are currently supported. 

 - *(You may need to make other modifications to the Makefile and some environment variables for your particular machine. Please share any modifications you find)*

- Run `> make new` to compile Grbl Sim!  



## Validator
**NEW** 

Run `gvalidate.exe GCODE_FILE` to validate that grbl will parse your GCODE with no errors.

