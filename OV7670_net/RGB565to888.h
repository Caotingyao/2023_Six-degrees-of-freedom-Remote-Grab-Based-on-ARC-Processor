#ifndef _RGB565TO888_H
#define _RGB565TO888_H
#define RGB565_RED      0xf800
#define RGB565_GREEN    0x07e0
#define RGB565_BLUE     0x001f

typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;

void Rgb565toRgb888(uint16_t n565Color, uint8_t *out); // change (uint16)RGB565 to (3*uint8)RGB888

#endif