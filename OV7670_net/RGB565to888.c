#include "RGB565to888.h"

void Rgb565toRgb888(uint16_t n565Color, uint8_t *out)
{
	unsigned int n888Color = 0;
 
	// 获取RGB单色，并填充低位
	out[0] = (n565Color & RGB565_RED)    >> 8;  //R
	out[1] = (n565Color & RGB565_GREEN)  >> 3;  //G
	out[2] = (n565Color & RGB565_BLUE)   << 3;  //B
}


