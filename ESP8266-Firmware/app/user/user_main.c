/******************************************************************************
 * Copyright 2015 Piotr Sperka (http://www.piotrsperka.info)
 *
 * FileName: user_main.c
 *
 * Description: entry file of user application
*******************************************************************************/
#include "esp_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "el_uart.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "interface.h"
#include "webserver.h"
#include "webclient.h"

#include "vs1053.h"

#include "eeprom.h"

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
			if(t == 64) t = 0;
		}
		checkCommand(t, tmp);
		for(t = 0; t<64; t++) tmp[t] = 0;
		t = 0;
		vTaskDelay(25); // 250ms
	}
}

UART_SetBaudrate(uint8 uart_no, uint32 baud_rate) {
	uart_div_modify(uart_no, UART_CLK_FREQ / baud_rate);
}

void testtask(void* p) {
	gpio16_output_conf();
	while(1) {
		gpio16_output_set(0);
		vTaskDelay(50);
		gpio16_output_set(1);
		vTaskDelay(50);
	};
}

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
	UART_SetBaudrate(0,115200);
	wifi_set_opmode(STATION_MODE);
	
	clientInit();
	
	//DEBUG
	/*struct station_config *config = (struct station_config *)malloc(sizeof(struct station_config));
	if(!config) printf("Unable to create config!\n");
	else {
		wifi_station_disconnect();
		sprintf(config->ssid, "linksys@Pogodna8");
		sprintf(config->password, "Pogodna8");
		wifi_station_set_config(config);
		wifi_station_connect();
		free(config);
	}*/
	//DEBUG
	
	VS1053_HW_init();
	VS1053_Start();
	VS1053_SetVolume(70);

	VS1053_SPI_SpeedUp();
	
	xTaskCreate(testtask, "t0", 256, NULL, 1, NULL); // DEBUG/TEST

	xTaskCreate(uartInterfaceTask, "t1", 256, NULL, 1, NULL);
	xTaskCreate(serverTask, "t2", 256, NULL, 1, NULL);
	xTaskCreate(clientTask, "t3", 512, NULL, 2, NULL);
	xTaskCreate(vsTask, "t4", 512, NULL, 2, NULL);
}

