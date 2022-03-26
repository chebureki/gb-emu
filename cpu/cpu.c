#include "cpu.h"
#include "instructions.h"
#include "helper.h"
#include "../common/log.h"

#include <stdlib.h>

CPU *cpu_new(Bus *bus){
    CPU *cpu = calloc(1,sizeof(CPU));
    cpu->bus = bus;
    return cpu;
}

void cpu_close(CPU *cpu){
    free(cpu);
}

u8 cpu_handle_interrupts(CPU *cpu){
    if(!cpu->IME) //master says no :^(
        return 0;

    u8 flag;
    for(flag=0x01;flag!=0;flag<<=1){
        if(cpu->IF & cpu->IE & flag)
            break;
    }
    if (!flag) {
        return 0; //went through all flags and A: it isn't enabled in IE or not requested in IF
    }
    cpu->IME = 0; //should be re-enabled by reti
    cpu->IF &= ~flag; //disable flag

    u8 interrupt_offset=(flag>>1)<<3; //shift right so 1 becomes 0
    stack_push_u16(cpu,PC());
    PC_SET(0x40+ interrupt_offset);
    cpu->extra_cycles=60;
    return 1;
}

void cpu_clock(CPU *cpu){
    if(cpu->extra_cycles){
        cpu->extra_cycles--;
        return;
    }
    if(cpu_handle_interrupts(cpu))
        return;

    cpu->extra_cycles = cpu_next_ins(cpu);
}

#include <unistd.h>

int flag = 0;
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
    for(int i=1;i<ins->length;i++) {
        bytes[i] = bus_read(cpu->bus,cpu->PC+i);
    }


    /*
    if(cpu->PC >= 0x100)
        toggle =1;
    if(toggle){
        log_debug("%02x%02x%02x%02x pc %04x: %s AF: %04x BC: %04x DE: %04x HL: %04x SP: %04x",bytes[0],bytes[1],bytes[2],bytes[3],cpu->PC,debug,cpu->AF,cpu->BC,cpu->DE,cpu->HL,cpu->SP);
    }
    if(cpu->PC == (0xc129+0x20)){
        log_fatal("NO");
    }
     */

    char debug[16];
    sprintf(debug,ins->mnemonic_format, bytes[1],bytes[2],bytes[3]);
    if(cpu->PC >= 0x100)
        flag = 1;
    if(flag){
        log_debug("%02x%02x%02x%02x pc %04x: %s AF: %04x BC: %04x DE: %04x HL: %04x SP: %04x",bytes[0],bytes[1],bytes[2],bytes[3],cpu->PC,debug,cpu->AF,cpu->BC,cpu->DE,cpu->HL,cpu->SP);
    }
    //log_debug("%02x%02x%02x%02x pc %04x: %s AF: %04x BC: %04x DE: %04x HL: %04x SP: %04x",bytes[0],bytes[1],bytes[2],bytes[3],cpu->PC,debug,cpu->AF,cpu->BC,cpu->DE,cpu->HL,cpu->SP);



    cpu->PC+=ins->length;
    ins->callback(cpu,bytes[0],bytes[1],bytes[2],bytes[3]);
    return ins->cycles;
}