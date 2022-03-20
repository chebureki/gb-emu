#include "bus.h"
#include "../common/log.h"

#include <stdlib.h>
#include <stdio.h>

#define UNDEFINED_U8 0xff

//TODO: there isn't really a need for this macro
//inclusive between
#define bw(a,b,c) (b<=a) && (a<=c)

_Bool bus_has_listener_conflict(Bus *b, u16 from, u16 to){
    for(int li=0;li<b->head;li++)
        if(bw(from,b->listeners[li].from,b->listeners[li].to) || bw(to,b->listeners[li].from,b->listeners[li].to))
            return 1;
    return 0;
}

int bus_unmap(Bus *bus, u16 from, u16 to){
    for(int li=0;li<bus->head;li++){
        if (bus->listeners[li].from == from && bus->listeners[li].to == to){
            bus->listeners[li].from = bus->listeners[bus->head].from;
            bus->listeners[li].to = bus->listeners[bus->head].to;
            bus->listeners[li].this = bus->listeners[bus->head].this;
            bus->listeners[li].read_func = bus->listeners[bus->head].read_func;
            bus->listeners[li].write_func = bus->listeners[bus->head].write_func;

            bus->head--;
            return 1;
        }
    }
    log_error("failed to unmap %04x~%04x",from,to);
    return 0;
}

int bus_map(Bus *bus, u16 from, u16 to, void *this, BusReadFunc read_func, BusWriteFunc write_func){
    if(bus->head>=BUS_STACK_CAP){
        log_error("exceeded bus listener cap");
        return 0;
    }

    if(bus_has_listener_conflict(bus,from,to)){
        log_error("listening conflict in bus for range %x to %x",from,to);
        return 0;
    }

    bus->listeners[bus->head++] = (BusListener){.from=from,.to=to,.this=this,.read_func=read_func, .write_func=write_func};
    log_debug("new bus region %04x-%04x",from,to);
    return 1;
}

Bus *bus_new(){
    Bus *bus = calloc(1,sizeof (Bus));
    return bus;
}

void bus_close(Bus *bus){
    free(bus);
}

BusListener *bus_get_listener(Bus *bus, u16 addr){
    for(int li=0;li<bus->head; li++){
        BusListener* l= &bus->listeners[li];
        if (bw(addr,l->from, l->to))
            return l;
    }
    return NULL;
}

u8 bus_read(Bus *bus, u16 addr){
    BusListener *listener = bus_get_listener(bus,addr);
    if(listener){
        if (listener->read_func)
            return listener->read_func(listener->this,addr-listener->from,addr);
        log_error("no way to read from bus in %04x",addr);
    }
    log_error("invalid bus read in %04x",addr);
    return UNDEFINED_U8;
}

void bus_write(Bus *bus, u16 addr, u8 val){
    BusListener *listener = bus_get_listener(bus,addr);
    if(listener){
        if (listener->write_func){
            listener->write_func(listener->this,addr-listener->from,addr, val);
            return;
        }
        log_error("no way to write to bus in %04x, possibly read-only",addr);
    }
    log_error("invalid bus write in %04x",addr);
}
