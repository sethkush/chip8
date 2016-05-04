#include <stdio.h>
#include <inttypes.h>
#include "chip8.h"
#include "cpu.h"

// Run the CPU cycle, this checks the opcode and decides where to go. This is
// the function that we call from the emulation loop in main() in chip8.c.
void cpu_runcycle(struct emu_vars* hw_state) {

  // First get the opcode:
  hw_state->opcode = hw_state->memory[hw_state->pc]; // First byte, 2nd below
  hw_state->opcode = hw_state->opcode | hw_state->memory[hw_state->pc +1];
  printf("%" PRIu16 "\n", hw_state->opcode);

  // Now decide where to go:
  switch(hw_state->opcode & 0xF000) {
    
  case 0x0000: // First 3 opcodes, beginning with 0
    switch(hw_state->opcode) {
    case 0x0E0: // Clear screen
      CLS(hw_state);
      break;
    case 0x00EE: // Return from subroutine
      RET(hw_state);
      break;
    default:
      SYS();
    }
    break;

  case 0x1000: // Jump
    JP(hw_state);
    break;

  case 0x2000: // Call subroutine
    CALL(hw_state);
    break;

  case 0x3000: // Skip if equal
    SE(hw_state);
    break;

  case 0x4000: // Skip if not equal
    SNE(hw_state);
    break;

  case 0x5000: // Skip if equal (V registers)
    SE(hw_state);
    break;

  case 0x6000: // Store kk in V[x]
    LD(hw_state);
    break;

  case 0x7000: // Add kk to V[x]
    ADD(hw_state);
    break;

  case 0x8000: // Many start with 8 so theres another switch here:
    switch(hw_state->opcode & 0xF00F) {
    case 0x8000:
      LD(hw_state);
      break;
    case 0x8001:
      OR(hw_state);
      break;
    case 0x8002:
      AND(hw_state);
      break;
    case 0x8003:
      XOR(hw_state);
      break;
    case 0x8005:
      SUB(hw_state);
      break;
    case 0x8006:
      SHR(hw_state);
      break;
    case 0x8007:
      SUBN(hw_state);
      break;
    case 0x800E:
      SHL(hw_state);
      break;
    }
    
    // TODO: more opcodes
  }
}

// The SYS instruction is ignored by modern interpreters.
// This function does nothing, but it was good practice.
void SYS() {
}

// Clear the screen
void CLS(struct emu_vars* hw_state) {
  for(int i = 0; i < 64; i++) {
    for(int j = 0; j < 32; j++) {
      hw_state->gfx[i][j] = 0;
    }
  }
}

// Return from subroutine
void RET(struct emu_vars* hw_state) {
  hw_state->pc = hw_state->stack[hw_state->sp];
  hw_state->sp--;
}

// Jump to address
void JP(struct emu_vars* hw_state) {
  hw_state->pc = hw_state->opcode & 0x0FFF;
}

// Call subroutine
void CALL(struct emu_vars* hw_state) {
  hw_state->sp++;
  hw_state->stack[hw_state->sp] = hw_state->pc;
  hw_state->pc = hw_state->opcode & 0x0FFF;
}

// Skip next instruction if V[x] = kk or if V[x] = V[y]
void SE(struct emu_vars* hw_state) {
  if((hw_state->opcode & 0xF000) == 0x3000) {
    uint8_t vx = (hw_state->V[(hw_state->opcode & 0x0F00) >> 8]);
    uint8_t kk = (hw_state->opcode & 0x00FF);
    if(vx == kk) {
      hw_state->pc += 2;
    }
  } else if((hw_state->opcode & 0xF00F) == 0x5000) {
    uint8_t x = hw_state->V[(hw_state->opcode & 0x0F00) >> 8];
    uint8_t y = hw_state->V[(hw_state->opcode & 0x00F0) >> 4];
    if(x == y) {
      hw_state->pc += 2;
    }
  }
}

// Skip next instruction if V[x] != kk
void SNE(struct emu_vars* hw_state) {
  uint8_t vx = (hw_state->V[(hw_state->opcode & 0x0F00) >> 8]);
  uint8_t kk = (hw_state->opcode & 0x00FF);
  if(vx != kk) {
    hw_state->pc += 2;
  }
}

