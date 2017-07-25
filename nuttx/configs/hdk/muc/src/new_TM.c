/***************************************************************************
 *   Copyright (C) 2015 by                                                 *
 *   - Carlos Eduardo Millani (carloseduardomillani@gmail.com)             *
 *   - Edson Borin (edson@ic.unicamp.br)                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "TM.h"
#include "EH.h"
#include "HAL.h"
#include "vm.h"

#include "stdutils.h"
#include "inttypes.h"

//#include <stdio.h>
//#include <stdint.h>
//#include <time.h>

//#include <iostream>
//#include <fstream>
//#include <sstream>
//#include <string>
// #include <cstring>

// #include "base64.h"

// #include "libMiletus.h"

uint32_t tm_counter = 0;
uint16_t tot_size = 0;

void idle(void);
void receiving_sz(void);
void receiving_x(void);
void executing(void);
void reseting(void);
void hold(void);

void (*state)(void);

void idle(void) {
  //TODO: should sleep here :)
}
  
void receiving_sz(void) {
  eh_init();
  
  uint8_t size1 = read_byte();
  uint8_t size2 = read_byte();

  tot_size = (uint16_t)size1 | ((uint16_t)size2 << 8);

  state = receiving_x;
  send_byte('k');
}

void receiving_x(void) {
  uint16_t i;
  for (i = 0; i < tot_size; i++) {
    VM_memory[i] = read_byte();
    if ((i+1)%20 == 0) send_byte('k');
  }
  if (tot_size%20 != 0) send_byte('k');
  state = executing;
  enable_commands();
  vm_init(0);
  // print("Exec\n");
}

void executing(void) {
  uint8_t res = vm_cpu();
  if (res == 1) state = idle;
  else if (res == 2) state = hold;
}

void reseting(void) {
  eh_init();
  
  uint16_t i;
  for (i = 0; i < VM_MEMORY_SZ; i++) {
    VM_memory[i] = 0;
  }
  state = idle;
  enable_commands();
}

void hold(void) {
  //
}

void parse_Command(volatile char * command) {
  has_command = 0;
  if (!strcmpsz((char *)command,"RD", 2)) {
    print("RD-OK");
    state = receiving_sz;
  } else if (!strcmpsz((char *)command,"RS", 2)) {
    print("RS-OK");
    state = reseting;
  } else {
    send_byte(inBuffer[0]);
    send_byte(inBuffer[1]);
    send_byte('-');
    send_byte('?');
    send_byte('?');
  }
}

void tm_init(void) {
  /*COISA's Initialization*/
  eh_init();
  
  /*Sets initial State*/
  state = idle;
  
  /*Coisa VM cpu, HAL, EH and TM loop*/
    while(1)
    {
    if (has_command) {
      parse_Command(inBuffer);
    }
    if(timer_flag)
    { 
      tm_counter++;
      timed_polling();
      timer_flag = 0;

      if (state == idle) //Doesn't interrupts other functions - All funcs must be non blocking
      {
        state = executing; //TODO: do i really need to set this here, can iterate for nothing :(
        consume_event();
      }
    }
    state();
    
  }
}
  
#ifdef __cplusplus
}
#endif
  