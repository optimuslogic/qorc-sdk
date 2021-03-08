/*==========================================================
 * Copyright 2020 QuickLogic Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *==========================================================*/

/*==========================================================
 *
 *    File   : main.c
 *    Purpose: main for QuickFeather helloworldsw and LED/UserButton test
 *                                                          
 *=========================================================*/

#include "Fw_global_config.h"   // This defines application specific charactersitics

#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "RtosTask.h"
//#include "gpio_api.h"

/*    Include the generic headers required for QORC */
#include "eoss3_hal_gpio.h"
#include "eoss3_hal_rtc.h"
//#include "eoss3_hal_fpga_usbserial.h"
#include "ql_time.h"
#include "s3x_clock_hal.h"
#include "s3x_clock.h"
#include "s3x_pi.h"
#include "dbg_uart.h"
#include "cli.h"
#include <eoss3_hal_timer.h>
#include <eoss3_hal_pad_config.h>
#include <eoss3_hal_i2c.h>

extern const struct cli_cmd_entry my_main_menu[];
const char *SOFTWARE_VERSION_STR;

TaskHandle_t smartsensor_task_handle = NULL;

/*
 * Global variable definition
 */
uint8_t i2c_test_data;

void test_I2C0_scan(void)
{
	int i = 0;
	static int k = 0;
	static int j = 0;
	static int addr1[10] =  {0};
	int errcode = 0;
	volatile int count = 0;

	for (i=100;i<0x7F;i++)
	{
		i2c_test_data = 0xFF;
		errcode = HAL_I2C_Read(i, 0x00, &i2c_test_data, 1);
		if (errcode == HAL_OK)
		{
			++count;
			addr1[j] = i;
			j++;
			dbg_str_hex8("Device Detected at:",i);
			printf("i=0x%x, CHIPID = 0x%02X\r\n count = %d\n",i,i2c_test_data,count);
		}
		if (i%2)
			HAL_GPIO_Write(GPIO_6 ,1);
		else
			HAL_GPIO_Write(GPIO_6 ,0);
		HAL_DelayUSec(1000000);

		//vTaskDelay(pdMS_TO_TICKS(1000));
	}
}


void setup_I2C0_M4()
{
    I2C_Config xI2CConfig;

    // I2C config for I2C0
    xI2CConfig.eI2CFreq = I2C_100KHZ;
    xI2CConfig.eI2CInt = I2C_DISABLE;
    xI2CConfig.ucI2Cn = 0;
    HAL_I2C_Init(xI2CConfig);
}

void smartsensor_task_handler()
{
    // initialize the sensors here.
    HAL_Delay_Init();
    setup_I2C0_M4();
    HAL_I2C0_Select();

    uint8_t errcode = HAL_I2C_Read(0x68, 0x00, &i2c_test_data, 1);
    dbg_str_hex8("test:",i2c_test_data);

    test_I2C0_scan();
    vTaskDelete( NULL );
}

void init_smartsensor_task(void)
{
    //smartsensor_task_msgq_handle = xQueueCreate(20, sizeof(struct display_task_msg));
    //vQueueAddToRegistry( BLE_MsgQ, "Ble_Q" );

	xTaskCreate(smartsensor_task_handler,
			"displayT",
			256,NULL,
			PRIORITY_LOWER,
			&smartsensor_task_handle);

	//QL_ASSERT(dishplay_task_handle);
}

extern void qf_hardwareSetup();
static void nvic_init(void);

int main(void)
{
	PadConfig Pad_test;
	I2C_Config I2c_test;
	uint8_t error_code;
    SOFTWARE_VERSION_STR = "qorc-sdk/qf_apps/qf_helloworldm4";
    
    qf_hardwareSetup();
    nvic_init();

    dbg_str("\n\n");
    dbg_str( "##########################\n");
    dbg_str( "Quicklogic QuickFeather M4 standalone\n");
    dbg_str( "SW Version: ");
    dbg_str( SOFTWARE_VERSION_STR );
    dbg_str( "\n" );
    dbg_str( __DATE__ " " __TIME__ "\n" );
    dbg_str( "##########################\n");
    dbg_str( "Hello world!!\n");	// <<<<<<<<<<<<<<<<<<<<<  Change me!
    dbg_str( "I2C scan.. \n");

    CLI_start_task( my_main_menu );

    // I2C configure
//	I2c_test.eI2CFreq = I2C_100KHZ;
//	I2c_test.eI2CInt = I2C_DISABLE;
//	I2c_test.ucI2Cn = 0;
//	error_code = HAL_I2C_Init(I2c_test);

//	HAL_I2C0_Select();
//	test_I2C0_scan();

//    HAL_Delay_Init();


    //Pad Configure for red led
	Pad_test.ucPin = PAD_22;
	Pad_test.ucFunc = PAD22_FUNC_SEL_GPIO_6;
	Pad_test.ucCtrl = PAD_CTRL_SRC_A0;
	Pad_test.ucMode = PAD_MODE_OUTPUT_EN;
	Pad_test.ucPull = PAD_NOPULL;
	Pad_test.ucDrv = PAD_DRV_STRENGTH_4MA;
	Pad_test.ucSpeed = PAD_SLEW_RATE_SLOW;
	Pad_test.ucSmtTrg = PAD_SMT_TRIG_DIS;
	HAL_PAD_Config(&Pad_test);

    init_smartsensor_task();

	/* Start the tasks and timer running. */
    vTaskStartScheduler();
    dbg_str("\n");

    while(1);
}

static void nvic_init(void)
 {
    // To initialize system, this interrupt should be triggered at main.
    // So, we will set its priority just before calling vTaskStartScheduler(), not the time of enabling each irq.
    NVIC_SetPriority(Ffe0_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(SpiMs_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(CfgDma_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(Uart_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
    NVIC_SetPriority(FbMsg_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
 }    

//needed for startup_EOSS3b.s asm file
void SystemInit(void)
{

}


