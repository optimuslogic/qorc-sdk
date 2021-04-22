#include "ol_st7789.h"
#include "stdio.h"
#include "stdint.h"
#include "eoss3_hal_spi.h"
#include "eoss3_hal_spi_zero.h"
#include "eoss3_hal_gpio.h"
#include "eoss3_hal_pad_config.h"
#include "eoss3_hal_timer.h"

SPI_HandleTypeDef hSpiHandle;

//variables storing the offset:
uint16_t x_offset = 0;
uint16_t y_offset = 0;

void ol_display_spi_init(void)
{
    	
    PadConfig xPadConf;

	xPadConf.ucPin = PAD_10;
	xPadConf.ucFunc = PAD10_FUNC_SEL_SPI_SENSOR_CLK;
	xPadConf.ucCtrl = PAD_CTRL_SRC_A0;
	xPadConf.ucMode = PAD_MODE_OUTPUT_EN;
	xPadConf.ucPull = PAD_NOPULL;
	xPadConf.ucDrv = PAD_DRV_STRENGHT_4MA;
	xPadConf.ucSpeed = PAD_SLEW_RATE_SLOW;
	xPadConf.ucSmtTrg = PAD_SMT_TRIG_DIS;
	HAL_PAD_Config(&xPadConf);

	xPadConf.ucPin = PAD_9;
	xPadConf.ucFunc = PAD9_FUNC_SEL_SPI_SENSOR_SSn_1;
	xPadConf.ucCtrl = PAD_CTRL_SRC_A0;
	xPadConf.ucMode = PAD_MODE_OUTPUT_EN;
	xPadConf.ucPull = PAD_NOPULL;
	xPadConf.ucDrv = PAD_DRV_STRENGHT_4MA;
	xPadConf.ucSpeed = PAD_SLEW_RATE_SLOW;
	xPadConf.ucSmtTrg = PAD_SMT_TRIG_DIS;
	HAL_PAD_Config(&xPadConf);

	xPadConf.ucPin = PAD_6;
	xPadConf.ucFunc = PAD6_FUNC_SPI_SENSOR_MOSI;
	xPadConf.ucCtrl = PAD_CTRL_SRC_A0;
	xPadConf.ucMode = PAD_MODE_OUTPUT_EN;
	xPadConf.ucPull = PAD_NOPULL;
	xPadConf.ucDrv = PAD_DRV_STRENGHT_4MA;
	xPadConf.ucSpeed = PAD_SLEW_RATE_SLOW;
	xPadConf.ucSmtTrg = PAD_SMT_TRIG_DIS;
	HAL_PAD_Config(&xPadConf);

	xPadConf.ucPin = PAD_21;
	xPadConf.ucFunc = PAD21_FUNC_SEL_GPIO_5;
	xPadConf.ucCtrl = PAD_CTRL_SRC_A0;
	xPadConf.ucMode = PAD_MODE_OUTPUT_EN;
	xPadConf.ucPull = PAD_NOPULL;
	xPadConf.ucDrv = PAD_DRV_STRENGHT_4MA;
	xPadConf.ucSpeed = PAD_SLEW_RATE_SLOW;
	xPadConf.ucSmtTrg = PAD_SMT_TRIG_DIS;
	HAL_PAD_Config(&xPadConf);
}

void ol_display_init(void)
{

	/* Configure spi pins */
	//ol_display_spi_init(); 		//Pins are already configured
		
	/* initialize lcd module */
	//hSpiHandle.Init.ucSPIMode = SPI_MODE_2;
	hSpiHandle.Init.ucSSn = SPI_SLAVE_1_SELECT;
	hSpiHandle.Init.ulCLKPhase = SPI_PHASE_1EDGE;
	hSpiHandle.Init.ulCLKPolarity = SPI_POLARITY_LOW;
	hSpiHandle.Init.ulDataSize = SPI_DATASIZE_8BIT;
	hSpiHandle.Init.ulFirstBit = SPI_FIRSTBIT_MSB;
	//hSpiHandle.ucSP.format( 8,0 );         // 8bit mode3
	//_spi.frequency( 2000000 );  // 2 Mhz SPI clock 
	//_spi.write( 0x00 );         // dummy

	hSpiHandle.Init.ucFreq = SPI_BAUDRATE_8MHZ;//SPI_BAUDRATE_1MHZ;
	hSpiHandle.Init.ucSPIInf = SPI_4_WIRE_MODE;
	//hSpiHandleIx = SPI0_MASTER_SEL;

	if(HAL_SPI_Init_ZERO(&hSpiHandle) != HAL_OK)
	{
		printf("HAL_SPI0_Init failed\r\n");
	}
	else
	{
		printf("HAL_SPI0_Init ok\r\n");
	}

//	uint8_t ucTxBuf[0] = 0;
//	SPI_Enable_ZERO(&hSpiHandle,0);
//	SPI_Enable_ZERO(&hSpiHandle,1);

	HAL_GPIO_Write(ST7789_DC_PIN, 1);


    command_list();

	st7789_rotation_set(2);

//	SPI_Enable_ZERO(&hSpiHandle,1);
//	SPI_Enable_ZERO(&hSpiHandle,0);
}

