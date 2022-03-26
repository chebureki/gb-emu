#include "helper.h"

void stack_push_u8(CPU *cpu, u8 d){
    M_SET(SP(),d);
    SP_SET(SP()-1);
}

u8 stack_pop_u8(CPU *cpu){
    SP_SET(SP()+1);
    u8 v = M_GET(SP());
    return v;
}

void stack_push_u16(CPU *cpu, u16 d){
    //little endian aren't we?
    stack_push_u8(cpu,(d&0xff00)>>8);
    stack_push_u8(cpu,(d&0x00ff));
}


u16 stack_pop_u16(CPU *cpu){
    u16 v = 0;
    v |= stack_pop_u8(cpu);
    v |= ((u16)stack_pop_u8(cpu))<<8;
    return v;
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

u8 rotr_u8(u8 v, int s) {
    return (v >> s) | (v << (sizeof(v) * 8 - s));
}