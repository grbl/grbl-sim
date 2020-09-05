#  Part of Grbl Simulator
#
#  Copyright (c) 2012 Jens Geisler
#  Copyright (c) 2014-2015 Adam Shelly
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
# PLATFORM   = OSX
PLATFORM   = LINUX

#The original grbl code, except those files overriden by sim
GRBL_BASE_OBJECTS =   ../protocol.o ../planner.o ../settings.o ../print.o ../nuts_bolts.o  ../stepper.o ../gcode.o ../spindle_control.o ../motion_control.o ../limits.o ../coolant_control.o ../probe.o ../system.o
# grbl files that have simulator overrrides 
GRBL_OVERRIDE_OBJECTS =  ../main.o ../serial.o ../report.o

#AVR interface simulation
AVR_OBJECTS  = avr/interrupt.o avr/pgmspace.o  avr/io.o  avr/eeprom.o grbl_eeprom_extensions.o

# Simulator Only Objects
SIM_OBJECTS = main.o simulator.o serial.o util/delay.o util/floatunsisf.o platform_$(PLATFORM).o system_declares.o

GRBL_SIM_OBJECTS = grbl_interface.o  $(GRBL_BASE_OBJECTS) $(GRBL_OVERRIDE_OBJECTS) $(SIM_OBJECTS) $(AVR_OBJECTS)
GRBL_VAL_OBJECTS = validator.o overridden_report.o $(GRBL_BASE_OBJECTS) $(AVR_OBJECTS) system_declares.o

CLOCK      = 16000000
SIM_EXE_NAME   = grbl_sim.exe
VALIDATOR_NAME = gvalidate.exe
FLAGS = -g -O3
COMPILE    = $(CC) -Wall $(FLAGS) -DF_CPU=$(CLOCK)  -include config.h -I. -DPLAT_$(PLATFORM)
LINUX_LIBRARIES = -lrt -pthread
OSX_LIBRARIES =
WINDOWS_LIBRARIES =

# symbolic targets:
all:	main gvalidate

new: clean main gvalidate

clean:
	rm -f $(SIM_EXE_NAME) $(GRBL_SIM_OBJECTS) $(VALIDATOR_NAME) $(GRBL_VAL_OBJECTS)

# file targets:
main: $(GRBL_SIM_OBJECTS) 
	$(COMPILE) -o $(SIM_EXE_NAME) $(GRBL_SIM_OBJECTS) -lm $($(PLATFORM)_LIBRARIES)


gvalidate: $(GRBL_VAL_OBJECTS) 
	$(COMPILE)  -o $(VALIDATOR_NAME) $(GRBL_VAL_OBJECTS) -lm  $($(PLATFORM)_LIBRARIES)


%.o: %.c
	$(COMPILE) -c $< -o $@

../planner.o: ../planner.c
	$(COMPILE) -include planner_inject_accessors.c -c $< -o $@

../serial.o: ../serial.c
	$(COMPILE) -include serial_hooks.h -c $< -o $@

../main.o: ../main.c
	$(COMPILE) -include rename_main.h -c $< -o $@

overridden_report.o: ../report.c
	$(COMPILE) -include rename_report_status_message.h -c $< -o $@
