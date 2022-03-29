#ifndef GB_EMU_BITWISE_H
#define GB_EMU_BITWISE_H

#define left_nibble(a) (u8)((a)>>4)
#define right_nibble(a) (u8)((a)&0x0f)

//join 2 bytes to 16 bit val, where b is the higher-order byte
#define JU16(a,b) (((u16)(b&0xff)<<8) | ((u16)(a&0xff)))

#define U16(a,b) (((u16)(b&0xff)<<8) | ((u16)(a&0xff)))

#endif //GB_EMU_BITWISE_H
