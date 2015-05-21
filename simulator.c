/*
  simulator.c - functions to simulate how the buffer is emptied and the
    stepper interrupt is called

  Part of Grbl Simulator

  Copyright (c) 2012-2014 Jens Geisler
  Copyright (c) 2014-2015 Adam Shelly

  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdbool.h>

#include "simulator.h"
#include "avr/eeprom.h"
#include "avr/io.h"
#include "avr/interrupt.h"


sim_vars_t sim={0};

enum {sh_INIT,sh_TICK,sh_BYTE,sh_EXIT};
void sim_nop(void) {}
sim_hook_fp sim_hook[4]={sim_nop,sim_nop,sim_nop,sim_nop};
void sim_add_hooks(sim_hook_fp initialize, 
                   sim_hook_fp per_tick,
                   sim_hook_fp per_byte,
                   sim_hook_fp shutdown)
{
  if (initialize) sim_hook[sh_INIT]=initialize;
  if (per_tick) sim_hook[sh_TICK]=per_tick;
  if (per_byte) sim_hook[sh_BYTE]=per_byte;
  if (shutdown) sim_hook[sh_EXIT]=shutdown;
}

//setup 
void init_simulator(float time_multiplier) {
  sim.speedup = time_multiplier;

  sim_hook[sh_INIT]();
}



//shutdown simulator - call exit hooks, save eeprom
void shutdown_simulator() {
  sim_hook[sh_EXIT]();
  eeprom_close();
}


void simulate_hardware(bool do_serial){

  //do one tick
  sim.masterclock++;
  sim.sim_time = (float)sim.masterclock/F_CPU;

  timer_interrupts();
  
  if (do_serial) simulate_serial();

  //TODO:
  //  check limit pins,  call pinchange interrupt if enabled
  //  can ignore pinout int vect - hw start/hold not supported

}

//runs the hardware simulator at the desired rate until sim.exit is set
void sim_loop(){
  uint64_t simulated_ticks=0;
  uint32_t ns_prev = platform_ns();
  uint64_t next_byte_tick = F_CPU;   //wait 1 sec before reading IO.


  while (sim.exit != exit_OK  ) { //don't quit until idle

   if (sim.speedup) {
    //calculate how many ticks to do.
    uint32_t ns_now = platform_ns();
    uint32_t ns_elapsed = (ns_now-ns_prev)*sim.speedup; //todo: try multipling nsnow
    simulated_ticks += F_CPU/1e9*ns_elapsed;
    ns_prev = ns_now;
   }
   else {
    simulated_ticks++;  //as fast as possible
   }
    
   while (sim.masterclock < simulated_ticks){

    //only read serial port as fast as the baud rate allows
    bool read_serial = (sim.masterclock >= next_byte_tick);

    //do low level hardware
    simulate_hardware(read_serial);

    // do app-specific per-tick processing
    sim_hook[sh_TICK]();
    
    if (read_serial){
      //decode baud rate. //TODO: maybe do this only once, baud shouldn't change on the fly
      sim.baud_ticks = 2*((UBRR0H<<8)+UBRR0L)+1;
      if (UCSR0A & (1<<U2X0)) { sim.baud_ticks*=2; }

      next_byte_tick+=sim.baud_ticks;
      // do app-specific per-byte processing
      sim_hook[sh_BYTE]();
    }
   }

   platform_sleep(0); //yield
  }
}


//print serial output to args.serial_out_file
void sim_serial_out(uint8_t data){
  static uint8_t buf[128]={0};
  static uint8_t len=0;
  static bool continuation = 0;

  buf[len++]=data;
  // print when we get to newline or run out of buffer
  if(data=='\n' || data=='\r' || len>=127) {
   if (args.comment_char && !continuation){
    fprintf(args.serial_out_file,"%c ",args.comment_char);
   }
   buf[len]=0;
   fprintf(args.serial_out_file,"%s",buf);
   //don't print comment on next line if we are just printing to avoid buffer overflow
   continuation = (len>=128); 
   len=0;
  }
}

