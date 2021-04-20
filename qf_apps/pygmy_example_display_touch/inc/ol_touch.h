//st7789v.h
#ifndef _OL_TOUCH_H
#define _OL_TOUCH_H

#include "stdint.h"

uint8_t touch_read(uint8_t */*,uint8_t *data*/);
uint8_t touch_write(uint8_t *,uint8_t );
uint16_t read_x_data(void);
uint16_t read_y_data(void);
void twi_init (void);
int touchpad_init(void);
#endif
