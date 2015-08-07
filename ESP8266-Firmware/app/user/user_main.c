/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_main.c
 *
 * Description: entry file of user application
 *
 * Modification history:
 *     2014/12/1, v1.0 create this file.
*******************************************************************************/
#include "esp_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "el_uart.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "interface.h"
#include "webserver.h"
#include "webclient.h"

#include "vs1053.h"


void uartInterfaceTask(void *pvParameters) {
	char tmp[64];
	int t = 0;
	for(t = 0; t<64; t++) tmp[t] = 0;
	t = 0;
	uart_rx_init();
	printf("UART READY TO READ\n");
	while(1) {
		while(1) {
			char c = uart_getchar();
			if(c == '\r') break;
			if(c == '\n') break;
			tmp[t] = c;
			t++;
		}
		checkCommand(t, tmp);
		for(t = 0; t<64; t++) tmp[t] = 0;
		t = 0;
		vTaskDelay(100);
	}
}

UART_SetBaudrate(uint8 uart_no, uint32 baud_rate) {
	uart_div_modify(uart_no, UART_CLK_FREQ / baud_rate);
}

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
	UART_SetBaudrate(0,460800);
	wifi_set_opmode(STATION_MODE);
    printf("SDK version:%s\n", system_get_sdk_version());
	
	//DEBUG
	struct station_config *config = (struct station_config *)zalloc(sizeof(struct station_config));
	sprintf(config->ssid, "linksys@Pogodna8");
	sprintf(config->password, "Pogodna8");
	wifi_station_set_config(config);
    free(config);
	//DEBUG
	
	VS1053_HW_init();
	VS1053_Start();
	VS1053_SetVolume(50);
	//VS1053_SineTest();
	VS1053_regtest();
	VS1053_SPI_SpeedUp();
	VS1053_regtest();

	xTaskCreate(uartInterfaceTask, "t1", 256, NULL, 2, NULL);
	xTaskCreate(serverTask, "t2", 256, NULL, 2, NULL);
	xTaskCreate(clientTask, "t3", 256, NULL, 2, NULL);
	xTaskCreate(vsTask, "t4", 768, NULL, 2, NULL);
}

