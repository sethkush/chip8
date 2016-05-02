#include <stdio.h>
#include <inttypes.h>
#include "chip8.h"
#include "cpu.h"

// Run the CPU cycle, this checks the opcode and decides where to go. This is
// the function that we call from the emulation loop in main() in chip8.c.
int cpu_runcycle(struct emu_vars* hw_state) {

  // First get the opcode:
  hw_state->opcode = hw_state->memory[hw_state->pc]; // First byte, 2nd below
  hw_state->opcode = hw_state->opcode | hw_state->memory[hw_state->pc +1];
  printf("%" PRIu16 "\n", hw_state->opcode);

  // Now decide where to go:
  switch(hw_state->opcode & 0xF000) {
    
  case 0x0000: // First 3 opcodes, beginning with 0
    switch(hw_state->opcode) {
    case 0x0E0:
      CLS();
      break;
    case 0x00EE:
      RET();
      break;
    default:
      SYS(hw_state->opcode & 0x0FFF);
    }
    break;

    // TODO: more opcodes
  }
  
  return 0;
}

// The SYS instruction is ignored by modern interpreters, so just return 0.
// This function does nothing, but it was good practice.
int SYS(uint16_t address){
  return 0;
}

// Clear the screen TODO
int CLS(){
  return 0;
}

// Return from subroutine TODO
int RET(){
  return 0;
}
