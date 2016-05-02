#ifndef _chip8_h
#define _chip8_h

struct emu_vars {
  uint16_t opcode;      // Opcodes are 2 bytes long
  uint8_t memory[4096]; // 4K of memory
  uint8_t V[16];        // 16 8-bit registers
  uint16_t I;           // Index register
  uint16_t pc;          // Program counter
  uint8_t gfx[64 * 32]; // Array containing screen state
  uint8_t delay_timer;  // 60 Hz timer register
  uint8_t sound_timer;  // 60 Hz timer that sounds buzzer at 0
  uint16_t stack[16];   // 16 level stack for subroutine jump markers
  uint16_t sp;          // Stack pointer - which level of the stack is used
  uint8_t key[16];      // Keypad state
};

#endif
