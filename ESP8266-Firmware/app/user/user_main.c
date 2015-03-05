/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_main.c
 *
 * Description: entry file of user application
 *
 * Modification history:
 *     2014/1/1, v1.0 create this file.
 *     2015/3/4, v1.01 PS tests and some modifications
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
/*
void task_lua(os_event_t *e){
    char* lua_argv[] = { (char *)"lua", (char *)"-i", NULL };
    NODE_DBG("Task task_lua started.\n");
    switch(e->sig){
        case SIG_LUA:
            NODE_DBG("SIG_LUA received.\n");
            lua_main( 2, lua_argv );
            break;
        default:
            break;
    }
}
*/
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
    os_timer_disarm(&testTmr);
    os_timer_setfn(&testTmr, &onTestTmr, (void*)0);
    os_timer_arm(&testTmr, 500, 1);
    //taskQueue = (os_event_t *)os_malloc(sizeof(os_event_t) * TASK_QUEUE_LEN);
    //system_os_task(task_lua, USER_TASK_PRIO_0, taskQueue, TASK_QUEUE_LEN);
}

//extern void spiffs_mount();
/*
void nodemcu_init(void)
{
    NODE_ERR("\n");
    // Initialize platform first for lua modules.   
    if( platform_init() != PLATFORM_OK )
    {
        // This should never happen
        NODE_DBG("Can not init platform for modules.\n");
        return;
    }
    
    if( !flash_init_data_written() ){
        NODE_ERR("Restore init data.\n");
        // Flash init data at FLASHSIZE - 0x04000 Byte.
        flash_init_data_default();
        // Flash blank data at FLASHSIZE - 0x02000 Byte.
        flash_init_data_blank(); 
    }

#if defined( BUILD_WOFS )
    romfs_init();

#elif defined ( BUILD_SPIFFS )
    spiffs_mount();
    // test_spiffs();
#endif

    task_init();
    system_os_post(USER_TASK_PRIO_0,SIG_LUA,'s');
}
*/
/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
    // NODE_DBG("SDK version:%s\n", system_get_sdk_version());
    // system_print_meminfo();
    // os_printf("Heap size::%d.\n",system_get_free_heap_size());
    // os_delay_us(50*1000);   // delay 50ms before init uart

#ifdef DEVELOP_VERSION
    uart_init(BIT_RATE_74880, BIT_RATE_74880);
#else
    uart_init(BIT_RATE_460800, BIT_RATE_460800);
    //uart0_sendStr("\r\nModded by Piotr Sperka\r\nhttp:\\\\piotrsperka.info\r\n\r\n");
#endif
    // uart_init(BIT_RATE_115200, BIT_RATE_115200);
    
    #ifndef NODE_DEBUG
    system_set_os_print(0);
    #endif

    uart0_sendStr("\n\n\nModded by Piotr Sperka\nhttp:\\\\piotrsperka.info");
    
    system_init_done_cb(task_init);
    //system_init_done_cb(nodemcu_init);
}
