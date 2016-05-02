#include <inttypes.h>
#include "chip8.h"
#include "cpu.h"

// Run the CPU cycle, this checks the opcode and decides where to go. This is
// the function that we call from the emulation loop in main() in chip8.c.
int cpu_runcycle(struct emu_vars* hw_state) {

  // First get the opcode:
  hw_state->opcode = hw_state->memory[hw_state->pc]; // First byte, 2nd below
  hw_state->opcode = hw_state->opcode | hw_state->memory[hw_state->pc +1];

  return 0;
}
