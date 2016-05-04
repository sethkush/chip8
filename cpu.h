#ifndef _cpu_h
#define _cpu_h

void cpu_runcycle(struct emu_vars*);

// CPU instructions:
void SYS(); //0x0nnn
void CLS(struct emu_vars*);  //0x00E0
void RET(struct emu_vars*);  //0x00EE
void JP(struct emu_vars*);   //0x1nnn
void CALL(struct emu_vars*); //0x2nnn
void SE(struct emu_vars*);   //0x3xkk or 0x5xy0
void SNE(struct emu_vars*);  //0x4xkk
void LD(struct emu_vars*);   //0x6xkk or 0x8xy0
void ADD(struct emu_vars*);  //0x7xkk or 0x8xy4
void OR(struct emu_vars*);   //0x8xy1
void AND(struct emu_vars*);  //0x8xy2
void XOR(struct emu_vars*);  //0x8xy3
void SUB(struct emu_vars*);  //0x8xy5
void SHR(struct emu_vars*);  //0x8xy6
void SUBN(struct emu_vars*); //0x8xy7
void SHL(struct emu_vars*);  //0x8xyE

#endif