// Store in register V[x]
void LD(struct emu_vars* hw_state) {
  int x = (hw_state->opcode & 0x0F00) >> 8;
  if((hw_state->opcode & 0xF000) == 0x6000) {
    hw_state->V[x] = hw_state->opcode & 0x00FF;
  } else if((hw_state->opcode & 0xF00F) == 0x8000) {
    int y = (hw_state->opcode & 0x00F0) >> 4;
    hw_state->V[x] = hw_state->V[y];
  }
}

// Add to register V[x], set the carry flag if 0x8xy4 overflows.
void ADD(struct emu_vars* hw_state) {
  int x = (hw_state->opcode & 0x0F00) >> 8;
  if((hw_state->opcode & 0xF000) == 0x7000) {
    hw_state->V[x] += hw_state->opcode & 0x00FF;
  } else if((hw_state->opcode & 0xF00F) == 0x8004) {
    int y = (hw_state->opcode & 0x00F0) >> 4;
    if(hw_state->V[y] > (0xFF - hw_state->V[x])) {
      hw_state->V[0xF] = 1;
    } else {
      hw_state->V[0xF] = 0;
    }
    hw_state->V[x] += hw_state->V[y];
  }
      
}

// Do a bitwise OR on V[x] and V[y], store in V[x]
void OR(struct emu_vars* hw_state) {
  int x = (hw_state->opcode & 0x0F00) >> 8;
  int y = (hw_state->opcode & 0x00F0) >> 4;
  hw_state->V[x] = hw_state->V[x] | hw_state->V[y];
}

// Do a bitwise AND on V[x] and V[y], store in V[x]
void AND(struct emu_vars* hw_state) {
  int x = (hw_state->opcode & 0x0F00) >> 8;
  int y = (hw_state->opcode & 0x00F0) >> 4;
  hw_state->V[x] = hw_state->V[x] & hw_state->V[y];
}

// Do a bitwise XOR on V[x] and V[y], store in V[x]
void XOR(struct emu_vars* hw_state) {
  int x = (hw_state->opcode & 0x0F00) >> 8;
  int y = (hw_state->opcode & 0x00F0) >> 4;
  hw_state->V[x] = hw_state->V[x] ^ hw_state->V[y];
}

// Subtract V[y] from V[x], set borrow flag if necessary, store in V[x]
void SUB(struct emu_vars* hw_state) {
  int x = (hw_state->opcode & 0x0F00) >> 8;
  int y = (hw_state->opcode & 0x00F0) >> 4;
  if(hw_state->V[x] > hw_state->V[y]) {
    hw_state->V[0xF] = 1;
  } else {
    hw_state->V[0xF] = 0;
  }
  hw_state->V[x] -= hw_state->V[y];
}

// If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0.
// Then Vx is divided by 2.
void SHR(struct emu_vars* hw_state) {
  int x = (hw_state->opcode & 0x0F00) >> 8;
  if(hw_state->V[x] % 2 == 0) {
    hw_state->V[0xF] = 0;
  } else {
    hw_state->V[0xF] = 1;
  }
  hw_state->V[x] /= 2;
}

// Subtract V[x] from V[y], set borrow flag if necessary, store in V[x]
void SUBN(struct emu_vars* hw_state) {
  int x = (hw_state->opcode & 0x0F00) >> 8;
  int y = (hw_state->opcode & 0x00F0) >> 4;
  if(hw_state->V[y] > hw_state->V[x]) {
    hw_state->V[0xF] = 1;
  } else {
    hw_state->V[0xF] = 0;
  }
  hw_state->V[x] = hw_state->V[y] - hw_state->V[x];
}

// If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0.
// Then Vx is multiplied by 2.
void SHL(struct emu_vars* hw_state) {
  int x = (hw_state->opcode & 0x0F00) >> 8;
  if(((hw_state->V[x] & 0xF000) >> 12) > 0x7) {
    hw_state->V[0xF] = 1;
  } else {
    hw_state->V[0xF] = 0;
  }
  hw_state->V[x] *= 2;
}
