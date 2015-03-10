/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_main.c
 *
 * Description: entry file of user application
 *
 * Modification history:
 *     2014/1/1,  v1.00 create this file.
 *     2015/03/06 v1.01 Piotr Sperka - tests and some modifications
*******************************************************************************/
//#include "lua.h"
#include "platform.h"
#include "c_string.h"
#include "c_stdlib.h"
#include "c_stdio.h"

#include "osapi.h"

//#include "romfs.h"
 
#include "user_interface.h"

#include "ets_sys.h"
#include "driver/uart.h"
#include "mem.h"

#define SIG_LUA 0
#define TASK_QUEUE_LEN 4
os_event_t *taskQueue;
ETSTimer testTmr;

void onTestTmr(void)
{
    char buf[8];
    static int i = 0;
    i++;
    c_sprintf(buf, "%d", i);
    uart0_sendStr("\nTest timera - wywolanie ");
    uart0_sendStr(buf);
}

void task_init(void){
    //os_timer_disarm(&testTmr);
    //os_timer_setfn(&testTmr, &onTestTmr, (void*)0);
    //os_timer_arm(&testTmr, 2000, 1);
    //taskQueue = (os_event_t *)os_malloc(sizeof(os_event_t) * TASK_QUEUE_LEN);
    //system_os_task(task_lua, USER_TASK_PRIO_0, taskQueue, TASK_QUEUE_LEN);
}

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
    uart_init(BIT_RATE_460800, BIT_RATE_460800);

    uart0_sendStr("\n\nFirmware v0.1 by Piotr Sperka\nhttp:\\\\piotrsperka.info\n\n");
    
    wifi_set_opmode(0x01); // SET WIFI TO STATION MODE
    
    system_init_done_cb(task_init);
}
