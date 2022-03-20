#include "instructions.h"
#include "../common/log.h"
#include "../common/bitwise.h"

#define UNIMPLEMENTED() log_fatal("UNIMPLEMENTED INSTRUCTION %02x%02x",ins,a0)

//TODO: whats the difference between rl r and rlc r?

//some helper macros

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


#define F_TOGGLE(v,f,p) F_SET(F()&(~f)|((u8)OZ(v)<<((u8)p)))
#define FZ_SET(v) F_TOGGLE(v,CPU_ZEROFLAG,CPU_ZEROFLAG_POS)
#define FN_SET(v) F_TOGGLE(v,CPU_SUBTRACTIONFLAG,CPU_SUBTRACTIONFLAG_POS)
#define FH_SET(v) F_TOGGLE(v,CPU_HALFCARRYFLAG,CPU_HALFCARRYFLAG_POS)
#define FC_SET(v) F_TOGGLE(v,CPU_CARRYFLAG,CPU_CARRYFLAG_POS)


#define CARRY_4(a,b) (((u16)(a&0x0f)+(u16)(b&0x0f))>0x0f)
#define CARRY_8(a,b) (((u16)((u8)a)+(u16)((u8)b))>0xff)
#define CARRY_16(a,b) (((u32)a+(u32)b)>0xffff)

#define CARRY_4_SUB(a,b) (((s16)a+(s16)b)<0)
#define CARRY_8_SUB(a,b) (((s16)a+(s16)b)<0)
#define CARRY_16_SUB(a,b) (((s32)a+(s32)b)<0)

void stack_push_u8(CPU *cpu, u8 d){
    M_SET(SP(),d);
    SP_SET(SP()-1);
}

void stack_push_u16(CPU *cpu, u16 d){
    M_SET(SP(),d&0xff);
    M_SET(SP(),d>>8);
    SP_SET(SP()-2);
}

u8 stack_pop_u8(CPU *cpu){
    SP_SET(SP()+1);
    return M_GET(SP());
}

u16 stack_pop_u16(CPU *cpu){
    SP_SET(SP()+2);
    return (u16)M_GET(SP()) | ((u16)M_GET(SP()) << 8);
}

//gimme generics
u16 rotl_u16(u16 v, int s){
    return (v<<s) | (v>>(sizeof (v)*8-s));
}

u8 rotl_u8(u8 v, int s){
    return (v<<s) | (v>>(sizeof (v)*8-s));
}

u16 rotr_u16(u16 v, int s){
    return (v>>s) | (v<<(sizeof (v)*8-s));
}

u8 rotr_u8(u8 v, int s){
    return (v>>s) | (v<<(sizeof (v)*8-s));
}

//NOP  | Z:- N:- H:- C:-
void ins_00(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){}

//LD BC,d16 | Z:- N:- H:- C:-
void ins_01(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    BC_SET(D16());
}

//LD (BC),A | Z:- N:- H:- C:-
void ins_02(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(BC(),A());
}

//INC BC | Z:- N:- H:- C:-
void ins_03(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//INC B | Z:Z N:0 H:H C:-
void ins_04(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//DEC B | Z:Z N:1 H:H C:-
void ins_05(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FH_SET(CARRY_4_SUB(B(),1));
    B_SET(B()-1);
    FN_SET(1);FZ_SET(B()==0);
}

//LD B,d8 | Z:- N:- H:- C:-
void ins_06(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    B_SET(D8());
}

//RLCA  | Z:0 N:0 H:0 C:C
void ins_07(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//LD (a16),SP | Z:- N:- H:- C:-
void ins_08(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(A16(),SP());
}

//ADD HL,BC | Z:- N:0 H:H C:C
void ins_09(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//LD A,(BC) | Z:- N:- H:- C:-
void ins_0A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(M_GET(BC()));
}

//DEC BC | Z:- N:- H:- C:-
void ins_0B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    BC_SET(BC()-1);
}

//INC C | Z:Z N:0 H:H C:-
void ins_0C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FH_SET(CARRY_4(C(),1));
    C_SET(C()+1);
    FZ_SET(C()); FN_SET(0);
}

//DEC C | Z:Z N:1 H:H C:-
void ins_0D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FH_SET(CARRY_4_SUB(C(),-1));
    C_SET(C()-1);
    FN_SET(1);FZ_SET(C()==0);
}

//LD C,d8 | Z:- N:- H:- C:-
void ins_0E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    C_SET(D8());
}

//RRCA  | Z:0 N:0 H:0 C:C
void ins_0F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//STOP d8 | Z:- N:- H:- C:-
void ins_10(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//LD DE,d16 | Z:- N:- H:- C:-
void ins_11(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    DE_SET(D16());
}

//LD (DE),A | Z:- N:- H:- C:-
void ins_12(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(DE(),A());
}

//INC DE | Z:- N:- H:- C:-
void ins_13(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//INC D | Z:Z N:0 H:H C:-
void ins_14(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//DEC D | Z:Z N:1 H:H C:-
void ins_15(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FH_SET(CARRY_4_SUB(D(),-1));
    D_SET(D()-1);
    FN_SET(1);FZ_SET(D()==0);
}

//LD D,d8 | Z:- N:- H:- C:-
void ins_16(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    D_SET(D8());
}

//RLA  | Z:0 N:0 H:0 C:C
void ins_17(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 o = A();
    u8 v = rotl_u8(o,1);
    A_SET(v);
    FZ_SET(0);FN_SET(0);FH_SET(0);FH_SET(o&80);
}

//JR r8 | Z:- N:- H:- C:-
void ins_18(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//ADD HL,DE | Z:- N:0 H:H C:C
void ins_19(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//LD A,(DE) | Z:- N:- H:- C:-
void ins_1A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(M_GET(DE()));
}

//DEC DE | Z:- N:- H:- C:-
void ins_1B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    DE_SET(DE()-1);
}

//INC E | Z:Z N:0 H:H C:-
void ins_1C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//DEC E | Z:Z N:1 H:H C:-
void ins_1D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FH_SET(CARRY_4_SUB(E(),-1));
    E_SET(E()-1);
    FN_SET(1);FZ_SET(E()==0);
}

//LD E,d8 | Z:- N:- H:- C:-
void ins_1E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(D8());
}

//RRA  | Z:0 N:0 H:0 C:C
void ins_1F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//JR NZ,r8 | Z:- N:- H:- C:-
void ins_20(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    if(!FZ()){
        PC_SET(PC()+R8());
    }
}

//LD HL,d16 | Z:- N:- H:- C:-
void ins_21(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){ HL_SET(JU16(a0, a1));}

//LD (HL+),A | Z:- N:- H:- C:-
void ins_22(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(),A());
    HL_SET(HL()-1);
}

//INC HL | Z:- N:- H:- C:-
void ins_23(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//INC H | Z:Z N:0 H:H C:-
void ins_24(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//DEC H | Z:Z N:1 H:H C:-
void ins_25(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FH_SET(CARRY_4_SUB(H(),-1));
    H_SET(H()-1);
    FN_SET(1);FZ_SET(H()==0);
}

//LD H,d8 | Z:- N:- H:- C:-
void ins_26(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    H_SET(D8());
}

//DAA  | Z:Z N:- H:0 C:C
void ins_27(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//JR Z,r8 | Z:- N:- H:- C:-
void ins_28(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//ADD HL,HL | Z:- N:0 H:H C:C
void ins_29(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//LD A,(HL+) | Z:- N:- H:- C:-
void ins_2A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(M_GET(HL()));
    HL_SET(HL()+1);
}

//DEC HL | Z:- N:- H:- C:-
void ins_2B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    HL_SET(HL()-1);
}

//INC L | Z:Z N:0 H:H C:-
void ins_2C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//DEC L | Z:Z N:1 H:H C:-
void ins_2D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FH_SET(CARRY_4_SUB(L(),-1));
    L_SET(L()-1);
    FN_SET(0);FZ_SET(L()==0);
}

//LD L,d8 | Z:- N:- H:- C:-
void ins_2E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    L_SET(D8());
}

//CPL  | Z:- N:1 H:1 C:-
void ins_2F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()^0xff);
    FN_SET(1);FH_SET(1);
}

//JR NC,r8 | Z:- N:- H:- C:-
void ins_30(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//LD SP,d16 | Z:- N:- H:- C:-
void ins_31(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){ SP_SET(D16());}

//LD (HL-),A | Z:- N:- H:- C:-
void ins_32(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){M_SET(HL(), A());HL_SET(HL() - 1);}

//INC SP | Z:- N:- H:- C:-
void ins_33(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    SP_SET(SP()+1);
}

//INC (HL) | Z:Z N:0 H:H C:-
void ins_34(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 c = M_GET(HL());
    FH_SET(CARRY_4(c,1));
    M_SET(HL(),c+1);
    FN_SET(0);FZ_SET((c+1)==0);
}

//DEC (HL) | Z:Z N:1 H:H C:-
void ins_35(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FH_SET(CARRY_4_SUB(M_GET(HL()),-1));
    M_SET(HL(),M_GET(HL())-1);
    FN_SET(0);FZ_SET(M_GET(HL())==0);
}

//LD (HL),d8 | Z:- N:- H:- C:-
void ins_36(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    HL_SET(D8());
}

//SCF  | Z:- N:0 H:0 C:1
void ins_37(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//JR C,r8 | Z:- N:- H:- C:-
void ins_38(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//ADD HL,SP | Z:- N:0 H:H C:C
void ins_39(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//LD A,(HL-) | Z:- N:- H:- C:-
void ins_3A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(M_GET(HL()));
    HL_SET(HL()-1);
}

//DEC SP | Z:- N:- H:- C:-
void ins_3B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    SP_SET(SP()-1);
}

//INC A | Z:Z N:0 H:H C:-
void ins_3C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//DEC A | Z:Z N:1 H:H C:-
void ins_3D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FH_SET(CARRY_4_SUB(A(),1));
    A_SET(A()-1);
    FN_SET(1);FZ_SET(A()==0);
}

//LD A,d8 | Z:- N:- H:- C:-
void ins_3E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(D8());
}

//CCF  | Z:- N:0 H:0 C:C
void ins_3F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//LD B,B | Z:- N:- H:- C:-
void ins_40(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){

}

//LD B,C | Z:- N:- H:- C:-
void ins_41(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    B_SET(C());
}

//LD B,D | Z:- N:- H:- C:-
void ins_42(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    B_SET(D());
}

//LD B,E | Z:- N:- H:- C:-
void ins_43(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    B_SET(E());
}

//LD B,H | Z:- N:- H:- C:-
void ins_44(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    B_SET(H());
}

//LD B,L | Z:- N:- H:- C:-
void ins_45(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    B_SET(L());
}

//LD B,(HL) | Z:- N:- H:- C:-
void ins_46(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    B_SET(M_GET(HL()));
}

//LD B,A | Z:- N:- H:- C:-
void ins_47(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    B_SET(A());
}

//LD C,B | Z:- N:- H:- C:-
void ins_48(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    C_SET(B());
}

//LD C,C | Z:- N:- H:- C:-
void ins_49(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){

}

//LD C,D | Z:- N:- H:- C:-
void ins_4A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    C_SET(D());
}

