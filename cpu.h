#ifndef _cpu_h
#define _cpu_h

int cpu_runcycle(struct emu_vars*);

// CPU instructions:
int SYS(uint16_t);
int CLS();
int RET();

#endif
