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

/* Include the generic headers required for QORC */
#include "eoss3_hal_gpio.h"
#include "eoss3_hal_rtc.h"
//#include "eoss3_hal_fpga_usbserial.h"
#include "ql_time.h"
#include "s3x_clock_hal.h"
#include "s3x_clock.h"
#include "s3x_pi.h"
#include "dbg_uart.h"
#include "eoss3_hal_timer.h"
#include "cli.h"
#include <eoss3_hal_pad_config.h>

extern const struct cli_cmd_entry my_main_menu[];
const char *SOFTWARE_VERSION_STR;

/*
 * Global variable definition
 */

void red_led_blink(void)
{
	uint8_t led_blink = 0;
	for(led_blink = 10;led_blink >=0; led_blink--)
	{
		HAL_GPIO_Write(GPIO_6 ,1);
		HAL_DelayUSec(500);
		HAL_GPIO_Write(GPIO_6 ,0);
	}
}

extern void qf_hardwareSetup();
static void nvic_init(void);

int main(void)
{
	PadConfig Pad_test;
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
    dbg_str( "##########################\n\n");
    dbg_str( "\n\nHello world!!\n\n");	// <<<<<<<<<<<<<<<<<<<<<  Change me!

    CLI_start_task( my_main_menu );

    HAL_Delay_Init();

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

	red_led_blink();

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


