#include "cpu.h"
#include "../common/log.h"

#ifndef GB_EMU_HELPER_H
#define GB_EMU_HELPER_H
//some helper macros

#define LO_GET(a) (u8)((a&0xff00)>>8)
#define HI_GET(a) (u8)(a&0x00ff)

#define LO_SET(a,v) a = (a&0x00ff) | (((u8)v)<<8)
#define HI_SET(a,v) a = (a&0xff00) | ((u8)v)

//one or zero, 255 => 1, 5 => 1, 0=>0
#define OZ(v) ((v)!=0)

//memory get & set
#define M_GET(a) (bus_read(cpu->bus, a))
#define M_SET(a,v) bus_write(cpu->bus, a, v)

//d16 is always in the second two bytes
#define D16() (JU16(a0,a1))
//same for a16
#define A16() (JU16(a0,a1))

#define D8() (a0)
//signed 8 bits
#define R8() (s8)D8()

#define AF() (cpu->AF)
#define AF_SET(v) cpu->AF = v

#define BC() (cpu->BC)
#define BC_SET(v) cpu->BC = v

#define DE() (cpu->DE)
#define DE_SET(v) cpu->DE = v

#define HL() (cpu->HL)
#define HL_SET(v) cpu->HL = v
#define A() (LO_GET(cpu->AF))
#define F() (HI_GET(cpu->AF))
#define A_SET(v) LO_SET(cpu->AF,v)
#define F_SET(v) HI_SET(cpu->AF,v)

#define B() (LO_GET(cpu->BC))
#define C() (HI_GET(cpu->BC))
#define B_SET(v) LO_SET(cpu->BC,v)
#define C_SET(v) HI_SET(cpu->BC,v)

#define D() (LO_GET(cpu->DE))
#define E() (HI_GET(cpu->DE))
#define D_SET(v) LO_SET(cpu->DE,v)
#define E_SET(v) HI_SET(cpu->DE,v)

#define H() (LO_GET(cpu->HL))
#define L() (HI_GET(cpu->HL))
#define H_SET(v) (LO_SET(cpu->HL,v))
#define L_SET(v) (HI_SET(cpu->HL,v))

#define SP() (cpu->SP)
#define SP_SET(v) cpu->SP=v

#define PC() (cpu->PC)
#define PC_SET(v) cpu->PC=v

//flags
#define FZ() (((F())&CPU_ZEROFLAG)>>CPU_ZEROFLAG_POS)
#define FN() (((F())&CPU_SUBTRACTIONFLAG)>>CPU_SUBTRACTIONFLAG_POS)
#define FH() (((F())&CPU_HALFCARRYFLAG)>>CPU_HALFCARRYFLAG_POS)
#define FC() (((F())&CPU_CARRYFLAG)>>CPU_CARRYFLAG_POS)


#define F_TOGGLE(v,f,p) F_SET(F()&(~(f))|((u8)OZ(v)<<((u8)p)))
#define FZ_SET(v) F_TOGGLE(v,CPU_ZEROFLAG,CPU_ZEROFLAG_POS)
#define FN_SET(v) F_TOGGLE(v,CPU_SUBTRACTIONFLAG,CPU_SUBTRACTIONFLAG_POS)
#define FH_SET(v) F_TOGGLE(v,CPU_HALFCARRYFLAG,CPU_HALFCARRYFLAG_POS)
#define FC_SET(v) F_TOGGLE(v,CPU_CARRYFLAG,CPU_CARRYFLAG_POS)


#define CARRY_4(a,b) (((u16)(a&0x0f)+(u16)(b&0x0f))>0x0f)
#define CARRY_8(a,b) (((u16)((u8)a)+(u16)((u8)b))>0xff)
#define CARRY_16(a,b) (((u32)a+(u32)b)>0xffff)

#define CARRY_4C(a,b,c) (((u16)(a&0x0f)+(u16)(b&0x0f)+(u16)(c&0x0f))>0x0f)
#define CARRY_8C(a,b,c) (((u16)((u8)a)+(u16)((u8)b)+(u16)((u8)c))>0xff)
#define CARRY_16C(a,b,c) (((u32)a+(u32)b+(u32)c)>0xffff)

#define CARRY_4_SUB(a,b) (((s16)a-(s16)b)<0)
#define CARRY_8_SUB(a,b) (((s16)a-(s16)b)<0)
#define CARRY_16_SUB(a,b) (((s32)a-(s32)b)<0)

#define CARRY_4_SUBC(a,b,c) (((s16)a-(s16)b-(s16)c)<0)
#define CARRY_8_SUBC(a,b,c) (((s16)a-(s16)b-(s16)c)<0)
#define CARRY_16_SUBC(a,b,c) (((s32)a-(s32)b-(s32)c)<0)

#define SWAP8(a) ((u8)((a&0xf0)>>4)) | ((u8)((a&0x0f)<<4))

void stack_push_u8(CPU *cpu, u8 d);
u8 stack_pop_u8(CPU *cpu);
void stack_push_u16(CPU *cpu, u16 d);
u16 stack_pop_u16(CPU *cpu);
//gimme generics
u16 rotl_u16(u16 v, int s);
u8 rotl_u8(u8 v, int s);
u16 rotr_u16(u16 v, int s);
u8 rotr_u8(u8 v, int s);

#endif //GB_EMU_HELPER_H