//LD C,E | Z:- N:- H:- C:-
void ins_4B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    C_SET(E());
}

//LD C,H | Z:- N:- H:- C:-
void ins_4C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    C_SET(H());
}

//LD C,L | Z:- N:- H:- C:-
void ins_4D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    C_SET(L());
}

//LD C,(HL) | Z:- N:- H:- C:-
void ins_4E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    C_SET(M_GET(HL()));
}

//LD C,A | Z:- N:- H:- C:-
void ins_4F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    C_SET(A());
}

//LD D,B | Z:- N:- H:- C:-
void ins_50(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    D_SET(B());
}

//LD D,C | Z:- N:- H:- C:-
void ins_51(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    D_SET(C());
}

//LD D,D | Z:- N:- H:- C:-
void ins_52(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){

}

//LD D,E | Z:- N:- H:- C:-
void ins_53(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    D_SET(E());
}

//LD D,H | Z:- N:- H:- C:-
void ins_54(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    D_SET(H());
}

//LD D,L | Z:- N:- H:- C:-
void ins_55(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    D_SET(L());
}

//LD D,(HL) | Z:- N:- H:- C:-
void ins_56(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    D_SET(M_GET(HL()));
}

//LD D,A | Z:- N:- H:- C:-
void ins_57(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    D_SET(A());
}

//LD E,B | Z:- N:- H:- C:-
void ins_58(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(B());
}

//LD E,C | Z:- N:- H:- C:-
void ins_59(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(C());
}

//LD E,D | Z:- N:- H:- C:-
void ins_5A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(D());
}

//LD E,E | Z:- N:- H:- C:-
void ins_5B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){

}

//LD E,H | Z:- N:- H:- C:-
void ins_5C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(H());
}

//LD E,L | Z:- N:- H:- C:-
void ins_5D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(L());
}

//LD E,(HL) | Z:- N:- H:- C:-
void ins_5E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(M_GET(HL()));
}

//LD E,A | Z:- N:- H:- C:-
void ins_5F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    E_SET(A());
}

//LD H,B | Z:- N:- H:- C:-
void ins_60(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    H_SET(B());
}

//LD H,C | Z:- N:- H:- C:-
void ins_61(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    H_SET(C());
}

//LD H,D | Z:- N:- H:- C:-
void ins_62(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    H_SET(D());
}

//LD H,E | Z:- N:- H:- C:-
void ins_63(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    H_SET(E());
}

//LD H,H | Z:- N:- H:- C:-
void ins_64(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){

}

//LD H,L | Z:- N:- H:- C:-
void ins_65(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    H_SET(L());
}

//LD H,(HL) | Z:- N:- H:- C:-
void ins_66(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    H_SET(M_GET(HL()));
}

//LD H,A | Z:- N:- H:- C:-
void ins_67(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    H_SET(A());
}

//LD L,B | Z:- N:- H:- C:-
void ins_68(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    L_SET(B());
}

//LD L,C | Z:- N:- H:- C:-
void ins_69(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    L_SET(C());
}

//LD L,D | Z:- N:- H:- C:-
void ins_6A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    L_SET(D());
}

//LD L,E | Z:- N:- H:- C:-
void ins_6B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    L_SET(E());
}

//LD L,H | Z:- N:- H:- C:-
void ins_6C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    L_SET(H());
}

//LD L,L | Z:- N:- H:- C:-
void ins_6D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){

}

//LD L,(HL) | Z:- N:- H:- C:-
void ins_6E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    L_SET(M_GET(HL()));
}

//LD L,A | Z:- N:- H:- C:-
void ins_6F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    L_SET(A());
}

//LD (HL),B | Z:- N:- H:- C:-
void ins_70(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(),B());
}

//LD (HL),C | Z:- N:- H:- C:-
void ins_71(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(),C());
}

//LD (HL),D | Z:- N:- H:- C:-
void ins_72(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(),D());
}

//LD (HL),E | Z:- N:- H:- C:-
void ins_73(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(),E());
}

//LD (HL),H | Z:- N:- H:- C:-
void ins_74(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(),H());
}

//LD (HL),L | Z:- N:- H:- C:-
void ins_75(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(),L());
}

//HALT  | Z:- N:- H:- C:-
void ins_76(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//LD (HL),A | Z:- N:- H:- C:-
void ins_77(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(HL(),A());
}

//LD A,B | Z:- N:- H:- C:-
void ins_78(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(B());
}

//LD A,C | Z:- N:- H:- C:-
void ins_79(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(C());
}

//LD A,D | Z:- N:- H:- C:-
void ins_7A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(D());
}

//LD A,E | Z:- N:- H:- C:-
void ins_7B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(E());
}

//LD A,H | Z:- N:- H:- C:-
void ins_7C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(H());
}

//LD A,L | Z:- N:- H:- C:-
void ins_7D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(L());
}

//LD A,(HL) | Z:- N:- H:- C:-
void ins_7E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(M_GET(HL()));
}

//LD A,A | Z:- N:- H:- C:-
void ins_7F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){

}

//ADD A,B | Z:Z N:0 H:H C:C
void ins_80(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//ADD A,C | Z:Z N:0 H:H C:C
void ins_81(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//ADD A,D | Z:Z N:0 H:H C:C
void ins_82(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//ADD A,E | Z:Z N:0 H:H C:C
void ins_83(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//ADD A,H | Z:Z N:0 H:H C:C
void ins_84(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//ADD A,L | Z:Z N:0 H:H C:C
void ins_85(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//ADD A,(HL) | Z:Z N:0 H:H C:C
void ins_86(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//ADD A,A | Z:Z N:0 H:H C:C
void ins_87(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//ADC A,B | Z:Z N:0 H:H C:C
void ins_88(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//ADC A,C | Z:Z N:0 H:H C:C
void ins_89(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//ADC A,D | Z:Z N:0 H:H C:C
void ins_8A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//ADC A,E | Z:Z N:0 H:H C:C
void ins_8B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//ADC A,H | Z:Z N:0 H:H C:C
void ins_8C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//ADC A,L | Z:Z N:0 H:H C:C
void ins_8D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//ADC A,(HL) | Z:Z N:0 H:H C:C
void ins_8E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//ADC A,A | Z:Z N:0 H:H C:C
void ins_8F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SUB B | Z:Z N:1 H:H C:C
void ins_90(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SUB C | Z:Z N:1 H:H C:C
void ins_91(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SUB D | Z:Z N:1 H:H C:C
void ins_92(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SUB E | Z:Z N:1 H:H C:C
void ins_93(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SUB H | Z:Z N:1 H:H C:C
void ins_94(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SUB L | Z:Z N:1 H:H C:C
void ins_95(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SUB (HL) | Z:Z N:1 H:H C:C
void ins_96(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SUB A | Z:1 N:1 H:0 C:0
void ins_97(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SBC A,B | Z:Z N:1 H:H C:C
void ins_98(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SBC A,C | Z:Z N:1 H:H C:C
void ins_99(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SBC A,D | Z:Z N:1 H:H C:C
void ins_9A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SBC A,E | Z:Z N:1 H:H C:C
void ins_9B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SBC A,H | Z:Z N:1 H:H C:C
void ins_9C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SBC A,L | Z:Z N:1 H:H C:C
void ins_9D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SBC A,(HL) | Z:Z N:1 H:H C:C
void ins_9E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SBC A,A | Z:Z N:1 H:H C:C
void ins_9F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//AND B | Z:Z N:0 H:1 C:0
void ins_A0(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()&B());
    FZ_SET(A()==0);FN_SET(0);FH_SET(1);FN_SET(0);
}

//AND C | Z:Z N:0 H:1 C:0
void ins_A1(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()&C());
    FZ_SET(A()==0);FN_SET(0);FH_SET(1);FN_SET(0);
}

//AND D | Z:Z N:0 H:1 C:0
void ins_A2(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()&D());
    FZ_SET(A()==0);FN_SET(0);FH_SET(1);FN_SET(0);
}

//AND E | Z:Z N:0 H:1 C:0
void ins_A3(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()&E());
    FZ_SET(A()==0);FN_SET(0);FH_SET(1);FN_SET(0);
}

//AND H | Z:Z N:0 H:1 C:0
void ins_A4(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()&H());
    FZ_SET(A()==0);FN_SET(0);FH_SET(1);FN_SET(0);
}

//AND L | Z:Z N:0 H:1 C:0
void ins_A5(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()&L());
    FZ_SET(A()==0);FN_SET(0);FH_SET(1);FN_SET(0);
}

//AND (HL) | Z:Z N:0 H:1 C:0
void ins_A6(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()&M_GET(HL()));
    FZ_SET(A()==0);FN_SET(0);FH_SET(1);FN_SET(0);
}

//AND A | Z:Z N:0 H:1 C:0
void ins_A7(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()&A());
    FZ_SET(A()==0);FN_SET(0);FH_SET(1);FN_SET(0);
}

//XOR B | Z:Z N:0 H:0 C:0
void ins_A8(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()^B());
    FZ_SET(A()==0);FN_SET(0);FH_SET(0);FN_SET(0);
}

//XOR C | Z:Z N:0 H:0 C:0
void ins_A9(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()^C());
    FZ_SET(A()==0);FN_SET(0);FH_SET(0);FN_SET(0);
}

//XOR D | Z:Z N:0 H:0 C:0
void ins_AA(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()^D());
    FZ_SET(A()==0);FN_SET(0);FH_SET(0);FN_SET(0);
}

//XOR E | Z:Z N:0 H:0 C:0
void ins_AB(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()^E());
    FZ_SET(A()==0);FN_SET(0);FH_SET(0);FN_SET(0);
}

//XOR H | Z:Z N:0 H:0 C:0
void ins_AC(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()^H());
    FZ_SET(A()==0);FN_SET(0);FH_SET(0);FN_SET(0);
}

//XOR L | Z:Z N:0 H:0 C:0
void ins_AD(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()^L());
    FZ_SET(A()==0);FN_SET(0);FH_SET(0);FN_SET(0);
}

//XOR (HL) | Z:Z N:0 H:0 C:0
void ins_AE(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()^M_GET(HL()));
    FZ_SET(A()==0);FN_SET(0);FH_SET(0);FN_SET(0);
}

//XOR A | Z:1 N:0 H:0 C:0
void ins_AF(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A() ^ A());
    FZ_SET(1); FN_SET(0); FH_SET(0); FC_SET(0);
}

//OR B | Z:Z N:0 H:0 C:0
void ins_B0(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()|B());
    FZ_SET(A()==0); FN_SET(0); FH_SET(0); FC_SET(0);
}

//OR C | Z:Z N:0 H:0 C:0
void ins_B1(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()|C());
    FZ_SET(A()==0); FN_SET(0); FH_SET(0); FC_SET(0);
}

//OR D | Z:Z N:0 H:0 C:0
void ins_B2(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()|D());
    FZ_SET(A()==0); FN_SET(0); FH_SET(0); FC_SET(0);
}

//OR E | Z:Z N:0 H:0 C:0
void ins_B3(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()|E());
    FZ_SET(A()==0); FN_SET(0); FH_SET(0); FC_SET(0);
}