void spi_write(const void * data, uint32_t size)
{
	//ucTxBuf[0] = 0;
	SPI_Enable_ZERO(&hSpiHandle,0);
//	SPI_Enable_ZERO(&hSpiHandle,1);
	//sleep(50000);
	if(HAL_SPI_Transmit_ZERO(&hSpiHandle,(uint8_t*)data,size) != HAL_OK)
	{
//		printf("spi tx fail\r\n");
	}
	else
	{
//		printf("spi tx ok\r\n");
	}
	SPI_Enable_ZERO(&hSpiHandle,1);

}

void write_command(uint8_t c)
{
	HAL_GPIO_Write(ST7789_DC_PIN, 0);
	spi_write(&c, sizeof(c));
}

void write_data(uint8_t c)
{
	HAL_GPIO_Write(ST7789_DC_PIN, 1);
	spi_write(&c, sizeof(c));
}

void write_command_buffered(uint8_t * c, uint16_t len)
{
	HAL_GPIO_Write(ST7789_DC_PIN, 0);
	spi_write(c, len);
}

void write_data_buffered(uint8_t * c, uint16_t len)
{
	HAL_GPIO_Write(ST7789_DC_PIN, 1);
	spi_write(c, len);
}

void set_addr_window(uint16_t x_0, uint16_t y_0, uint16_t x_1, uint16_t y_1)
{
//    ASSERT(x_0 <= x_1);
//    ASSERT(y_0 <= y_1);

    // Adaption for rotation
    y_0 += y_offset;
    y_1 += y_offset;
    x_0 += x_offset;
    x_1 += x_offset;  

    write_command(ST7789_CASET);
    write_data(x_0 >> 8);
    write_data(x_0);
    write_data(x_1 >> 8);
    write_data(x_1);
    write_command(ST7789_RASET);
    write_data(y_0 >> 8);
    write_data(y_0);
    write_data(y_1 >> 8);
    write_data(y_1);
    write_command(ST7789_RAMWR);
}

