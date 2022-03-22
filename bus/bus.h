#include "../common/datatypes.h"

#ifndef GB_EMU_BUS_H
#define GB_EMU_BUS_H

#define UNDEFINED_U8 0xff

typedef u8(*BusReadFunc)(void *this, u16 addr,u16 abs_addr);
typedef void(*BusWriteFunc)(void *this, u16 addr, u16 abs_addr, u8 val);

typedef struct{
    u16 from;
    u16 to;

    void *this;
    BusReadFunc read_func;
    BusWriteFunc write_func;
}BusListener;

#define BUS_STACK_CAP 16
typedef struct {
    int head;
    BusListener listeners[BUS_STACK_CAP];
}Bus;

Bus *bus_new();
void bus_close(Bus *bus);

//returns 1 if successful
int bus_map(Bus *bus, u16 from, u16 to, void *this, BusReadFunc read_func, BusWriteFunc write_func);

void bus_write(Bus *bus, u16 addr, u8 val);
u8 bus_read(Bus *bus, u16 addr);

#endif //GB_EMU_BUS_H