//OR H | Z:Z N:0 H:0 C:0
void ins_B4(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()|H());
    FZ_SET(A()==0); FN_SET(0); FH_SET(0); FC_SET(0);
}

//OR L | Z:Z N:0 H:0 C:0
void ins_B5(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()|L());
    FZ_SET(A()==0); FN_SET(0); FH_SET(0); FC_SET(0);
}

//OR (HL) | Z:Z N:0 H:0 C:0
void ins_B6(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()|M_GET(HL()));
    FZ_SET(A()==0); FN_SET(0); FH_SET(0); FC_SET(0);
}

//OR A | Z:Z N:0 H:0 C:0
void ins_B7(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()|A());
    FZ_SET(A()==0); FN_SET(0); FH_SET(0); FC_SET(0);
}

//CP B | Z:Z N:1 H:H C:C
void ins_B8(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(A()!=B()); FN_SET(0); FH_SET(0); FC_SET(0);
}

//CP C | Z:Z N:1 H:H C:C
void ins_B9(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 sub = A()-C();
    FZ_SET(sub == 0);FN_SET(1);FH_SET(CARRY_4_SUB(A(),C()));FC_SET(CARRY_8_SUB(A(),C()));
}

//CP D | Z:Z N:1 H:H C:C
void ins_BA(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 sub = A()-D();
    FZ_SET(sub == 0);FN_SET(1);FH_SET(CARRY_4_SUB(A(),D()));FC_SET(CARRY_8_SUB(A(),D()));
}

//CP E | Z:Z N:1 H:H C:C
void ins_BB(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 sub = A()-E();
    FZ_SET(sub == 0);FN_SET(1);FH_SET(CARRY_4_SUB(A(),E()));FC_SET(CARRY_8_SUB(A(),E()));
}

//CP H | Z:Z N:1 H:H C:C
void ins_BC(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 sub = A()-H();
    FZ_SET(sub == 0);FN_SET(1);FH_SET(CARRY_4_SUB(A(),H()));FC_SET(CARRY_8_SUB(A(),H()));
}

//CP L | Z:Z N:1 H:H C:C
void ins_BD(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 sub = A()-L();
    FZ_SET(sub == 0);FN_SET(1);FH_SET(CARRY_4_SUB(A(),L()));FC_SET(CARRY_8_SUB(A(),L()));
}

//CP (HL) | Z:Z N:1 H:H C:C
void ins_BE(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 m = M_GET(HL());
    u8 sub = A()-m;
    FZ_SET(sub == 0);FN_SET(1);FH_SET(CARRY_4_SUB(A(),m));FC_SET(CARRY_8_SUB(A(),m));
}

//CP A | Z:1 N:1 H:0 C:0
void ins_BF(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(1);FN_SET(1);FH_SET(0);FC_SET(0);
}

