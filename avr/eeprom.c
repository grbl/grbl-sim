/*
  eeprom.c - replacement for the avr library of the same name to provide
  replacement functionality - write to "EEPROM.dat" in working directory

  Part of Grbl Simulator

  Copyright (c) 2012 Jens Geisler
  Copyright (c) 2014 Adam Shelly

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
#define MAX_EEPROM_SIZE 4096   //4KB EEPROM in Mega


FILE* eeprom_create_empty_file(){
  FILE* fp = fopen("EEPROM.DAT","w+b");
  int i;
  if (fp){
	 for(i=0;i<MAX_EEPROM_SIZE;i++){
		fputc(0,fp);
	 }
	 fseek(fp,0,SEEK_SET);
  }
  return fp;
}


FILE* eeprom_fp(){
  static FILE* EEPROM_FP = NULL;
  static int tried= 0;
  if (!EEPROM_FP && !tried){
	 tried = 1;
	 EEPROM_FP = fopen("EEPROM.DAT","r+b");
	 if (!EEPROM_FP) {
		EEPROM_FP = eeprom_create_empty_file();
	 }
  }
  return EEPROM_FP;
}

void eeprom_close(){
  FILE* fp = eeprom_fp();
  fclose(fp);
}
unsigned char eeprom_get_char( unsigned int addr ) {

  FILE* fp = eeprom_fp();
  if (fseek(fp,addr,SEEK_SET)) { return 0; } //no such address
  return fgetc(fp);
}

void eeprom_put_char( unsigned int addr, unsigned char new_value ) {
  FILE* fp = eeprom_fp();
  if (fseek(fp,addr,SEEK_SET)) { return; } //no such address
  fputc(new_value, fp);
}



// end of file
