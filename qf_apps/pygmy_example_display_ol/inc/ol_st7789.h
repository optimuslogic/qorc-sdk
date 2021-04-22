//st7789v.h
#ifndef _ST7789V_H
#define _ST7789V_H

#include "stdint.h"

// Set of commands described in ST7789 datasheet.
#define ST7789_NOP         	0x00
#define ST7789_SWRESET     	0x01
#define ST7789_RDDID       	0x04
#define ST7789_RDDST       	0x09

#define ST7789_SLPIN       	0x10
#define ST7789_SLPOUT      	0x11
#define ST7789_PTLON       	0x12
#define ST7789_NORON       	0x13

#define ST7789_RDMODE      	0x0A
#define ST7789_RDMADCTL    	0x0B
#define ST7789_RDPIXFMT    	0x0C
#define ST7789_RDIMGFMT    	0x0D
#define ST7789_RDSELFDIAG  	0x0F

#define ST7789_INVOFF      	0x20
#define ST7789_INVON       	0x21
#define ST7789_GAMMASET    	0x26
#define ST7789_DISPOFF     	0x28
#define ST7789_DISPON      	0x29

#define ST7789_CASET       	0x2A
#define ST7789_RASET       	0x2B
#define ST7789_RAMWR       	0x2C
#define ST7789_RAMRD       	0x2E

#define ST7789_PTLAR       	0x30
#define ST7789_MADCTL      	0x36
#define ST7789_COLMOD      	0x3A

//#define ST7789_FRMCTR1     0xB1
#define ST7789_RGBCTRL     	0xB1
#define ST7789_PORCTR      	0xB2
//#define ST7789_FRMCTR2     0xB2
#define ST7789_FRMCTR3     	0xB3
#define ST7789_INVCTR      	0xB4
#define ST7789_DFUNCTR     	0xB6
#define ST7789_GCTRL       	0xB7

#define ST7789_VCOM        	0xBB

#define ST7789_LCMCTR      	0xC0
//#define ST7789_PWCTR1      0xC0
#define ST7789_PWCTR2      	0xC1
#define ST7789_VDVVRHEN    	0xC2
//#define ST7789_PWCTR3      0xC2
#define ST7789_VRHS        	0xC3
//#define ST7789_PWCTR4      0xC3
#define ST7789_VDVS        	0xC4
//#define ST7789_PWCTR5      0xC4
#define ST7789_VMCTR1      	0xC5
#define ST7789_FRMCTR2     	0xC6
#define ST7789_VMCTR2      	0xC7
#define ST7789_PWCTRSEQ    	0xCB
#define ST7789_PWCTRA      	0xCD
#define ST7789_PWCTRB      	0xCF

#define ST7789_PWCTRL1     	0xD0

#define ST7789_RDID1       	0xDA
#define ST7789_RDID2       	0xDB
#define ST7789_RDID3       	0xDC
#define ST7789_RDID4       	0xDD

#define ST7789_PVGAMCTRL    0xE0
#define ST7789_NVGAMCTRL    0xE1
#define ST7789_DGMCTR1     	0xE2
#define ST7789_DGMCTR2     	0xE3
#define ST7789_SPI2EN      	0xE7
#define ST7789_TIMCTRA     	0xE8
#define ST7789_TIMCTRB     	0xEA

#define ST7789_ENGMCTR     	0xF2
#define ST7789_INCTR       	0xF6
#define ST7789_PUMP        	0xF7

#define ST7789_MADCTL_MY  	0x80
#define ST7789_MADCTL_MX  	0x40
#define ST7789_MADCTL_MV  	0x20
#define ST7789_MADCTL_ML  	0x10
#define ST7789_MADCTL_RGB 	0x00 	//replace BGR with this to get normal coloring
#define ST7789_MADCTL_BGR 	0x08 	//used by default
#define ST7789_MADCTL_MH  	0x04

//Position setting for 240x240 LCD
#define ST7789_240x240_XSTART 0
#define ST7789_240x240_YSTART 80
#define ST7789_TFTWIDTH   240
#define ST7789_TFTHEIGHT  240

// Color definitions
//#define BLACK   0x0000
//#define BLUE    0x001F
//#define RED     0xF800
//#define GREEN   0x07E0
//#define CYAN    0x07FF
//#define MAGENTA 0xF81F
//#define YELLOW  0xFFE0
//#define WHITE   0xFFFF


#define ST7789_DC_PIN GPIO_5

/**
 * @brief Enumerator with available rotations.
 */
typedef enum{
    NRF_LCD_ROTATE_0 = 0,       /**< Rotate 0 degrees, clockwise. */
    NRF_LCD_ROTATE_90,          /**< Rotate 90 degrees, clockwise. */
    NRF_LCD_ROTATE_180,         /**< Rotate 180 degrees, clockwise. */
    NRF_LCD_ROTATE_270          /**< Rotate 270 degrees, clockwise. */
}nrf_lcd_rotation_t;

void ol_display_spi_init(void);
void ol_display_init(void);
void spi_write(const void *, uint32_t);
void write_command(uint8_t);
void write_data(uint8_t);
void write_command_buffered(uint8_t* , uint16_t);
void write_data_buffered(uint8_t *, uint16_t);
void set_addr_window(uint16_t , uint16_t , uint16_t , uint16_t );
void command_list(void);
void st7789_pixel_draw(uint16_t , uint16_t , uint32_t );
void st7789_rect_draw(uint16_t , uint16_t , uint16_t , uint16_t , uint32_t );
//void st7789_flush(const lv_area_t * , lv_color_t * );
void st7789_dummy_display(uint8_t * , uint16_t , uint8_t , uint8_t , uint8_t , uint8_t );
void st7789_rotation_set(nrf_lcd_rotation_t rotation);
void st7789_display_invert(uint8_t );
void st7789_screen_clear(uint32_t);

#endif
