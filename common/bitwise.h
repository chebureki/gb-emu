#ifndef GB_EMU_BITWISE_H
#define GB_EMU_BITWISE_H

#define left_nibble(a) (u8)((a)>>4)
#define right_nibble(a) (u8)((a)&0x0f)

//join 2 bytes to 16 bit val
#define JU16(a,b) ((u16)b<<8) | ((u16)a)

#endif //GB_EMU_BITWISE_H
