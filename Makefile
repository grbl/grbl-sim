#  Part of Grbl Simulator
#
#  Copyright (c) 2012 Jens Geisler
#
#  Grbl is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  Grbl is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.

# PLATFORM   = WINDOWS
PLATFORM   = LINUX

# All grbl objects except those overridden by the sim
BASE_OBJECTS = ../protocol.o ../planner.o ../settings.o ../print.o ../nuts_bolts.o eeprom.o ../stepper.o ../gcode.o ../spindle_control.o ../motion_control.o ../limits.o ../report.o ../coolant_control.o ../probe.o ../system.o 
AVR_OBJECTS  = avr/interrupt.o avr/pgmspace.o  avr/io.o util/delay.o util/floatunsisf.o

# Objects for runtime sim
SIM_OBJECTS  = main.o simulator.o serial.o ../main.o ../serial.o  $(BASE_OBJECTS) $(AVR_OBJECTS) platform_$(PLATFORM).o
# Objects for offline gcode validation
GV_OBJECTS = validator.o $(BASE_OBJECTS) $(AVR_OBJECTS)

CLOCK      = 16000000
EXE_NAME   = grbl_sim.exe
COMPILE    = $(CC) -Wall -g -DF_CPU=$(CLOCK) -include config.h -I. -DPLAT_$(PLATFORM)
LINUX_LIBRARIES = -lrt -pthread
WINDOWS_LIBRARIES =


# symbolic targets:
all:	main

new: clean main gvalidate

gvalidate: validator

clean:
	rm -f $(EXE_NAME) $(SIM_OBJECTS) $(GV_OBJECTS) gvalidate.exe

# file targets:
main: $(SIM_OBJECTS)
	$(COMPILE) -o $(EXE_NAME) $(SIM_OBJECTS) -lm  $($(PLATFORM)_LIBRARIES)

validator: $(GV_OBJECTS)
	$(COMPILE) -o gvalidate.exe $(GV_OBJECTS)  -lm  $($(PLATFORM)_LIBRARIES)

%.o: %.c
	$(COMPILE)  -c $< -o $@

../planner.o: ../planner.c
	$(COMPILE) -include planner_inject_accessors.c -c $< -o $@

../serial.o: ../serial.c
	$(COMPILE) -include serial_hooks.h -c $< -o $@

../main.o: ../main.c
	$(COMPILE) -include rename_main.h -c $< -o $@


