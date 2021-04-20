#include "ol_touch.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include <eoss3_hal_i2c.h>
#include "eoss3_hal_timer.h"
#include "string.h"
#include "dbg_uart.h"

uint8_t device_address =        0x38;
uint8_t mode =                  0x00; // Defalt value 0x00
uint8_t gest_id =               0x01; // Defalt value 0x00
uint8_t num_touch =             0x02; // Defalt value 0x00
uint8_t xh_pos =                0x03; // Defalt value 0xFF
uint8_t xl_pos =                0x04; // Defalt value 0xFF
uint8_t yh_pos =                0x05; // Defalt value 0xFF
uint8_t yl_pos =                0x06; // Defalt value 0xFF
uint8_t touch_weight =          0x07; // Defalt value 0xFF
uint8_t touch_area =            0x08; // Defalt value 0xFF
uint8_t xh_pos_2 =              0x09; // Defalt value 0xFF
uint8_t xl_pos_2 =              0x0A; // Defalt value 0xFF
uint8_t yh_pos_2 =              0x0B; // Defalt value 0xFF
uint8_t yl_pos_2 =              0x0C; // Defalt value 0xFF
uint8_t touch_weight_2 =        0x0D; // Defalt value 0xFF
uint8_t touch_area_2 =          0x0E; // Defalt value 0xFF
uint8_t thres =                 0x80;
uint8_t th_diff =               0x85;
uint8_t ctrl =                  0x86; // Defalt value 0x01
uint8_t time_enter_monitor =    0x87; // Defalt value 0x0A
uint8_t active_rate =           0x88;
uint8_t monitor_rate =          0x89; // Defalt value 0x28
uint8_t radian_value =          0x91; // Defalt value 0x0A
uint8_t offset_left_right =     0x92; // Defalt value 0x19
uint8_t offset_up_down =        0x93; // Defalt value 0x19
uint8_t distance_left_right =   0x94; // Defalt value 0x19
uint8_t distance_up_down =      0x95; // Defalt value 0x19
uint8_t distance_zoon_in_out =  0x96; // Defalt value 0x32
uint8_t lib_ver_h =             0xA1;
uint8_t lib_ver_l =             0xA2;
uint8_t chip_id =               0xA3; // Defalt value 0x06
uint8_t g_mode =                0xA4; // Defalt value 0x01
uint8_t pwr_mode =              0xA5; // Defalt value 0x00
uint8_t firmvare =              0xA6;
uint8_t panel_id =              0xA8; // Defalt value 0x11
uint8_t release_code_id =       0xAF; // Defalt value 0x01
uint8_t state =                 0xBC; // Defalt value 0x01


uint8_t touch_read(uint8_t *reg/*,uint8_t *data*/)
{
    uint8_t err_code= 0;
    uint8_t data = 0;
    err_code = HAL_I2C_Read(device_address, *reg, &data, 1);
//    printf("err_code %d\n",err_code);
    if (err_code == HAL_OK)
		dbg_str("Success \n");
    return data;
}

uint8_t touch_write(uint8_t *reg,uint8_t value)
{
	uint8_t err_code= 0;
	err_code = HAL_I2C_Write(device_address, *reg, &value, 1);
    return err_code;
}

/* Reading x-data*/
uint16_t read_x_data(void)
{
      uint8_t err_code = 0;
      uint8_t x_data[2] = " ";

      uint16_t xdata = 0;
      memset(x_data,0,2);

      err_code = HAL_I2C_Read(device_address,xh_pos, &x_data[0], 1);
      err_code = HAL_I2C_Read(device_address,xl_pos, &x_data[1], 1);
      xdata =(((x_data[0]&0x0F)<<8)|x_data[1]);

      return xdata;
}

/* Reading y-data*/
uint16_t read_y_data(void)
{
      uint8_t err_code = 0;
      uint8_t y_data[2] = " ";

      uint16_t ydata = 0;
      memset(y_data,0,2);

      err_code = HAL_I2C_Read(device_address,yh_pos, &y_data[0], 1);
      err_code = HAL_I2C_Read(device_address,yl_pos, &y_data[1], 1);
      ydata =(((y_data[0]&0x0F)<<8)|y_data[1]);

      return ydata;
}
/**
 * @brief TWI initialization.
 */
void twi_init (void)
{
	// Already Initialized
}

int touchpad_init(void)
{
    uint8_t data = 0,err_code = 0;
    uint8_t w2_data[2];

    err_code = touch_write(&thres,30);    // Setting Threshold 30
//    HAL_DelayUSec(1500);
    dbg_str_int("thres:",touch_read(&thres));

    err_code = touch_write(&g_mode,0x00);    // Setting gmode 0x00
    dbg_str_int("g_mode:",touch_read(&g_mode));

    err_code = touch_write(&offset_left_right,0x19);    // Setting offset_left_right 0x19
    dbg_str_int("offset_left_right:",touch_read(&offset_left_right));

    err_code = touch_write(&offset_up_down,0x19);    // Setting offset_up_down 0x19
    dbg_str_int("offset_up_down:",touch_read(&offset_up_down));

    err_code = touch_write(&distance_left_right,0x19);    // Setting distance_left_right 0x19
    dbg_str_int("distance_left_right:",touch_read(&distance_left_right));

    err_code = touch_write(&distance_up_down,0x19);    // Setting distance_up_down 0x19
    dbg_str_int("distance_up_down:",touch_read(&distance_up_down));

    dbg_str("touchpad_init Done \n");
    return 0;
}
