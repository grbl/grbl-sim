/*
  grbl_interface.c - hooks to interact with grbl app running on simulator

  Part of Grbl Simulator

  Copyright (c) 2012 Jens Geisler
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
#include "simulator.h"

#include "../system.h"
#include "../planner.h"
#include "../settings.h"


int block_position[N_AXIS]= {0}; //step count after most recently planned block
uint32_t block_number= 0;
double next_print_time;

void print_steps(bool force);
void printBlock(void);


void grbl_app_init(void){
  //register the interrupt handlers we actually use.
  compa_vect[1] = interrupt_TIMER1_COMPA_vect;
  ovf_vect[0] = interrupt_TIMER0_OVF_vect;
#ifdef STEP_PULSE_DELAY
  compa_vect[0] = interrupt_TIMER0_COMPA_vect;
#endif
#ifdef ENABLE_SOFTWARE_DEBOUNCE
  wdt_vect = interrupt_WDT_vect;
#endif
  pc_vect = interrupt_LIMIT_INT_vect;

  //setup local tacking vars
  next_print_time = args.step_time;

}

void grbl_per_tick(void){
  //maybe print the position every tick
  print_steps(0);

  //TODO:
  //  set limit pins based on position,
  //  set probe pin when probing.
  //  if VARIABLE_SPINDLE, measure pwm pin to report speed?
}

void grbl_per_byte(void){
  //maybe print newest block
  printBlock();
}

void grbl_app_exit(void){
  //force final position print
  print_steps(1);
}


//show current position in steps
void print_steps(bool force)
{ 
  static plan_block_t* printed_block = NULL;
  plan_block_t* current_block = plan_get_current_block();
  int ocr = 0;
  #ifdef VARIABLE_SPINDLE
  if(TCCRA_REGISTER >= 127) ocr = OCR_REGISTER;
  #endif

  //Allow exit when idle. Prevents aborting before all streamed commands have run
  if (sim.exit == exit_REQ && sys.state < STATE_HOMING ) { sim.exit = exit_OK;} 

  if (next_print_time == 0.0) { return; }  //no printing
  if (current_block != printed_block ) {
   //new block. 
   if (block_number) { //print values from the end of prev block
     fprintf(args.step_out_file, "%12.5f %d, %d, %d, %d\n", sim.sim_time, sys.position[X_AXIS], sys.position[Y_AXIS], sys.position[Z_AXIS],ocr);
   }
   printed_block = current_block;
   if (current_block == NULL) { return; }
   // print header
   fprintf(args.step_out_file, "# block number %d\n", block_number++);
  }
  //print at correct interval while executing block
  else if ((current_block && sim.sim_time>=next_print_time) || force ) {
     fprintf(args.step_out_file, "%12.5f %d, %d, %d, %d\n", sim.sim_time, sys.position[X_AXIS], sys.position[Y_AXIS], sys.position[Z_AXIS],ocr);
   fflush(args.step_out_file);

   //make sure the simulation time doesn't get ahead of next_print_time
   while (next_print_time<=sim.sim_time) next_print_time += args.step_time;
  }
}


//Functions for peeking inside planner state:
plan_block_t *get_block_buffer();
uint8_t get_block_buffer_head();
uint8_t get_block_buffer_tail();

// Returns the index of the previous block in the ring buffer
uint8_t prev_block_index(uint8_t block_index)
{
  if (block_index == 0) { block_index = BLOCK_BUFFER_SIZE; }
  block_index--;
  return(block_index);
}

plan_block_t *plan_get_recent_block() {
  if (get_block_buffer_head() == get_block_buffer_tail()) { return(NULL); }
  return(get_block_buffer()+prev_block_index(get_block_buffer_head()));
}

// Print information about the most recently inserted block
// but only once!
void printBlock() {
  plan_block_t *b;
  static plan_block_t *last_block;

  b= plan_get_recent_block();
  if(b!=last_block && b!=NULL) {
   int i;
   for (i=0;i<N_AXIS;i++){
    if(b->direction_bits & get_direction_pin_mask(i)) block_position[i]-= b->steps[i];
    else block_position[i]+= b->steps[i];
    fprintf(args.block_out_file,"%d, ", block_position[i]);
   }
   fprintf(args.block_out_file,"%f", b->entry_speed_sqr);
   fprintf(args.block_out_file,"\n");
   fflush(args.block_out_file); //TODO: needed?

    last_block= b;
  }
}