void command_list(void)
{
	printf("command_list\r\n");

    //Adafruit's
    /*
    write_command(ST7789_SWRESET); //Software reset
    nrf_delay_ms(150);

    write_command(ST7789_SLPOUT);  //Exit sleep
    nrf_delay_ms(500);

    write_command(ST7789_COLMOD);  //Color mode
    write_data(0x55); //16-bit, 565 RGB
    nrf_delay_ms(10);

    write_command(ST7789_MADCTL);  //Memory Data Access control
    write_data(0x08); //Bottom to top refresh

    write_command(ST7789_CASET);  //Column address set
    write_data(0x00);                                 //x start high byte = 0x00
    write_data(ST7789_240x240_XSTART);                //x start low byte = 0x00
    write_data((240 + ST7789_240x240_XSTART) >> 8);   //x end high byte = 0x00
    write_data((240 + ST7789_240x240_XSTART) & 0xFF - 1); //x end low byte = 0xF0

    write_command(ST7789_RASET);  //Row address set
    write_data(0x00);                                 //x start high byte = 0x00
    write_data(ST7789_240x240_YSTART);                //x start low byte = 0x50
    write_data((240 + ST7789_240x240_YSTART) >> 8);   //x end high byte = 0x01
    write_data((240 + ST7789_240x240_YSTART) & 0xFF - 1); //x end low byte = 0x40

    write_command(ST7789_INVON);  //Screen inversion on
    nrf_delay_ms(10);

    write_command(ST7789_NORON);  //Normal display mode on
    nrf_delay_ms(10);

    write_command(ST7789_DISPON); //Display on
    nrf_delay_ms(500);
    //*/

    //NewHaven's http://www.newhavendisplay.com/app_notes/2-4TFT_ST7789.txt
    //THIS ONE WORKED THE BEST
    
    write_command(ST7789_SWRESET);  //Software reset
    HAL_DelayUSec(125);

    write_command(ST7789_SLPOUT);  //Exit sleep
    HAL_DelayUSec(150);

    write_command(ST7789_MADCTL);  //Memory Data Access control
    write_data(0x08); //Bottom to top page address order

    write_command(ST7789_COLMOD); //Color mode
    write_data(0x55); //16-bit, 565 RGB
    //write_data(0x53); //12-bit, 444N RGB (N=col for next byte, uses 1.5B per pixel when mass sending data)

    /*
    write_command(ST7789_PORCTR); //Porch control
    write_data(0x0C); //
    write_data(0x0C); //
    write_data(0x00); //Separate porch control disabled
    write_data(0x33); //
    write_data(0x33); //

    write_command(ST7789_GCTRL);  //Gate control
    write_data(0x35); //+13.26V, -10.43V

    write_command(ST7789_VCOM); //VCOM setting
    write_data(0x2B); //1.175V

    write_command(ST7789_LCMCTR); //LCM control
    write_data(0x2C); //XOR RGB in MADCTL, XOR MV in MADCTL, reverse source output order XMH (?)

    write_command(ST7789_VDVVRHEN); //VDV & VRV command enable
    write_data(0x01); //Enable
    write_data(0xFF); //Hardcoded/ default

    write_command(ST7789_VRHS); //VRH set
    //write_data(0x11); //+4.4V + ( vcom+vcom offset+0.5vdv), -4.4V + ( vcom+vcom offset-0.5vdv)
    write_data(0x0B); //+4.1 + ( vcom+vcom offset+0.5vdv), -4.1 + ( vcom+vcom offset+0.5vdv)
    //May be an error confusing hex and decimal, 11dec = 0x0B = register default value

    write_command(ST7789_VDVS); //VDV set
    write_data(0x20); //0V
    //*/

    write_command(ST7789_FRMCTR2);  //Frame rate control in normal mode
    write_data(0x01); //111Hz
    //write_data(0x0F); //60Hz

    /*
    write_command(ST7789_PWCTRL1);  //Power control 1
    write_data(0xA4); //Hardcoded/ default
    write_data(0xA1); //Avdd = 6.8V, Avcl = -4.8V, Vdds = 2.3V
    //*/
    
    //nrf_delay_ms(2000);

    write_command(ST7789_PVGAMCTRL); //+ voltage gamma control
    write_data(0xD0);
    write_data(0x00);
    write_data(0x05);
    write_data(0x0E);
    write_data(0x15);
    write_data(0x0D);
    write_data(0x37);
    write_data(0x43);
    write_data(0x47);
    write_data(0x09);
    write_data(0x15);
    write_data(0x12);
    write_data(0x16);
    write_data(0x09);//*/

    //uint8_t dat_buf[14] = {0xD0, 0x00, 0x05, 0x0E, 0x15, 0x0D, 0x37, 0x43, 0x47, 0x09, 0x15, 0x12, 0x16, 0x09};
    //write_data_buffered(dat_buf, 14);

    write_command(ST7789_NVGAMCTRL); //- voltage gamma control
    write_data(0xD0);
    write_data(0x00);
    write_data(0x05);
    write_data(0x0D);
    write_data(0x0C);
    write_data(0x06);
    write_data(0x2D);
    write_data(0x44);
    write_data(0x40);
    write_data(0x0E);
    write_data(0x1C);
    write_data(0x18);
    write_data(0x16);
    write_data(0x19);//*/

    //uint8_t dat_buf2[14] = {0xD0, 0x00, 0x05, 0x0D, 0x0C, 0x06, 0x2D, 0x44, 0x40, 0x0E, 0x1C, 0x18, 0x16, 0x19};
    //write_data_buffered(dat_buf2, 14);

    /*write_command(ST7789_RGBCTRL);  //RGB interface control
    write_data(
    write_data(
    write_data(//*/

    /*
    write_command(ST7789_CASET);  //Column address set
    write_data(0x00); //x start high byte = 0
    write_data(0x00); //x start low byte = 0
    write_data(0x00); //x end high byte = 0
    write_data(0xEf); //x end low byte = 239

    write_command(ST7789_RASET);  //Row address set
    write_data(0x00); //x start high byte = 0
    write_data(0x50); //x start low byte = 80
    write_data(0x01); //x end high byte = 1
    write_data(0x3F); //x end low byte = 63
    nrf_delay_ms(10); //x end total = 319
    //*/
    
    write_command(ST7789_INVON);  //Screen inversion on, added in because colors were reversed without

    write_command(ST7789_DISPON);

    //*/

    // Nordic's (?) ST7789, modified from ILI9341
    /*
    write_command(ST7789_SWRESET);
    nrf_delay_ms(120);
    write_command(ST7789_DISPOFF);
    nrf_delay_ms(120);
    write_command(ST7789_PWCTRB);
    write_data(0x00);
    write_data(0XC1);
    write_data(0X30);

    write_command(ST7789_TIMCTRA);
    write_data(0x85);
    write_data(0x00);
    write_data(0x78);

    write_command(ST7789_PWCTRSEQ);
    write_data(0x39);
    write_data(0x2C);
    write_data(0x00);
    write_data(0x34);
    write_data(0x02);

    write_command(ST7789_PUMP);
    write_data(0x20);

    write_command(ST7789_TIMCTRB);
    write_data(0x00);
    write_data(0x00);

    //write_command(ST7789_PWCTR1);
    write_command(0xC0);
    write_data(0x23);

    write_command(ST7789_PWCTR2);
    write_data(0x10);

    write_command(ST7789_VMCTR1);
    write_data(0x3e);
    write_data(0x28);

    write_command(ST7789_VMCTR2);
    write_data(0x86);

    write_command(ST7789_MADCTL);
    write_data(0x48);

    write_command(ST7789_COLMOD);
    write_data(0x55);

    write_command(ST7789_FRMCTR1);
    write_data(0x00);
    write_data(0x18);

    write_command(ST7789_DFUNCTR);
    write_data(0x08);
    write_data(0x82);
    write_data(0x27);

    write_command(ST7789_ENGMCTR);
    write_data(0x00);

    write_command(ST7789_GAMMASET);
    write_data(0x01);

    write_command(ST7789_PVGAMCTRL);
    write_data(0x0F);
    write_data(0x31);
    write_data(0x2B);
    write_data(0x0C);
    write_data(0x0E);
    write_data(0x08);
    write_data(0x4E);
    write_data(0xF1);
    write_data(0x37);
    write_data(0x07);
    write_data(0x10);
    write_data(0x03);
    write_data(0x0E);
    write_data(0x09);
    write_data(0x00);

    write_command(ST7789_NVGAMCTRL);
    write_data(0x00);
    write_data(0x0E);
    write_data(0x14);
    write_data(0x03);
    write_data(0x11);
    write_data(0x07);
    write_data(0x31);
    write_data(0xC1);
    write_data(0x48);
    write_data(0x08);
    write_data(0x0F);
    write_data(0x0C);
    write_data(0x31);
    write_data(0x36);
    write_data(0x0F);

    write_command(ST7789_SLPOUT);
    nrf_delay_ms(120);
    write_command(ST7789_DISPON);
    //*/
}

