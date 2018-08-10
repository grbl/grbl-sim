/*
  config.h - replacement for the include of the same name in grbl
  to define dummy registers

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

#ifndef config_h
#define __flash
// Include grbl's system.h, not its config.h, 
// because of circular dependency (config.h includes system.h which includes config.h).
// This way ensures that the CPU Map and other config flags are set before they are needed
#include "../system.h"



#endif



