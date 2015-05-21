/*
  wdt.h - replacement for the avr include of the same name to provide
  dummy watchdog timer variables and macros

  Part of Grbl Simulator

  Copyright (c) 2012-2104 Jens Geisler, Adam Shelly

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

#define WDTCSR wdt
#define WDP0 0
#define WDP1 1
#define WDP2 2
#define WDE  3
#define WDCE 4
#define WDP3 5
#define WDIE 6
#define WDIF 7

uint16_t wdt;