void st7789_pixel_draw(uint16_t x, uint16_t y, uint32_t color)
{
    set_addr_window(x, y, x, y);

    uint8_t data[2] = {color >> 8, color};
    
    //nrf_gpio_pin_set(ST7789_DC_PIN);

    //spi_write(data, sizeof(data));

    write_data_buffered(data, sizeof(data));
}

void st7789_rect_draw(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color)
{
	printf("st7789_rect_draw\n");
    set_addr_window(x, y, x + width - 1, y + height - 1);

    uint8_t data[2] = {color >> 8, color};

    HAL_GPIO_Write(ST7789_DC_PIN,1);

    // Duff's device algorithm for optimizing loop.
    uint32_t i = (height * width + 7) / 8;

/*lint -save -e525 -e616 -e646 */
    switch ((height * width) % 8) {
        case 0:
            do {
                spi_write(data, sizeof(data));
        case 7:
                spi_write(data, sizeof(data));
        case 6:
                spi_write(data, sizeof(data));
        case 5:
                spi_write(data, sizeof(data));
        case 4:
                spi_write(data, sizeof(data));
        case 3:
                spi_write(data, sizeof(data));
        case 2:
                spi_write(data, sizeof(data));
        case 1:
                spi_write(data, sizeof(data));
            } while (--i > 0);
        default:
            break;
    }
    HAL_GPIO_Write(ST7789_DC_PIN,0);
}

