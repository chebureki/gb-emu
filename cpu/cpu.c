#include "cpu.h"
#include "instructions.h"
#include "../common/log.h"
#include "../common/bitwise.h"

#include <stdlib.h>

CPU *cpu_new(Bus *bus){
    CPU *cpu = calloc(1,sizeof(CPU));
    cpu->bus = bus;
    return cpu;
}

void cpu_close(CPU *cpu){
    free(cpu);
}

void cpu_clock(CPU *cpu){
    if(cpu->extra_cycles)
        cpu->extra_cycles--;
    else
        cpu->extra_cycles = cpu_next_ins(cpu);
}

#include <unistd.h>
int cpu_next_ins(CPU* cpu){

    //an instruction is 4 bytes long at most
    //leave them uninitialized
    u8 bytes[4] = {0,0,0,0};
    bytes[0] = bus_read(cpu->bus,cpu->PC+0);

    const CPUInstruction *ins = NULL;
    if(bytes[0] == 0xcb){
        bytes[1] = bus_read(cpu->bus,cpu->PC+1);
        ins = cpu_fetch_instruction_cb(bytes[1]);
    } else
        ins = cpu_fetch_instruction(bytes[0]);

    //TODO: this should technically never occur, remove it!
    if(!ins){
        log_fatal("failed to fetch instruction for %02x due to stupid implementation",bytes[0]);
    }

    //skips first byte as we've read it already
    for(int i=1;i<ins->length;i++)
        bytes[i] = bus_read(cpu->bus,cpu->PC+i);

    char debug[16];
    sprintf(debug,ins->mnemonic_format, bytes[1],bytes[2],bytes[3]);
    if(cpu->PC >=0xc){

        //usleep(1000000);
    }
    //log_debug("pc %04x: %s AF: %04x BC: %04x DE: %04x HL: %04x SP: %04x",cpu->PC,debug,cpu->AF,cpu->BC,cpu->DE,cpu->HL,cpu->SP);

    cpu->PC+=ins->length;
    ins->callback(cpu,bytes[0],bytes[1],bytes[2],bytes[3]);

    return ins->cycles;
}