#include "user/interface.h"
#include "user_interface.h"
#include "osapi.h"
#include "c_stdio.h"
#include "driver/uart.h"
#include "c_string.h"
#include "c_stdlib.h"

#include "user/webserv.h"

#define MAX_WIFI_STATIONS 50

uint8_t startsWith(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}

void printInfo(char* s)
{
	int i;
	//char buf[12];
	//c_sprintf(buf, " %d", size);
	uart0_sendStr("\n#INFO#");
	uart0_sendStr(s);
	//uart0_sendStr(buf);
	//for(i=0; i<size; i++) uart0_putc(*s++);
	uart0_sendStr("\n##INFO#");
}

/*
* LIST OF REQUIRED COMMANDS:
* -OK- List available wifi networks
* -OK- Connect to choosen wifi network (id, passwd)
* -OK- Disconnect from wifi (?)
* -OK- Get info about connected network
* - Set settings (to admin panel)
* - Get settings (from admin panel)
* - Connect to shoutcast (address, path)
* - Disconnect from shoutcast
*/

void wifiScanCallback(void *arg, STATUS status)
{
	if(status == OK)
	{
		int i = MAX_WIFI_STATIONS;
		char buf[64];
		struct bss_info *bss_link = (struct bss_info *) arg;
		uart0_sendStr("\n#WIFI.LIST#");
		while(i > 0)
		{
			i--;
			bss_link = bss_link->next.stqe_next;
			if(bss_link == NULL) break;
			c_sprintf(buf, "\n%s;%d;%d;%d", bss_link->ssid, bss_link->channel, bss_link->rssi, bss_link->authmode);
			uart0_sendStr(buf);
		}
		uart0_sendStr("\n##WIFI.LIST#");
	}
}

void wifiScan()
{
	wifi_station_scan(NULL, wifiScanCallback);
}

void wifiConnect(char* cmd)
{
	int i;
	struct station_config cfg;
	for(i = 0; i < 32; i++) cfg.ssid[i] = 0;
	for(i = 0; i < 64; i++) cfg.password[i] = 0;
	cfg.bssid_set = 0;
	
	wifi_station_disconnect();
	
	char *t = strstr(cmd, "(\"");
	if(t == 0)
	{
		uart0_sendStr("\n##WIFI.CMD_ERROR#");
		return;
	}
	char *t_end  = strstr(t, "\",\"");
	if(t_end == 0)
	{
		uart0_sendStr("\n##WIFI.CMD_ERROR#");
		return;
	}
	
	strncpy( cfg.ssid, (t+2), (t_end-t-2) );
	
	t = t_end+3;
	t_end = strstr(t, "\")");
	if(t_end == 0)
	{
		uart0_sendStr("\n##WIFI.CMD_ERROR#");
		return;
	}
	
	strncpy( cfg.password, t, (t_end-t)) ;

	if( wifi_station_connect() ) uart0_sendStr("\n##WIFI.CONNECTED#");
	else uart0_sendStr("\n##WIFI.NOT_CONNECTED#");
}

void wifiDisconnect()
{
	if(wifi_station_disconnect()) uart0_sendStr("\n##WIFI.NOT_CONNECTED#");
	else uart0_sendStr("\n##WIFI.DISCONNECT_FAILED#");
}

void wifiStatus()
{
	struct ip_info ipi;
	char buf[32+50];
	uint8_t t = wifi_station_get_connect_status();	
	wifi_get_ip_info(0, &ipi);
	c_sprintf(buf, "#WIFI.STATUS#\n%d\n%d.%d.%d.%d\n%d.%d.%d.%d\n%d.%d.%d.%d\n##WIFI.STATUS#\n",
			  t, (ipi.ip.addr&0xff), ((ipi.ip.addr>>8)&0xff), ((ipi.ip.addr>>16)&0xff), ((ipi.ip.addr>>24)&0xff),
			 (ipi.netmask.addr&0xff), ((ipi.netmask.addr>>8)&0xff), ((ipi.netmask.addr>>16)&0xff), ((ipi.netmask.addr>>24)&0xff),
			 (ipi.gw.addr&0xff), ((ipi.gw.addr>>8)&0xff), ((ipi.gw.addr>>16)&0xff), ((ipi.gw.addr>>24)&0xff));
	uart0_sendStr(buf);
}

void wifiGetStation()
{
	char buf[131];
	struct station_config cfg;
	wifi_station_get_config(&cfg);
	c_sprintf(buf, "\n#WIFI.STATION#\n%s\n%s\n##WIFI.STATION#\n", cfg.ssid, cfg.password);
	uart0_sendStr(buf);
}

void checkCommand(int size, char* s)
{
	char *tmp = (char*)c_malloc((size+1)*sizeof(char));
	int i;
	for(i=0;i<size;i++) tmp[i] = s[i];
	tmp[size] = 0;
	if(strcmp(tmp, "wifi.list") == 0) wifiScan(); //printInfo("WIFI LIST");
	else if(startsWith("wifi.con", tmp)) wifiConnect(tmp);
	else if(strcmp(tmp, "wifi.discon") == 0) wifiDisconnect();
	else if(strcmp(tmp, "wifi.status") == 0) wifiStatus();
	else if(strcmp(tmp, "wifi.station") == 0) wifiGetStation();
	else if(strcmp(tmp, "srv.start") == 0) serverInit();
	else if(strcmp(tmp, "srv.stop") == 0) serverDisconnect();
	else printInfo(tmp);
	c_free(tmp);
}

void printConfig()
{
	
}