/*void st7789_flush(const lv_area_t * area, lv_color_t * color_p)
{
    set_addr_window(area->x1, area->y1, area->x2, area->y2);

    uint8_t data[2] = {0};

    nrf_gpio_pin_set(ST7789_DC_PIN);

    uint16_t width = 0;
    uint16_t height = 0;

    if( area->x2 >= area->x1 )
      width =(area->x2 - area->x1) + 1;
    else
      width = (area->x1 - area->x2) + 1;

    if( area->y2 >= area->y1 )
      height = (area->y2 - area->y1) + 1;
    else
      height = (area->y1 - area->y2) + 1;

    // Duff's device algorithm for optimizing loop.
    uint32_t i = (height * width + 7) / 8;

//lint -save -e525 -e616 -e646 
    switch ((height * width) % 8) {
        case 0:
            do {
                data[0] = color_p->full >> 8;
                data[1] = (uint8_t)color_p->full;
                spi_write(data, sizeof(data));
                color_p++;
        case 7:
                data[0] = color_p->full >> 8;
                data[1] = (uint8_t)color_p->full;
                spi_write(data, sizeof(data));
                color_p++;
        case 6:
                data[0] = color_p->full >> 8;
                data[1] = (uint8_t)color_p->full;
                spi_write(data, sizeof(data));
                color_p++;
        case 5:
                data[0] = color_p->full >> 8;
                data[1] = (uint8_t)color_p->full;
                spi_write(data, sizeof(data));
                color_p++;
        case 4:
                data[0] = color_p->full >> 8;
                data[1] = (uint8_t)color_p->full;
                spi_write(data, sizeof(data));
                color_p++;
        case 3:
                data[0] = color_p->full >> 8;
                data[1] = (uint8_t)color_p->full;
                spi_write(data, sizeof(data));
                color_p++;
        case 2:
                data[0] = color_p->full >> 8;
                data[1] = (uint8_t)color_p->full;
                spi_write(data, sizeof(data));
                color_p++;
        case 1:
                data[0] = color_p->full >> 8;
                data[1] = (uint8_t)color_p->full;
                spi_write(data, sizeof(data));
                color_p++;
            } while (--i > 0);
        default:
            break;
    }
    HAL_GPIO_Write(ST7789_DC_PIN, 0);
}*/


void st7789_dummy_display(uint8_t * data, uint16_t len, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    if(len == 0xFFFF){
        set_addr_window(x0, y0, x1, y1);
    }
    else{
      //spi_write(&data, len);
      write_data_buffered(data, len);
    }
}

void st7789_rotation_set(nrf_lcd_rotation_t rotation)
{
    write_command(ST7789_MADCTL);
    switch (rotation % 4) {
        case NRF_LCD_ROTATE_0:
            write_data(ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB);  //Not working correctly
            //Column address (MX): Right to left
            //Page address (MY): Bottom to top
            //Page/ Column order (MV): normal
            //RGB/BGR order: RGB
            x_offset = 0;
            y_offset = ST7789_240x240_YSTART;
            break;
        case NRF_LCD_ROTATE_90:
            write_data(ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
            //Column address (MX): Left to right
            //Page address (MY): Top to bottom
            //Page/ Column order (MV): reverse
            //RGB/BGR order: RGB
            x_offset = 0;
            y_offset = 0;
            break;
        case NRF_LCD_ROTATE_180:
            write_data(ST7789_MADCTL_RGB);
            //Column address (MX): Left to right
            //Page address (MY): Top to bottom
            //Page/ Column order (MV): normal
            //RGB/BGR order: RGB
            x_offset = 0;
            y_offset = 0;
            break;
        case NRF_LCD_ROTATE_270:
            write_data(ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
            //Column address (MX): Right to left
            //Page address (MY): Top to bottom
            //Page/ Column order (MV): reverse
            //RGB/BGR order: RGB
            x_offset = 0;
            y_offset = 0;
            break;
        default:
            break;
    }
}

void st7789_display_invert(uint8_t invert)
{
    write_command(invert ? ST7789_INVON : ST7789_INVOFF);
}

void st7789_screen_clear(uint32_t color)
{
	printf("st7789_screen_clear\n");
    st7789_rect_draw(0, 0, 240, 240, color);
}