//RET NZ | Z:- N:- H:- C:-
void ins_C0(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//POP BC | Z:- N:- H:- C:-
void ins_C1(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    BC_SET(stack_pop_u16(cpu));
}

//JP NZ,a16 | Z:- N:- H:- C:-
void ins_C2(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//JP a16 | Z:- N:- H:- C:-
void ins_C3(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//CALL NZ,a16 | Z:- N:- H:- C:-
void ins_C4(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//PUSH BC | Z:- N:- H:- C:-
void ins_C5(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    stack_push_u16(cpu,BC());
}

//ADD A,d8 | Z:Z N:0 H:H C:C
void ins_C6(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RST 00H | Z:- N:- H:- C:-
void ins_C7(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RET Z | Z:- N:- H:- C:-
void ins_C8(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RET  | Z:- N:- H:- C:-
void ins_C9(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//JP Z,a16 | Z:- N:- H:- C:-
void ins_CA(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//PREFIX  | Z:- N:- H:- C:-
void ins_CB(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//CALL Z,a16 | Z:- N:- H:- C:-
void ins_CC(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//CALL a16 | Z:- N:- H:- C:-
void ins_CD(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    stack_push_u16(cpu,PC());
    PC_SET(A16());
}

//ADC A,d8 | Z:Z N:0 H:H C:C
void ins_CE(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RST 08H | Z:- N:- H:- C:-
void ins_CF(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RET NC | Z:- N:- H:- C:-
void ins_D0(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//POP DE | Z:- N:- H:- C:-
void ins_D1(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    DE_SET(stack_pop_u16(cpu));
}

//JP NC,a16 | Z:- N:- H:- C:-
void ins_D2(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//ILLEGAL_D3  | Z:- N:- H:- C:-
void ins_D3(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//CALL NC,a16 | Z:- N:- H:- C:-
void ins_D4(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//PUSH DE | Z:- N:- H:- C:-
void ins_D5(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    stack_push_u16(cpu,DE());
}

//SUB d8 | Z:Z N:1 H:H C:C
void ins_D6(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RST 10H | Z:- N:- H:- C:-
void ins_D7(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RET C | Z:- N:- H:- C:-
void ins_D8(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RETI  | Z:- N:- H:- C:-
void ins_D9(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//JP C,a16 | Z:- N:- H:- C:-
void ins_DA(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//ILLEGAL_DB  | Z:- N:- H:- C:-
void ins_DB(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//CALL C,a16 | Z:- N:- H:- C:-
void ins_DC(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//ILLEGAL_DD  | Z:- N:- H:- C:-
void ins_DD(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SBC A,d8 | Z:Z N:1 H:H C:C
void ins_DE(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RST 18H | Z:- N:- H:- C:-
void ins_DF(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//LDH (a8),A | Z:- N:- H:- C:-
void ins_E0(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(0xff00+D8(), A());
}

//POP HL | Z:- N:- H:- C:-
void ins_E1(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    HL_SET(stack_pop_u16(cpu));
}

//LD (C),A | Z:- N:- H:- C:-
void ins_E2(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(C(),A());
}

//ILLEGAL_E3  | Z:- N:- H:- C:-
void ins_E3(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//ILLEGAL_E4  | Z:- N:- H:- C:-
void ins_E4(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//PUSH HL | Z:- N:- H:- C:-
void ins_E5(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    stack_push_u16(cpu,HL());
}

//AND d8 | Z:Z N:0 H:1 C:0
void ins_E6(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()&D8());
    FZ_SET(A()==0);FN_SET(0);FH_SET(1);FN_SET(0);
}

//RST 20H | Z:- N:- H:- C:-
void ins_E7(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//ADD SP,r8 | Z:0 N:0 H:H C:C
void ins_E8(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//JP HL | Z:- N:- H:- C:-
void ins_E9(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//LD (a16),A | Z:- N:- H:- C:-
void ins_EA(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    M_SET(A16(),A());
}

//ILLEGAL_EB  | Z:- N:- H:- C:-
void ins_EB(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//ILLEGAL_EC  | Z:- N:- H:- C:-
void ins_EC(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//ILLEGAL_ED  | Z:- N:- H:- C:-
void ins_ED(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//XOR d8 | Z:Z N:0 H:0 C:0
void ins_EE(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()^D8());
    FZ_SET(A()==0);FN_SET(0);FH_SET(0);FN_SET(0);
}

//RST 28H | Z:- N:- H:- C:-
void ins_EF(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//LDH A,(a8) | Z:- N:- H:- C:-
void ins_F0(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(M_GET(0xff00+D8()));
}

//POP AF | Z:Z N:N H:H C:C
void ins_F1(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    AF_SET(stack_pop_u16(cpu));
}

//LD A,(C) | Z:- N:- H:- C:-
void ins_F2(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(M_GET(C()));
}

//DI  | Z:- N:- H:- C:-
void ins_F3(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//ILLEGAL_F4  | Z:- N:- H:- C:-
void ins_F4(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//PUSH AF | Z:- N:- H:- C:-
void ins_F5(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//OR d8 | Z:Z N:0 H:0 C:0
void ins_F6(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(A()|D8());
    FZ_SET(A()==0); FN_SET(0); FH_SET(0); FC_SET(0);
}

//RST 30H | Z:- N:- H:- C:-
void ins_F7(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//LD HL,SP+r8 | Z:0 N:0 H:H C:C
void ins_F8(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    HL_SET(SP()+R8());
}

//LD SP,HL | Z:- N:- H:- C:-
void ins_F9(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    SP_SET(HL());
}

//LD A,(a16) | Z:- N:- H:- C:-
void ins_FA(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    A_SET(M_GET(A16()));
}

//EI  | Z:- N:- H:- C:-
void ins_FB(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//ILLEGAL_FC  | Z:- N:- H:- C:-
void ins_FC(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//ILLEGAL_FD  | Z:- N:- H:- C:-
void ins_FD(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//CP d8 | Z:Z N:1 H:H C:C
void ins_FE(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 sub = A()-D8();
    FZ_SET(sub == 0);FN_SET(1);FH_SET(CARRY_4_SUB(A(),D8()));FC_SET(CARRY_8_SUB(A(),D8()));
}

//RST 38H | Z:- N:- H:- C:-
void ins_FF(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}


//RLC B | Z:Z N:0 H:0 C:C
void ins_CB_00(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 o = B();
    u8 v = rotl_u8(o,1);
    B_SET(v);
    FZ_SET(v==0);FN_SET(0);FH_SET(0);FC_SET(o&80);
}

//RLC C | Z:Z N:0 H:0 C:C
void ins_CB_01(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 o = C();
    u8 v = rotl_u8(o,1);
    C_SET(v);
    FZ_SET(v==0);FN_SET(0);FH_SET(0);FC_SET(o&80);
}

//RLC D | Z:Z N:0 H:0 C:C
void ins_CB_02(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 o = E();
    u8 v = rotl_u8(o,1);
    E_SET(v);
    FZ_SET(v==0);FN_SET(0);FH_SET(0);FC_SET(o&80);
}

//RLC E | Z:Z N:0 H:0 C:C
void ins_CB_03(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 o = E();
    u8 v = rotl_u8(o,1);
    E_SET(v);
    FZ_SET(v==0);FN_SET(0);FH_SET(0);FC_SET(o&80);
}

//RLC H | Z:Z N:0 H:0 C:C
void ins_CB_04(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 o = H();
    u8 v = rotl_u8(o,1);
    H_SET(v);
    FZ_SET(v==0);FN_SET(0);FH_SET(0);FC_SET(o&80);
}

//RLC L | Z:Z N:0 H:0 C:C
void ins_CB_05(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 o = L();
    u8 v = rotl_u8(o,1);
    L_SET(v);
    FZ_SET(v==0);FN_SET(0);FH_SET(0);FC_SET(o&80);
}

//RLC (HL) | Z:Z N:0 H:0 C:C
void ins_CB_06(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 o = M_GET(HL());
    u8 v = rotl_u8(o,1);
    M_SET(HL(),v);
    FZ_SET(v==0);FN_SET(0);FH_SET(0);FC_SET(o&80);
}

//RLC A | Z:Z N:0 H:0 C:C
void ins_CB_07(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 o = A();
    u8 v = rotl_u8(o,1);
    A_SET(v);
    FZ_SET(v==0);FN_SET(0);FH_SET(0);FC_SET(o&80);
}

//RRC B | Z:Z N:0 H:0 C:C
void ins_CB_08(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RRC C | Z:Z N:0 H:0 C:C
void ins_CB_09(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RRC D | Z:Z N:0 H:0 C:C
void ins_CB_0A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RRC E | Z:Z N:0 H:0 C:C
void ins_CB_0B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RRC H | Z:Z N:0 H:0 C:C
void ins_CB_0C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RRC L | Z:Z N:0 H:0 C:C
void ins_CB_0D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RRC (HL) | Z:Z N:0 H:0 C:C
void ins_CB_0E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 o = M_GET(HL());
    u8 v = rotr_u8(o,1);
    M_SET(HL(),v);
    FZ_SET(v==0);FN_SET(0);FH_SET(0);FC_SET(o&1);
}

//RRC A | Z:Z N:0 H:0 C:C
void ins_CB_0F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RL B | Z:Z N:0 H:0 C:C
void ins_CB_10(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 o = B();
    u8 v = rotl_u8(o,1);
    B_SET(v);
    FZ_SET(v==0);FN_SET(0);FH_SET(0);FC_SET(o&80);
}

//RL C | Z:Z N:0 H:0 C:C
void ins_CB_11(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 o = C();
    u8 v = rotl_u8(o,1);
    C_SET(v);
    FZ_SET(v==0);FN_SET(0);FH_SET(0);FC_SET(o&80);
}

//RL D | Z:Z N:0 H:0 C:C
void ins_CB_12(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 o = D();
    u8 v = rotl_u8(o,1);
    D_SET(v);
    FZ_SET(v==0);FN_SET(0);FH_SET(0);FC_SET(o&80);
}

//RL E | Z:Z N:0 H:0 C:C
void ins_CB_13(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 o = E();
    u8 v = rotl_u8(o,1);
    E_SET(v);
    FZ_SET(v==0);FN_SET(0);FH_SET(0);FC_SET(o&80);
}

//RL H | Z:Z N:0 H:0 C:C
void ins_CB_14(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 o = H();
    u8 v = rotl_u8(o,1);
    H_SET(v);
    FZ_SET(v==0);FN_SET(0);FH_SET(0);FC_SET(o&80);
}

//RL L | Z:Z N:0 H:0 C:C
void ins_CB_15(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 o = L();
    u8 v = rotl_u8(o,1);
    L_SET(v);
    FZ_SET(v==0);FN_SET(0);FH_SET(0);FC_SET(o&80);
}

//RL (HL) | Z:Z N:0 H:0 C:C
void ins_CB_16(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 o = M_GET(HL());
    u8 v = rotl_u8(o,1);
    M_SET(HL(),v);
    FZ_SET(v==0);FN_SET(0);FH_SET(0);FC_SET(o&80);
}

//RL A | Z:Z N:0 H:0 C:C
void ins_CB_17(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    u8 o = A();
    u8 v = rotl_u8(o,1);
    A_SET(v);
    FZ_SET(v==0);FN_SET(0);FH_SET(0);FC_SET(o&80);
}

//RR B | Z:Z N:0 H:0 C:C
void ins_CB_18(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RR C | Z:Z N:0 H:0 C:C
void ins_CB_19(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RR D | Z:Z N:0 H:0 C:C
void ins_CB_1A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RR E | Z:Z N:0 H:0 C:C
void ins_CB_1B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RR H | Z:Z N:0 H:0 C:C
void ins_CB_1C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RR L | Z:Z N:0 H:0 C:C
void ins_CB_1D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RR (HL) | Z:Z N:0 H:0 C:C
void ins_CB_1E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RR A | Z:Z N:0 H:0 C:C
void ins_CB_1F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SLA B | Z:Z N:0 H:0 C:C
void ins_CB_20(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SLA C | Z:Z N:0 H:0 C:C
void ins_CB_21(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SLA D | Z:Z N:0 H:0 C:C
void ins_CB_22(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SLA E | Z:Z N:0 H:0 C:C
void ins_CB_23(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SLA H | Z:Z N:0 H:0 C:C
void ins_CB_24(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SLA L | Z:Z N:0 H:0 C:C
void ins_CB_25(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SLA (HL) | Z:Z N:0 H:0 C:C
void ins_CB_26(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SLA A | Z:Z N:0 H:0 C:C
void ins_CB_27(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SRA B | Z:Z N:0 H:0 C:C
void ins_CB_28(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SRA C | Z:Z N:0 H:0 C:C
void ins_CB_29(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SRA D | Z:Z N:0 H:0 C:C
void ins_CB_2A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SRA E | Z:Z N:0 H:0 C:C
void ins_CB_2B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SRA H | Z:Z N:0 H:0 C:C
void ins_CB_2C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SRA L | Z:Z N:0 H:0 C:C
void ins_CB_2D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SRA (HL) | Z:Z N:0 H:0 C:C
void ins_CB_2E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SRA A | Z:Z N:0 H:0 C:C
void ins_CB_2F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SWAP B | Z:Z N:0 H:0 C:0
void ins_CB_30(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SWAP C | Z:Z N:0 H:0 C:0
void ins_CB_31(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SWAP D | Z:Z N:0 H:0 C:0
void ins_CB_32(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SWAP E | Z:Z N:0 H:0 C:0
void ins_CB_33(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SWAP H | Z:Z N:0 H:0 C:0
void ins_CB_34(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SWAP L | Z:Z N:0 H:0 C:0
void ins_CB_35(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SWAP (HL) | Z:Z N:0 H:0 C:0
void ins_CB_36(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SWAP A | Z:Z N:0 H:0 C:0
void ins_CB_37(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SRL B | Z:Z N:0 H:0 C:C
void ins_CB_38(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SRL C | Z:Z N:0 H:0 C:C
void ins_CB_39(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SRL D | Z:Z N:0 H:0 C:C
void ins_CB_3A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SRL E | Z:Z N:0 H:0 C:C
void ins_CB_3B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SRL H | Z:Z N:0 H:0 C:C
void ins_CB_3C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SRL L | Z:Z N:0 H:0 C:C
void ins_CB_3D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SRL (HL) | Z:Z N:0 H:0 C:C
void ins_CB_3E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SRL A | Z:Z N:0 H:0 C:C
void ins_CB_3F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//BIT 0,B | Z:Z N:0 H:1 C:-
void ins_CB_40(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(B()&(1<<0)); FN_SET(0); FH_SET(1);
}

//BIT 0,C | Z:Z N:0 H:1 C:-
void ins_CB_41(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(C()&(1<<0)); FN_SET(0); FH_SET(1);
}

//BIT 0,D | Z:Z N:0 H:1 C:-
void ins_CB_42(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(D()&(1<<0)); FN_SET(0); FH_SET(1);
}

//BIT 0,E | Z:Z N:0 H:1 C:-
void ins_CB_43(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(E()&(1<<0)); FN_SET(0); FH_SET(1);
}

//BIT 0,H | Z:Z N:0 H:1 C:-
void ins_CB_44(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(H()&(1<<0)); FN_SET(0); FH_SET(1);
}

//BIT 0,L | Z:Z N:0 H:1 C:-
void ins_CB_45(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(L()&(1<<0)); FN_SET(0); FH_SET(1);
}

//BIT 0,(HL) | Z:Z N:0 H:1 C:-
void ins_CB_46(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(M_GET(HL())&(1<<0)); FN_SET(0); FH_SET(1);
}

//BIT 0,A | Z:Z N:0 H:1 C:-
void ins_CB_47(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(A()&(1<<0)); FN_SET(0); FH_SET(1);
}

//BIT 1,B | Z:Z N:0 H:1 C:-
void ins_CB_48(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(B()&(1<<1)); FN_SET(0); FH_SET(1);
}

//BIT 1,C | Z:Z N:0 H:1 C:-
void ins_CB_49(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(C()&(1<<1)); FN_SET(0); FH_SET(1);
}

//BIT 1,D | Z:Z N:0 H:1 C:-
void ins_CB_4A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(D()&(1<<1)); FN_SET(0); FH_SET(1);
}

//BIT 1,E | Z:Z N:0 H:1 C:-
void ins_CB_4B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(E()&(1<<1)); FN_SET(0); FH_SET(1);
}

//BIT 1,H | Z:Z N:0 H:1 C:-
void ins_CB_4C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(H()&(1<<1)); FN_SET(0); FH_SET(1);
}

//BIT 1,L | Z:Z N:0 H:1 C:-
void ins_CB_4D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(L()&(1<<1)); FN_SET(0); FH_SET(1);
}

//BIT 1,(HL) | Z:Z N:0 H:1 C:-
void ins_CB_4E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(M_GET(HL())&(1<<1)); FN_SET(0); FH_SET(1);
}

//BIT 1,A | Z:Z N:0 H:1 C:-
void ins_CB_4F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(A()&(1<<1)); FN_SET(0); FH_SET(1);
}

//BIT 2,B | Z:Z N:0 H:1 C:-
void ins_CB_50(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(B()&(1<<2)); FN_SET(0); FH_SET(1);
}

//BIT 2,C | Z:Z N:0 H:1 C:-
void ins_CB_51(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(C()&(1<<2)); FN_SET(0); FH_SET(1);
}

//BIT 2,D | Z:Z N:0 H:1 C:-
void ins_CB_52(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(D()&(1<<2)); FN_SET(0); FH_SET(1);
}

//BIT 2,E | Z:Z N:0 H:1 C:-
void ins_CB_53(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(E()&(1<<2)); FN_SET(0); FH_SET(1);
}

//BIT 2,H | Z:Z N:0 H:1 C:-
void ins_CB_54(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(H()&(1<<2)); FN_SET(0); FH_SET(1);
}

//BIT 2,L | Z:Z N:0 H:1 C:-
void ins_CB_55(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(L()&(1<<2)); FN_SET(0); FH_SET(1);
}

//BIT 2,(HL) | Z:Z N:0 H:1 C:-
void ins_CB_56(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(M_GET(HL())&(1<<2)); FN_SET(0); FH_SET(1);
}

//BIT 2,A | Z:Z N:0 H:1 C:-
void ins_CB_57(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(A()&(1<<2)); FN_SET(0); FH_SET(1);
}

//BIT 3,B | Z:Z N:0 H:1 C:-
void ins_CB_58(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(B()&(1<<3)); FN_SET(0); FH_SET(1);
}

//BIT 3,C | Z:Z N:0 H:1 C:-
void ins_CB_59(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(C()&(1<<3)); FN_SET(0); FH_SET(1);
}

//BIT 3,D | Z:Z N:0 H:1 C:-
void ins_CB_5A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(D()&(1<<3)); FN_SET(0); FH_SET(1);
}

//BIT 3,E | Z:Z N:0 H:1 C:-
void ins_CB_5B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(E()&(1<<3)); FN_SET(0); FH_SET(1);
}

//BIT 3,H | Z:Z N:0 H:1 C:-
void ins_CB_5C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(H()&(1<<3)); FN_SET(0); FH_SET(1);
}

//BIT 3,L | Z:Z N:0 H:1 C:-
void ins_CB_5D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(L()&(1<<3)); FN_SET(0); FH_SET(1);
}

//BIT 3,(HL) | Z:Z N:0 H:1 C:-
void ins_CB_5E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(M_GET(HL())&(1<<3)); FN_SET(0); FH_SET(1);
}

//BIT 3,A | Z:Z N:0 H:1 C:-
void ins_CB_5F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(A()&(1<<3)); FN_SET(0); FH_SET(1);
}

//BIT 4,B | Z:Z N:0 H:1 C:-
void ins_CB_60(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(B()&(1<<4)); FN_SET(0); FH_SET(1);
}

//BIT 4,C | Z:Z N:0 H:1 C:-
void ins_CB_61(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(C()&(1<<4)); FN_SET(0); FH_SET(1);
}

//BIT 4,D | Z:Z N:0 H:1 C:-
void ins_CB_62(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(D()&(1<<4)); FN_SET(0); FH_SET(1);
}

//BIT 4,E | Z:Z N:0 H:1 C:-
void ins_CB_63(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(E()&(1<<4)); FN_SET(0); FH_SET(1);
}

//BIT 4,H | Z:Z N:0 H:1 C:-
void ins_CB_64(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(H()&(1<<4)); FN_SET(0); FH_SET(1);
}

//BIT 4,L | Z:Z N:0 H:1 C:-
void ins_CB_65(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(L()&(1<<4)); FN_SET(0); FH_SET(1);
}

//BIT 4,(HL) | Z:Z N:0 H:1 C:-
void ins_CB_66(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(M_GET(HL())&(1<<4)); FN_SET(0); FH_SET(1);
}

//BIT 4,A | Z:Z N:0 H:1 C:-
void ins_CB_67(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(A()&(1<<4)); FN_SET(0); FH_SET(1);
}

//BIT 5,B | Z:Z N:0 H:1 C:-
void ins_CB_68(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(B()&(1<<5)); FN_SET(0); FH_SET(1);
}

//BIT 5,C | Z:Z N:0 H:1 C:-
void ins_CB_69(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(C()&(1<<5)); FN_SET(0); FH_SET(1);
}

//BIT 5,D | Z:Z N:0 H:1 C:-
void ins_CB_6A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(D()&(1<<5)); FN_SET(0); FH_SET(1);
}

//BIT 5,E | Z:Z N:0 H:1 C:-
void ins_CB_6B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(E()&(1<<5)); FN_SET(0); FH_SET(1);
}

//BIT 5,H | Z:Z N:0 H:1 C:-
void ins_CB_6C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(H()&(1<<5)); FN_SET(0); FH_SET(1);
}

//BIT 5,L | Z:Z N:0 H:1 C:-
void ins_CB_6D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(L()&(1<<5)); FN_SET(0); FH_SET(1);
}

//BIT 5,(HL) | Z:Z N:0 H:1 C:-
void ins_CB_6E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(M_GET(HL())&(1<<5)); FN_SET(0); FH_SET(1);
}

//BIT 5,A | Z:Z N:0 H:1 C:-
void ins_CB_6F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(A()&(1<<5)); FN_SET(0); FH_SET(1);
}

//BIT 6,B | Z:Z N:0 H:1 C:-
void ins_CB_70(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(B()&(1<<6)); FN_SET(0); FH_SET(1);
}

//BIT 6,C | Z:Z N:0 H:1 C:-
void ins_CB_71(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(C()&(1<<6)); FN_SET(0); FH_SET(1);
}

//BIT 6,D | Z:Z N:0 H:1 C:-
void ins_CB_72(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(D()&(1<<6)); FN_SET(0); FH_SET(1);
}

//BIT 6,E | Z:Z N:0 H:1 C:-
void ins_CB_73(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(E()&(1<<6)); FN_SET(0); FH_SET(1);
}

//BIT 6,H | Z:Z N:0 H:1 C:-
void ins_CB_74(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(H()&(1<<6)); FN_SET(0); FH_SET(1);
}

//BIT 6,L | Z:Z N:0 H:1 C:-
void ins_CB_75(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(L()&(1<<6)); FN_SET(0); FH_SET(1);
}

//BIT 6,(HL) | Z:Z N:0 H:1 C:-
void ins_CB_76(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(M_GET(HL())&(1<<6)); FN_SET(0); FH_SET(1);
}

//BIT 6,A | Z:Z N:0 H:1 C:-
void ins_CB_77(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(A()&(1<<6)); FN_SET(0); FH_SET(1);
}

//BIT 7,B | Z:Z N:0 H:1 C:-
void ins_CB_78(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(B()&(1<<7)); FN_SET(0); FH_SET(1);
}

//BIT 7,C | Z:Z N:0 H:1 C:-
void ins_CB_79(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(C()&(1<<7)); FN_SET(0); FH_SET(1);
}

//BIT 7,D | Z:Z N:0 H:1 C:-
void ins_CB_7A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(D()&(1<<7)); FN_SET(0); FH_SET(1);
}

//BIT 7,E | Z:Z N:0 H:1 C:-
void ins_CB_7B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(E()&(1<<7)); FN_SET(0); FH_SET(1);
}

//BIT 7,H | Z:Z N:0 H:1 C:-
void ins_CB_7C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(H()&(1<<7)); FN_SET(0); FH_SET(1);
}

//BIT 7,L | Z:Z N:0 H:1 C:-
void ins_CB_7D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(L()&(1<<7)); FN_SET(0); FH_SET(1);
}

//BIT 7,(HL) | Z:Z N:0 H:1 C:-
void ins_CB_7E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(M_GET(HL())&(1<<7)); FN_SET(0); FH_SET(1);
}

//BIT 7,A | Z:Z N:0 H:1 C:-
void ins_CB_7F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    FZ_SET(A()&(1<<7)); FN_SET(0); FH_SET(1);
}

//RES 0,B | Z:- N:- H:- C:-
void ins_CB_80(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 0,C | Z:- N:- H:- C:-
void ins_CB_81(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 0,D | Z:- N:- H:- C:-
void ins_CB_82(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 0,E | Z:- N:- H:- C:-
void ins_CB_83(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 0,H | Z:- N:- H:- C:-
void ins_CB_84(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 0,L | Z:- N:- H:- C:-
void ins_CB_85(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 0,(HL) | Z:- N:- H:- C:-
void ins_CB_86(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 0,A | Z:- N:- H:- C:-
void ins_CB_87(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 1,B | Z:- N:- H:- C:-
void ins_CB_88(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 1,C | Z:- N:- H:- C:-
void ins_CB_89(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 1,D | Z:- N:- H:- C:-
void ins_CB_8A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 1,E | Z:- N:- H:- C:-
void ins_CB_8B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 1,H | Z:- N:- H:- C:-
void ins_CB_8C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 1,L | Z:- N:- H:- C:-
void ins_CB_8D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 1,(HL) | Z:- N:- H:- C:-
void ins_CB_8E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 1,A | Z:- N:- H:- C:-
void ins_CB_8F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 2,B | Z:- N:- H:- C:-
void ins_CB_90(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 2,C | Z:- N:- H:- C:-
void ins_CB_91(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 2,D | Z:- N:- H:- C:-
void ins_CB_92(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 2,E | Z:- N:- H:- C:-
void ins_CB_93(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 2,H | Z:- N:- H:- C:-
void ins_CB_94(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 2,L | Z:- N:- H:- C:-
void ins_CB_95(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 2,(HL) | Z:- N:- H:- C:-
void ins_CB_96(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 2,A | Z:- N:- H:- C:-
void ins_CB_97(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 3,B | Z:- N:- H:- C:-
void ins_CB_98(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 3,C | Z:- N:- H:- C:-
void ins_CB_99(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 3,D | Z:- N:- H:- C:-
void ins_CB_9A(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 3,E | Z:- N:- H:- C:-
void ins_CB_9B(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 3,H | Z:- N:- H:- C:-
void ins_CB_9C(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 3,L | Z:- N:- H:- C:-
void ins_CB_9D(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 3,(HL) | Z:- N:- H:- C:-
void ins_CB_9E(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 3,A | Z:- N:- H:- C:-
void ins_CB_9F(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 4,B | Z:- N:- H:- C:-
void ins_CB_A0(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 4,C | Z:- N:- H:- C:-
void ins_CB_A1(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 4,D | Z:- N:- H:- C:-
void ins_CB_A2(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 4,E | Z:- N:- H:- C:-
void ins_CB_A3(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 4,H | Z:- N:- H:- C:-
void ins_CB_A4(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 4,L | Z:- N:- H:- C:-
void ins_CB_A5(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 4,(HL) | Z:- N:- H:- C:-
void ins_CB_A6(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 4,A | Z:- N:- H:- C:-
void ins_CB_A7(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 5,B | Z:- N:- H:- C:-
void ins_CB_A8(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 5,C | Z:- N:- H:- C:-
void ins_CB_A9(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 5,D | Z:- N:- H:- C:-
void ins_CB_AA(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 5,E | Z:- N:- H:- C:-
void ins_CB_AB(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 5,H | Z:- N:- H:- C:-
void ins_CB_AC(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 5,L | Z:- N:- H:- C:-
void ins_CB_AD(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 5,(HL) | Z:- N:- H:- C:-
void ins_CB_AE(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 5,A | Z:- N:- H:- C:-
void ins_CB_AF(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 6,B | Z:- N:- H:- C:-
void ins_CB_B0(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 6,C | Z:- N:- H:- C:-
void ins_CB_B1(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 6,D | Z:- N:- H:- C:-
void ins_CB_B2(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 6,E | Z:- N:- H:- C:-
void ins_CB_B3(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 6,H | Z:- N:- H:- C:-
void ins_CB_B4(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 6,L | Z:- N:- H:- C:-
void ins_CB_B5(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 6,(HL) | Z:- N:- H:- C:-
void ins_CB_B6(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 6,A | Z:- N:- H:- C:-
void ins_CB_B7(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 7,B | Z:- N:- H:- C:-
void ins_CB_B8(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 7,C | Z:- N:- H:- C:-
void ins_CB_B9(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 7,D | Z:- N:- H:- C:-
void ins_CB_BA(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 7,E | Z:- N:- H:- C:-
void ins_CB_BB(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 7,H | Z:- N:- H:- C:-
void ins_CB_BC(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 7,L | Z:- N:- H:- C:-
void ins_CB_BD(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 7,(HL) | Z:- N:- H:- C:-
void ins_CB_BE(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//RES 7,A | Z:- N:- H:- C:-
void ins_CB_BF(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 0,B | Z:- N:- H:- C:-
void ins_CB_C0(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 0,C | Z:- N:- H:- C:-
void ins_CB_C1(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 0,D | Z:- N:- H:- C:-
void ins_CB_C2(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 0,E | Z:- N:- H:- C:-
void ins_CB_C3(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 0,H | Z:- N:- H:- C:-
void ins_CB_C4(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 0,L | Z:- N:- H:- C:-
void ins_CB_C5(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 0,(HL) | Z:- N:- H:- C:-
void ins_CB_C6(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 0,A | Z:- N:- H:- C:-
void ins_CB_C7(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 1,B | Z:- N:- H:- C:-
void ins_CB_C8(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 1,C | Z:- N:- H:- C:-
void ins_CB_C9(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 1,D | Z:- N:- H:- C:-
void ins_CB_CA(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 1,E | Z:- N:- H:- C:-
void ins_CB_CB(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 1,H | Z:- N:- H:- C:-
void ins_CB_CC(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 1,L | Z:- N:- H:- C:-
void ins_CB_CD(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 1,(HL) | Z:- N:- H:- C:-
void ins_CB_CE(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 1,A | Z:- N:- H:- C:-
void ins_CB_CF(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 2,B | Z:- N:- H:- C:-
void ins_CB_D0(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 2,C | Z:- N:- H:- C:-
void ins_CB_D1(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 2,D | Z:- N:- H:- C:-
void ins_CB_D2(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 2,E | Z:- N:- H:- C:-
void ins_CB_D3(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 2,H | Z:- N:- H:- C:-
void ins_CB_D4(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 2,L | Z:- N:- H:- C:-
void ins_CB_D5(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 2,(HL) | Z:- N:- H:- C:-
void ins_CB_D6(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 2,A | Z:- N:- H:- C:-
void ins_CB_D7(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 3,B | Z:- N:- H:- C:-
void ins_CB_D8(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 3,C | Z:- N:- H:- C:-
void ins_CB_D9(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 3,D | Z:- N:- H:- C:-
void ins_CB_DA(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 3,E | Z:- N:- H:- C:-
void ins_CB_DB(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 3,H | Z:- N:- H:- C:-
void ins_CB_DC(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 3,L | Z:- N:- H:- C:-
void ins_CB_DD(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 3,(HL) | Z:- N:- H:- C:-
void ins_CB_DE(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 3,A | Z:- N:- H:- C:-
void ins_CB_DF(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 4,B | Z:- N:- H:- C:-
void ins_CB_E0(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 4,C | Z:- N:- H:- C:-
void ins_CB_E1(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 4,D | Z:- N:- H:- C:-
void ins_CB_E2(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 4,E | Z:- N:- H:- C:-
void ins_CB_E3(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 4,H | Z:- N:- H:- C:-
void ins_CB_E4(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 4,L | Z:- N:- H:- C:-
void ins_CB_E5(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 4,(HL) | Z:- N:- H:- C:-
void ins_CB_E6(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 4,A | Z:- N:- H:- C:-
void ins_CB_E7(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 5,B | Z:- N:- H:- C:-
void ins_CB_E8(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 5,C | Z:- N:- H:- C:-
void ins_CB_E9(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 5,D | Z:- N:- H:- C:-
void ins_CB_EA(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 5,E | Z:- N:- H:- C:-
void ins_CB_EB(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 5,H | Z:- N:- H:- C:-
void ins_CB_EC(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 5,L | Z:- N:- H:- C:-
void ins_CB_ED(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 5,(HL) | Z:- N:- H:- C:-
void ins_CB_EE(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 5,A | Z:- N:- H:- C:-
void ins_CB_EF(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 6,B | Z:- N:- H:- C:-
void ins_CB_F0(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 6,C | Z:- N:- H:- C:-
void ins_CB_F1(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 6,D | Z:- N:- H:- C:-
void ins_CB_F2(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 6,E | Z:- N:- H:- C:-
void ins_CB_F3(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 6,H | Z:- N:- H:- C:-
void ins_CB_F4(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 6,L | Z:- N:- H:- C:-
void ins_CB_F5(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 6,(HL) | Z:- N:- H:- C:-
void ins_CB_F6(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 6,A | Z:- N:- H:- C:-
void ins_CB_F7(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 7,B | Z:- N:- H:- C:-
void ins_CB_F8(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 7,C | Z:- N:- H:- C:-
void ins_CB_F9(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 7,D | Z:- N:- H:- C:-
void ins_CB_FA(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 7,E | Z:- N:- H:- C:-
void ins_CB_FB(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 7,H | Z:- N:- H:- C:-
void ins_CB_FC(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 7,L | Z:- N:- H:- C:-
void ins_CB_FD(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 7,(HL) | Z:- N:- H:- C:-
void ins_CB_FE(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

//SET 7,A | Z:- N:- H:- C:-
void ins_CB_FF(CPU* cpu, u8 ins,u8 a0, u8 a1, u8 a2){
    UNIMPLEMENTED();
}

const CPUInstruction lookup[0x10][0x10] = {
        {{"NOP", 1, 4, ins_00}, {"LD BC,d16", 3, 12, ins_01}, {"LD (BC),A", 1, 8, ins_02}, {"INC BC", 1, 8, ins_03}, {"INC B", 1, 4, ins_04}, {"DEC B", 1, 4, ins_05}, {"LD B,d8", 2, 8, ins_06}, {"RLCA", 1, 4, ins_07}, {"LD (a16),SP", 3, 20, ins_08}, {"ADD HL,BC", 1, 8, ins_09}, {"LD A,(BC)", 1, 8, ins_0A}, {"DEC BC", 1, 8, ins_0B}, {"INC C", 1, 4, ins_0C}, {"DEC C", 1, 4, ins_0D}, {"LD C,d8", 2, 8, ins_0E}, {"RRCA", 1, 4, ins_0F}, },
        {{"STOP d8", 2, 4, ins_10}, {"LD DE,d16", 3, 12, ins_11}, {"LD (DE),A", 1, 8, ins_12}, {"INC DE", 1, 8, ins_13}, {"INC D", 1, 4, ins_14}, {"DEC D", 1, 4, ins_15}, {"LD D,d8", 2, 8, ins_16}, {"RLA", 1, 4, ins_17}, {"JR r8", 2, 12, ins_18}, {"ADD HL,DE", 1, 8, ins_19}, {"LD A,(DE)", 1, 8, ins_1A}, {"DEC DE", 1, 8, ins_1B}, {"INC E", 1, 4, ins_1C}, {"DEC E", 1, 4, ins_1D}, {"LD E,d8", 2, 8, ins_1E}, {"RRA", 1, 4, ins_1F}, },
        {{"JR NZ,r8", 2, 12, ins_20}, {"LD HL,d16", 3, 12, ins_21}, {"LD (HL+),A", 1, 8, ins_22}, {"INC HL", 1, 8, ins_23}, {"INC H", 1, 4, ins_24}, {"DEC H", 1, 4, ins_25}, {"LD H,d8", 2, 8, ins_26}, {"DAA", 1, 4, ins_27}, {"JR Z,r8", 2, 12, ins_28}, {"ADD HL,HL", 1, 8, ins_29}, {"LD A,(HL+)", 1, 8, ins_2A}, {"DEC HL", 1, 8, ins_2B}, {"INC L", 1, 4, ins_2C}, {"DEC L", 1, 4, ins_2D}, {"LD L,d8", 2, 8, ins_2E}, {"CPL", 1, 4, ins_2F}, },
        {{"JR NC,r8", 2, 12, ins_30}, {"LD SP,d16", 3, 12, ins_31}, {"LD (HL-),A", 1, 8, ins_32}, {"INC SP", 1, 8, ins_33}, {"INC (HL)", 1, 12, ins_34}, {"DEC (HL)", 1, 12, ins_35}, {"LD (HL),d8", 2, 12, ins_36}, {"SCF", 1, 4, ins_37}, {"JR C,r8", 2, 12, ins_38}, {"ADD HL,SP", 1, 8, ins_39}, {"LD A,(HL-)", 1, 8, ins_3A}, {"DEC SP", 1, 8, ins_3B}, {"INC A", 1, 4, ins_3C}, {"DEC A", 1, 4, ins_3D}, {"LD A,d8", 2, 8, ins_3E}, {"CCF", 1, 4, ins_3F}, },
        {{"LD B,B", 1, 4, ins_40}, {"LD B,C", 1, 4, ins_41}, {"LD B,D", 1, 4, ins_42}, {"LD B,E", 1, 4, ins_43}, {"LD B,H", 1, 4, ins_44}, {"LD B,L", 1, 4, ins_45}, {"LD B,(HL)", 1, 8, ins_46}, {"LD B,A", 1, 4, ins_47}, {"LD C,B", 1, 4, ins_48}, {"LD C,C", 1, 4, ins_49}, {"LD C,D", 1, 4, ins_4A}, {"LD C,E", 1, 4, ins_4B}, {"LD C,H", 1, 4, ins_4C}, {"LD C,L", 1, 4, ins_4D}, {"LD C,(HL)", 1, 8, ins_4E}, {"LD C,A", 1, 4, ins_4F}, },
        {{"LD D,B", 1, 4, ins_50}, {"LD D,C", 1, 4, ins_51}, {"LD D,D", 1, 4, ins_52}, {"LD D,E", 1, 4, ins_53}, {"LD D,H", 1, 4, ins_54}, {"LD D,L", 1, 4, ins_55}, {"LD D,(HL)", 1, 8, ins_56}, {"LD D,A", 1, 4, ins_57}, {"LD E,B", 1, 4, ins_58}, {"LD E,C", 1, 4, ins_59}, {"LD E,D", 1, 4, ins_5A}, {"LD E,E", 1, 4, ins_5B}, {"LD E,H", 1, 4, ins_5C}, {"LD E,L", 1, 4, ins_5D}, {"LD E,(HL)", 1, 8, ins_5E}, {"LD E,A", 1, 4, ins_5F}, },
        {{"LD H,B", 1, 4, ins_60}, {"LD H,C", 1, 4, ins_61}, {"LD H,D", 1, 4, ins_62}, {"LD H,E", 1, 4, ins_63}, {"LD H,H", 1, 4, ins_64}, {"LD H,L", 1, 4, ins_65}, {"LD H,(HL)", 1, 8, ins_66}, {"LD H,A", 1, 4, ins_67}, {"LD L,B", 1, 4, ins_68}, {"LD L,C", 1, 4, ins_69}, {"LD L,D", 1, 4, ins_6A}, {"LD L,E", 1, 4, ins_6B}, {"LD L,H", 1, 4, ins_6C}, {"LD L,L", 1, 4, ins_6D}, {"LD L,(HL)", 1, 8, ins_6E}, {"LD L,A", 1, 4, ins_6F}, },
        {{"LD (HL),B", 1, 8, ins_70}, {"LD (HL),C", 1, 8, ins_71}, {"LD (HL),D", 1, 8, ins_72}, {"LD (HL),E", 1, 8, ins_73}, {"LD (HL),H", 1, 8, ins_74}, {"LD (HL),L", 1, 8, ins_75}, {"HALT", 1, 4, ins_76}, {"LD (HL),A", 1, 8, ins_77}, {"LD A,B", 1, 4, ins_78}, {"LD A,C", 1, 4, ins_79}, {"LD A,D", 1, 4, ins_7A}, {"LD A,E", 1, 4, ins_7B}, {"LD A,H", 1, 4, ins_7C}, {"LD A,L", 1, 4, ins_7D}, {"LD A,(HL)", 1, 8, ins_7E}, {"LD A,A", 1, 4, ins_7F}, },
        {{"ADD A,B", 1, 4, ins_80}, {"ADD A,C", 1, 4, ins_81}, {"ADD A,D", 1, 4, ins_82}, {"ADD A,E", 1, 4, ins_83}, {"ADD A,H", 1, 4, ins_84}, {"ADD A,L", 1, 4, ins_85}, {"ADD A,(HL)", 1, 8, ins_86}, {"ADD A,A", 1, 4, ins_87}, {"ADC A,B", 1, 4, ins_88}, {"ADC A,C", 1, 4, ins_89}, {"ADC A,D", 1, 4, ins_8A}, {"ADC A,E", 1, 4, ins_8B}, {"ADC A,H", 1, 4, ins_8C}, {"ADC A,L", 1, 4, ins_8D}, {"ADC A,(HL)", 1, 8, ins_8E}, {"ADC A,A", 1, 4, ins_8F}, },
        {{"SUB B", 1, 4, ins_90}, {"SUB C", 1, 4, ins_91}, {"SUB D", 1, 4, ins_92}, {"SUB E", 1, 4, ins_93}, {"SUB H", 1, 4, ins_94}, {"SUB L", 1, 4, ins_95}, {"SUB (HL)", 1, 8, ins_96}, {"SUB A", 1, 4, ins_97}, {"SBC A,B", 1, 4, ins_98}, {"SBC A,C", 1, 4, ins_99}, {"SBC A,D", 1, 4, ins_9A}, {"SBC A,E", 1, 4, ins_9B}, {"SBC A,H", 1, 4, ins_9C}, {"SBC A,L", 1, 4, ins_9D}, {"SBC A,(HL)", 1, 8, ins_9E}, {"SBC A,A", 1, 4, ins_9F}, },
        {{"AND B", 1, 4, ins_A0}, {"AND C", 1, 4, ins_A1}, {"AND D", 1, 4, ins_A2}, {"AND E", 1, 4, ins_A3}, {"AND H", 1, 4, ins_A4}, {"AND L", 1, 4, ins_A5}, {"AND (HL)", 1, 8, ins_A6}, {"AND A", 1, 4, ins_A7}, {"XOR B", 1, 4, ins_A8}, {"XOR C", 1, 4, ins_A9}, {"XOR D", 1, 4, ins_AA}, {"XOR E", 1, 4, ins_AB}, {"XOR H", 1, 4, ins_AC}, {"XOR L", 1, 4, ins_AD}, {"XOR (HL)", 1, 8, ins_AE}, {"XOR A", 1, 4, ins_AF}, },
        {{"OR B", 1, 4, ins_B0}, {"OR C", 1, 4, ins_B1}, {"OR D", 1, 4, ins_B2}, {"OR E", 1, 4, ins_B3}, {"OR H", 1, 4, ins_B4}, {"OR L", 1, 4, ins_B5}, {"OR (HL)", 1, 8, ins_B6}, {"OR A", 1, 4, ins_B7}, {"CP B", 1, 4, ins_B8}, {"CP C", 1, 4, ins_B9}, {"CP D", 1, 4, ins_BA}, {"CP E", 1, 4, ins_BB}, {"CP H", 1, 4, ins_BC}, {"CP L", 1, 4, ins_BD}, {"CP (HL)", 1, 8, ins_BE}, {"CP A", 1, 4, ins_BF}, },
        {{"RET NZ", 1, 20, ins_C0}, {"POP BC", 1, 12, ins_C1}, {"JP NZ,a16", 3, 16, ins_C2}, {"JP a16", 3, 16, ins_C3}, {"CALL NZ,a16", 3, 24, ins_C4}, {"PUSH BC", 1, 16, ins_C5}, {"ADD A,d8", 2, 8, ins_C6}, {"RST 00H", 1, 16, ins_C7}, {"RET Z", 1, 20, ins_C8}, {"RET", 1, 16, ins_C9}, {"JP Z,a16", 3, 16, ins_CA}, {"PREFIX", 1, 4, ins_CB}, {"CALL Z,a16", 3, 24, ins_CC}, {"CALL a16", 3, 24, ins_CD}, {"ADC A,d8", 2, 8, ins_CE}, {"RST 08H", 1, 16, ins_CF}, },
        {{"RET NC", 1, 20, ins_D0}, {"POP DE", 1, 12, ins_D1}, {"JP NC,a16", 3, 16, ins_D2}, {"ILLEGAL_D3", 1, 4, ins_D3}, {"CALL NC,a16", 3, 24, ins_D4}, {"PUSH DE", 1, 16, ins_D5}, {"SUB d8", 2, 8, ins_D6}, {"RST 10H", 1, 16, ins_D7}, {"RET C", 1, 20, ins_D8}, {"RETI", 1, 16, ins_D9}, {"JP C,a16", 3, 16, ins_DA}, {"ILLEGAL_DB", 1, 4, ins_DB}, {"CALL C,a16", 3, 24, ins_DC}, {"ILLEGAL_DD", 1, 4, ins_DD}, {"SBC A,d8", 2, 8, ins_DE}, {"RST 18H", 1, 16, ins_DF}, },
        {{"LDH (a8),A", 2, 12, ins_E0}, {"POP HL", 1, 12, ins_E1}, {"LD (C),A", 1, 8, ins_E2}, {"ILLEGAL_E3", 1, 4, ins_E3}, {"ILLEGAL_E4", 1, 4, ins_E4}, {"PUSH HL", 1, 16, ins_E5}, {"AND d8", 2, 8, ins_E6}, {"RST 20H", 1, 16, ins_E7}, {"ADD SP,r8", 2, 16, ins_E8}, {"JP HL", 1, 4, ins_E9}, {"LD (a16),A", 3, 16, ins_EA}, {"ILLEGAL_EB", 1, 4, ins_EB}, {"ILLEGAL_EC", 1, 4, ins_EC}, {"ILLEGAL_ED", 1, 4, ins_ED}, {"XOR d8", 2, 8, ins_EE}, {"RST 28H", 1, 16, ins_EF}, },
        {{"LDH A,(a8)", 2, 12, ins_F0}, {"POP AF", 1, 12, ins_F1}, {"LD A,(C)", 1, 8, ins_F2}, {"DI", 1, 4, ins_F3}, {"ILLEGAL_F4", 1, 4, ins_F4}, {"PUSH AF", 1, 16, ins_F5}, {"OR d8", 2, 8, ins_F6}, {"RST 30H", 1, 16, ins_F7}, {"LD HL,SP+r8", 2, 12, ins_F8}, {"LD SP,HL", 1, 8, ins_F9}, {"LD A,(a16)", 3, 16, ins_FA}, {"EI", 1, 4, ins_FB}, {"ILLEGAL_FC", 1, 4, ins_FC}, {"ILLEGAL_FD", 1, 4, ins_FD}, {"CP d8", 2, 8, ins_FE}, {"RST 38H", 1, 16, ins_FF}}
};

const CPUInstruction lookup_cb[0x10][0x10] = {
        {{"RLC B", 2, 8, ins_CB_00}, {"RLC C", 2, 8, ins_CB_01}, {"RLC D", 2, 8, ins_CB_02}, {"RLC E", 2, 8, ins_CB_03}, {"RLC H", 2, 8, ins_CB_04}, {"RLC L", 2, 8, ins_CB_05}, {"RLC (HL)", 2, 16, ins_CB_06}, {"RLC A", 2, 8, ins_CB_07}, {"RRC B", 2, 8, ins_CB_08}, {"RRC C", 2, 8, ins_CB_09}, {"RRC D", 2, 8, ins_CB_0A}, {"RRC E", 2, 8, ins_CB_0B}, {"RRC H", 2, 8, ins_CB_0C}, {"RRC L", 2, 8, ins_CB_0D}, {"RRC (HL)", 2, 16, ins_CB_0E}, {"RRC A", 2, 8, ins_CB_0F}, },
        {{"RL B", 2, 8, ins_CB_10}, {"RL C", 2, 8, ins_CB_11}, {"RL D", 2, 8, ins_CB_12}, {"RL E", 2, 8, ins_CB_13}, {"RL H", 2, 8, ins_CB_14}, {"RL L", 2, 8, ins_CB_15}, {"RL (HL)", 2, 16, ins_CB_16}, {"RL A", 2, 8, ins_CB_17}, {"RR B", 2, 8, ins_CB_18}, {"RR C", 2, 8, ins_CB_19}, {"RR D", 2, 8, ins_CB_1A}, {"RR E", 2, 8, ins_CB_1B}, {"RR H", 2, 8, ins_CB_1C}, {"RR L", 2, 8, ins_CB_1D}, {"RR (HL)", 2, 16, ins_CB_1E}, {"RR A", 2, 8, ins_CB_1F}, },
        {{"SLA B", 2, 8, ins_CB_20}, {"SLA C", 2, 8, ins_CB_21}, {"SLA D", 2, 8, ins_CB_22}, {"SLA E", 2, 8, ins_CB_23}, {"SLA H", 2, 8, ins_CB_24}, {"SLA L", 2, 8, ins_CB_25}, {"SLA (HL)", 2, 16, ins_CB_26}, {"SLA A", 2, 8, ins_CB_27}, {"SRA B", 2, 8, ins_CB_28}, {"SRA C", 2, 8, ins_CB_29}, {"SRA D", 2, 8, ins_CB_2A}, {"SRA E", 2, 8, ins_CB_2B}, {"SRA H", 2, 8, ins_CB_2C}, {"SRA L", 2, 8, ins_CB_2D}, {"SRA (HL)", 2, 16, ins_CB_2E}, {"SRA A", 2, 8, ins_CB_2F}, },
        {{"SWAP B", 2, 8, ins_CB_30}, {"SWAP C", 2, 8, ins_CB_31}, {"SWAP D", 2, 8, ins_CB_32}, {"SWAP E", 2, 8, ins_CB_33}, {"SWAP H", 2, 8, ins_CB_34}, {"SWAP L", 2, 8, ins_CB_35}, {"SWAP (HL)", 2, 16, ins_CB_36}, {"SWAP A", 2, 8, ins_CB_37}, {"SRL B", 2, 8, ins_CB_38}, {"SRL C", 2, 8, ins_CB_39}, {"SRL D", 2, 8, ins_CB_3A}, {"SRL E", 2, 8, ins_CB_3B}, {"SRL H", 2, 8, ins_CB_3C}, {"SRL L", 2, 8, ins_CB_3D}, {"SRL (HL)", 2, 16, ins_CB_3E}, {"SRL A", 2, 8, ins_CB_3F}, },
        {{"BIT 0,B", 2, 8, ins_CB_40}, {"BIT 0,C", 2, 8, ins_CB_41}, {"BIT 0,D", 2, 8, ins_CB_42}, {"BIT 0,E", 2, 8, ins_CB_43}, {"BIT 0,H", 2, 8, ins_CB_44}, {"BIT 0,L", 2, 8, ins_CB_45}, {"BIT 0,(HL)", 2, 12, ins_CB_46}, {"BIT 0,A", 2, 8, ins_CB_47}, {"BIT 1,B", 2, 8, ins_CB_48}, {"BIT 1,C", 2, 8, ins_CB_49}, {"BIT 1,D", 2, 8, ins_CB_4A}, {"BIT 1,E", 2, 8, ins_CB_4B}, {"BIT 1,H", 2, 8, ins_CB_4C}, {"BIT 1,L", 2, 8, ins_CB_4D}, {"BIT 1,(HL)", 2, 12, ins_CB_4E}, {"BIT 1,A", 2, 8, ins_CB_4F}, },
        {{"BIT 2,B", 2, 8, ins_CB_50}, {"BIT 2,C", 2, 8, ins_CB_51}, {"BIT 2,D", 2, 8, ins_CB_52}, {"BIT 2,E", 2, 8, ins_CB_53}, {"BIT 2,H", 2, 8, ins_CB_54}, {"BIT 2,L", 2, 8, ins_CB_55}, {"BIT 2,(HL)", 2, 12, ins_CB_56}, {"BIT 2,A", 2, 8, ins_CB_57}, {"BIT 3,B", 2, 8, ins_CB_58}, {"BIT 3,C", 2, 8, ins_CB_59}, {"BIT 3,D", 2, 8, ins_CB_5A}, {"BIT 3,E", 2, 8, ins_CB_5B}, {"BIT 3,H", 2, 8, ins_CB_5C}, {"BIT 3,L", 2, 8, ins_CB_5D}, {"BIT 3,(HL)", 2, 12, ins_CB_5E}, {"BIT 3,A", 2, 8, ins_CB_5F}, },
        {{"BIT 4,B", 2, 8, ins_CB_60}, {"BIT 4,C", 2, 8, ins_CB_61}, {"BIT 4,D", 2, 8, ins_CB_62}, {"BIT 4,E", 2, 8, ins_CB_63}, {"BIT 4,H", 2, 8, ins_CB_64}, {"BIT 4,L", 2, 8, ins_CB_65}, {"BIT 4,(HL)", 2, 12, ins_CB_66}, {"BIT 4,A", 2, 8, ins_CB_67}, {"BIT 5,B", 2, 8, ins_CB_68}, {"BIT 5,C", 2, 8, ins_CB_69}, {"BIT 5,D", 2, 8, ins_CB_6A}, {"BIT 5,E", 2, 8, ins_CB_6B}, {"BIT 5,H", 2, 8, ins_CB_6C}, {"BIT 5,L", 2, 8, ins_CB_6D}, {"BIT 5,(HL)", 2, 12, ins_CB_6E}, {"BIT 5,A", 2, 8, ins_CB_6F}, },
        {{"BIT 6,B", 2, 8, ins_CB_70}, {"BIT 6,C", 2, 8, ins_CB_71}, {"BIT 6,D", 2, 8, ins_CB_72}, {"BIT 6,E", 2, 8, ins_CB_73}, {"BIT 6,H", 2, 8, ins_CB_74}, {"BIT 6,L", 2, 8, ins_CB_75}, {"BIT 6,(HL)", 2, 12, ins_CB_76}, {"BIT 6,A", 2, 8, ins_CB_77}, {"BIT 7,B", 2, 8, ins_CB_78}, {"BIT 7,C", 2, 8, ins_CB_79}, {"BIT 7,D", 2, 8, ins_CB_7A}, {"BIT 7,E", 2, 8, ins_CB_7B}, {"BIT 7,H", 2, 8, ins_CB_7C}, {"BIT 7,L", 2, 8, ins_CB_7D}, {"BIT 7,(HL)", 2, 12, ins_CB_7E}, {"BIT 7,A", 2, 8, ins_CB_7F}, },
        {{"RES 0,B", 2, 8, ins_CB_80}, {"RES 0,C", 2, 8, ins_CB_81}, {"RES 0,D", 2, 8, ins_CB_82}, {"RES 0,E", 2, 8, ins_CB_83}, {"RES 0,H", 2, 8, ins_CB_84}, {"RES 0,L", 2, 8, ins_CB_85}, {"RES 0,(HL)", 2, 16, ins_CB_86}, {"RES 0,A", 2, 8, ins_CB_87}, {"RES 1,B", 2, 8, ins_CB_88}, {"RES 1,C", 2, 8, ins_CB_89}, {"RES 1,D", 2, 8, ins_CB_8A}, {"RES 1,E", 2, 8, ins_CB_8B}, {"RES 1,H", 2, 8, ins_CB_8C}, {"RES 1,L", 2, 8, ins_CB_8D}, {"RES 1,(HL)", 2, 16, ins_CB_8E}, {"RES 1,A", 2, 8, ins_CB_8F}, },
        {{"RES 2,B", 2, 8, ins_CB_90}, {"RES 2,C", 2, 8, ins_CB_91}, {"RES 2,D", 2, 8, ins_CB_92}, {"RES 2,E", 2, 8, ins_CB_93}, {"RES 2,H", 2, 8, ins_CB_94}, {"RES 2,L", 2, 8, ins_CB_95}, {"RES 2,(HL)", 2, 16, ins_CB_96}, {"RES 2,A", 2, 8, ins_CB_97}, {"RES 3,B", 2, 8, ins_CB_98}, {"RES 3,C", 2, 8, ins_CB_99}, {"RES 3,D", 2, 8, ins_CB_9A}, {"RES 3,E", 2, 8, ins_CB_9B}, {"RES 3,H", 2, 8, ins_CB_9C}, {"RES 3,L", 2, 8, ins_CB_9D}, {"RES 3,(HL)", 2, 16, ins_CB_9E}, {"RES 3,A", 2, 8, ins_CB_9F}, },
        {{"RES 4,B", 2, 8, ins_CB_A0}, {"RES 4,C", 2, 8, ins_CB_A1}, {"RES 4,D", 2, 8, ins_CB_A2}, {"RES 4,E", 2, 8, ins_CB_A3}, {"RES 4,H", 2, 8, ins_CB_A4}, {"RES 4,L", 2, 8, ins_CB_A5}, {"RES 4,(HL)", 2, 16, ins_CB_A6}, {"RES 4,A", 2, 8, ins_CB_A7}, {"RES 5,B", 2, 8, ins_CB_A8}, {"RES 5,C", 2, 8, ins_CB_A9}, {"RES 5,D", 2, 8, ins_CB_AA}, {"RES 5,E", 2, 8, ins_CB_AB}, {"RES 5,H", 2, 8, ins_CB_AC}, {"RES 5,L", 2, 8, ins_CB_AD}, {"RES 5,(HL)", 2, 16, ins_CB_AE}, {"RES 5,A", 2, 8, ins_CB_AF}, },
        {{"RES 6,B", 2, 8, ins_CB_B0}, {"RES 6,C", 2, 8, ins_CB_B1}, {"RES 6,D", 2, 8, ins_CB_B2}, {"RES 6,E", 2, 8, ins_CB_B3}, {"RES 6,H", 2, 8, ins_CB_B4}, {"RES 6,L", 2, 8, ins_CB_B5}, {"RES 6,(HL)", 2, 16, ins_CB_B6}, {"RES 6,A", 2, 8, ins_CB_B7}, {"RES 7,B", 2, 8, ins_CB_B8}, {"RES 7,C", 2, 8, ins_CB_B9}, {"RES 7,D", 2, 8, ins_CB_BA}, {"RES 7,E", 2, 8, ins_CB_BB}, {"RES 7,H", 2, 8, ins_CB_BC}, {"RES 7,L", 2, 8, ins_CB_BD}, {"RES 7,(HL)", 2, 16, ins_CB_BE}, {"RES 7,A", 2, 8, ins_CB_BF}, },
        {{"SET 0,B", 2, 8, ins_CB_C0}, {"SET 0,C", 2, 8, ins_CB_C1}, {"SET 0,D", 2, 8, ins_CB_C2}, {"SET 0,E", 2, 8, ins_CB_C3}, {"SET 0,H", 2, 8, ins_CB_C4}, {"SET 0,L", 2, 8, ins_CB_C5}, {"SET 0,(HL)", 2, 16, ins_CB_C6}, {"SET 0,A", 2, 8, ins_CB_C7}, {"SET 1,B", 2, 8, ins_CB_C8}, {"SET 1,C", 2, 8, ins_CB_C9}, {"SET 1,D", 2, 8, ins_CB_CA}, {"SET 1,E", 2, 8, ins_CB_CB}, {"SET 1,H", 2, 8, ins_CB_CC}, {"SET 1,L", 2, 8, ins_CB_CD}, {"SET 1,(HL)", 2, 16, ins_CB_CE}, {"SET 1,A", 2, 8, ins_CB_CF}, },
        {{"SET 2,B", 2, 8, ins_CB_D0}, {"SET 2,C", 2, 8, ins_CB_D1}, {"SET 2,D", 2, 8, ins_CB_D2}, {"SET 2,E", 2, 8, ins_CB_D3}, {"SET 2,H", 2, 8, ins_CB_D4}, {"SET 2,L", 2, 8, ins_CB_D5}, {"SET 2,(HL)", 2, 16, ins_CB_D6}, {"SET 2,A", 2, 8, ins_CB_D7}, {"SET 3,B", 2, 8, ins_CB_D8}, {"SET 3,C", 2, 8, ins_CB_D9}, {"SET 3,D", 2, 8, ins_CB_DA}, {"SET 3,E", 2, 8, ins_CB_DB}, {"SET 3,H", 2, 8, ins_CB_DC}, {"SET 3,L", 2, 8, ins_CB_DD}, {"SET 3,(HL)", 2, 16, ins_CB_DE}, {"SET 3,A", 2, 8, ins_CB_DF}, },
        {{"SET 4,B", 2, 8, ins_CB_E0}, {"SET 4,C", 2, 8, ins_CB_E1}, {"SET 4,D", 2, 8, ins_CB_E2}, {"SET 4,E", 2, 8, ins_CB_E3}, {"SET 4,H", 2, 8, ins_CB_E4}, {"SET 4,L", 2, 8, ins_CB_E5}, {"SET 4,(HL)", 2, 16, ins_CB_E6}, {"SET 4,A", 2, 8, ins_CB_E7}, {"SET 5,B", 2, 8, ins_CB_E8}, {"SET 5,C", 2, 8, ins_CB_E9}, {"SET 5,D", 2, 8, ins_CB_EA}, {"SET 5,E", 2, 8, ins_CB_EB}, {"SET 5,H", 2, 8, ins_CB_EC}, {"SET 5,L", 2, 8, ins_CB_ED}, {"SET 5,(HL)", 2, 16, ins_CB_EE}, {"SET 5,A", 2, 8, ins_CB_EF}, },
        {{"SET 6,B", 2, 8, ins_CB_F0}, {"SET 6,C", 2, 8, ins_CB_F1}, {"SET 6,D", 2, 8, ins_CB_F2}, {"SET 6,E", 2, 8, ins_CB_F3}, {"SET 6,H", 2, 8, ins_CB_F4}, {"SET 6,L", 2, 8, ins_CB_F5}, {"SET 6,(HL)", 2, 16, ins_CB_F6}, {"SET 6,A", 2, 8, ins_CB_F7}, {"SET 7,B", 2, 8, ins_CB_F8}, {"SET 7,C", 2, 8, ins_CB_F9}, {"SET 7,D", 2, 8, ins_CB_FA}, {"SET 7,E", 2, 8, ins_CB_FB}, {"SET 7,H", 2, 8, ins_CB_FC}, {"SET 7,L", 2, 8, ins_CB_FD}, {"SET 7,(HL)", 2, 16, ins_CB_FE}, {"SET 7,A", 2, 8, ins_CB_FF},}
};

const CPUInstruction *cpu_fetch_instruction(u8 ins){
    return &lookup[left_nibble(ins)][right_nibble(ins)];
}

const CPUInstruction *cpu_fetch_instruction_cb(u8 a0){
    return &lookup_cb[left_nibble(a0)][right_nibble(a0)];
}