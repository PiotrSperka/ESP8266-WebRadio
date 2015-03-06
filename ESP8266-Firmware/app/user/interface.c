#include "user/interface.h"
#include "osapi.h"
#include "c_stdio.h"
#include "driver/uart.h"
#include "c_string.h"
#include "c_stdlib.h"

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
* - List available wifi networks
* - Connect to choosen wifi network (id, passwd)
* - Disconnect from wifi (?)
* - Set settings (to admin panel)
* - Get settings (from admin panel)
* - Connect to shoutcast (address, path)
* - Disconnect from shoutcast
*/

void checkCommand(int size, char* s)
{
	char *tmp = (char*)c_malloc((size+1)*sizeof(char));
	int i;
	for(i=0;i<size;i++) tmp[i] = s[i];
	tmp[size] = 0;
	if(strcmp(tmp, "wifi.list") == 0) printInfo("WIFI LIST");
	else if(strcmp(tmp, "wifi.con") == 0) printInfo("WIFI CONNECT");
	else if(strcmp(tmp, "wifi.discon") == 0) printInfo("WIFI DISCONNECT");
	else printInfo("UNKNOWN COMMAND");
	c_free(tmp);
}

void printConfig()
{
	
}
