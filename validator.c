/*
  validator.c -  Grbl G-code validation

  Part of Grbl Simulator

  Copyright (c) 2012 Jens Geisler

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

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "../nuts_bolts.h"
#include "../settings.h"
#include "../protocol.h"
#include "../report.h"
#include "../system.h"
#include "../gcode.h"
#include "../planner.h"
#include "../serial.h"
#include "avr/io.h"

static void protocol_execute_line(char *line);
// Declare system global variable structure
system_t sys; 
volatile io_sim_t io;


typedef struct arg_vars {
  // Output file handles
  FILE *input_file;
  FILE *output_file;
  uint8_t echo;
  uint8_t silent;   
} arg_vars_t;
extern arg_vars_t args;


arg_vars_t args;
const char* progname;



int usage(const char* badarg){
  if (badarg){
    printf("Unrecognized option %s\n",badarg);
  }
  printf("Usage: \n"
         "%s <Options> [input_file]\n"
         "  Options:\n"
         "    -o <output file> : use output file instead of stdout\n"
         "    -e        : echo input to output\n"
         "    -s        : silent, no output only return code \n"
         "\n  Parses gcode from stdin or input line, prints grbl's expected response"
         "\n  Returns 0 on successs, or line number of error",
         progname);
  return -1;
}

int main(int argc, char *argv[]) {
  int err = 0;
  int positional_args=0;

  //defaults
  args.input_file = stdin;
  args.output_file = stdout;
  args.echo = 0;
  args.silent = 0;

  // Get the minimum time step for printing stepper values.
  // If not given or the command line cannot be parsed to a float than
  // step_time= 0.0; This means to not print stepper values at all

  progname = argv[0];
  while (argc>1) {
    argv++;argc--;
    if (argv[0][0] == '-'){
      switch(argv[0][1]){
        case 'e':  //echo mode
          args.echo = 1;
          break;
        case 's': //silent
          args.silent = 1;
          break;
        case 'o': //output file
          argv++;argc--;
          args.output_file = fopen(*argv,"w");
          if (!args.output_file) {
            perror("fopen");
            printf("Error opening : %s\n",*argv);
            return(usage(0));
          }
          break;
        case 'h':
          return usage(NULL);
        default:
          return usage(*argv);
      }
    }
    else { //handle positional arguments
      positional_args++;
      switch(positional_args){
        case 1: //input file
          args.input_file = fopen(*argv,"r");
          if (!args.input_file) {
            perror("fopen");
            printf("Error opening : %s\n",*argv);
            return(usage(0));
          }
          break;
        default:
          return usage(*argv);
      }
    }
  }

  protocol_main_loop();
  /* char line[LINE_BUFFER_SIZE+1]; */
  /* int linecount = 1; */
  /* while (!feof(args.input_file)) { */
  /*   char* data = fgets(line, LINE_BUFFER_SIZE, args.input_file); */
  /*   if (data) { */
  /*     protocol_execute_line(line); */
  /*     linecount++; */
  /*   } */
  /* } */
  return err;
}


/**** Re-implemented Funcitons ****/


//from protocol.c (is static there)
static void protocol_execute_line(char *line) 
{      
  protocol_execute_runtime(); // Runtime command check point.
  if (sys.abort) { return; } // Bail to calling function upon system abort  

  if (line[0] == 0) {
    // Empty or comment line. Send status message for syncing purposes.
    report_status_message(STATUS_OK);

  } else if (line[0] == '$') {
    // Grbl '$' system command
    report_status_message(system_execute_line(line));
    
  } else if (sys.state == STATE_ALARM) {
    // Everything else is gcode. Block if in alarm mode.
    report_status_message(STATUS_ALARM_LOCK);

  } else {
    // Parse and execute g-code block!
    report_status_message(gc_execute_line(line));
  }
}



//read fom input;
uint8_t serial_read() {

  int data = fgetc(args.input_file);
  if (args.echo) { fputc(data, args.output_file); }
  plan_reset();
  if (feof(args.input_file) || data == 0x06 || data == -1) { 
    sys.abort = 1;
    return SERIAL_NO_DATA;
  }
  return data;
}
//write to output
void serial_write(uint8_t data) {
  if (!args.silent){
    fputc(data, args.output_file);
  }
}
