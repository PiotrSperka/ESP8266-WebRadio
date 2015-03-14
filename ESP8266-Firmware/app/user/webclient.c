#include "user/webclient.h"
#include "string.h"
#include "c_stdio.h"
#include "c_stdlib.h"
#include "espconn.h"
#include "osapi.h"

struct icyHeader header = {NULL, NULL, NULL, NULL, 0};
char *clientURL = NULL;
char *clientPath = NULL;
uint16_t clientPort = 80;
ip_addr_t clientIP;

static struct espconn clientConn;
static esp_tcp clientTcp;

struct icyHeader* clientGetHeader()
{
	return &header;
}

void clientSetURL(char* url)
{
	int l = strlen(url)+1;
	if(clientURL != NULL) c_free(clientURL);
	clientURL = (char*) c_malloc(l*sizeof(char));
	if(clientURL != NULL) strcpy(clientURL, url);
	uart0_sendStr("\n##CLI.URLSET#\n");
}

void clientSetPath(char* path)
{
	int l = strlen(path)+1;
	if(clientPath != NULL) c_free(clientPath);
	clientPath = (char*) c_malloc(l*sizeof(char));
	if(clientPath != NULL) strcpy(clientPath, path);
	uart0_sendStr("\n##CLI.PATHSET#\n");
}

void clientSetPort(uint16_t port)
{
	clientPort = port;
	uart0_sendStr("\n##CLI.PORTSET#\n");
}

ICACHE_FLASH_ATTR void clientReceiveCallback(void *arg, char *pdata, unsigned short len)
{
	char buf[128];
	c_sprintf(buf, "TCP data received: %d bytes\n", len);
	uart0_sendStr(buf);
}

ICACHE_FLASH_ATTR void clientSentCallback(void *arg)
{
}

ICACHE_FLASH_ATTR void clientReconnectCallback(void *arg, int8_t err)
{
}

ICACHE_FLASH_ATTR void clientDisconnectCallback(void *arg)
{
}

ICACHE_FLASH_ATTR void clientConnectCallback(void *arg)
{
	uint16_t len = strlen(clientPath)+18;
	uint16_t i;
	char *data = (char*) c_malloc(len*sizeof(char));
	if(data != NULL)
	{
		for(i=0; i<len; i++) data[i] = 0;
		c_sprintf(data, "GET %s HTTP/1.0\r\n\r\n", clientPath);
		espconn_sent(&clientConn, (uint8_t*)data, len-1);
	}
}

ICACHE_FLASH_ATTR void clientIpFoundCallback(const char *name, ip_addr_t *ipaddr, void *arg)
{
	if(ipaddr == NULL)
	{
		uart0_sendStr(name); // DEBUG
		uart0_sendStr("\n##CLI.NOTFOUND#\n");
	}
	else
	{
		espconn_regist_connectcb(&clientConn, clientConnectCallback);
		espconn_regist_disconcb(&clientConn, clientDisconnectCallback);
		espconn_regist_recvcb(&clientConn, clientReceiveCallback);
		espconn_regist_sentcb(&clientConn, clientSentCallback);
		espconn_regist_reconcb(&clientConn, clientReconnectCallback);
		*((uint32_t*)clientTcp.remote_ip) = ipaddr->addr;
		/*
		char buf[64]; // DEBUG
		c_sprintf(buf, "\nDNS found: %d.%d.%d.%d\n", clientTcp.remote_ip[0], clientTcp.remote_ip[1], clientTcp.remote_ip[2], clientTcp.remote_ip[3]);
		uart0_sendStr(buf);
		*/
		espconn_connect(&clientConn);
		uart0_sendStr("\n##CLI.STARTED#\n");
	}
}

void clientConnect()
{
	clientConn.type = ESPCONN_TCP;
	clientConn.state = ESPCONN_NONE;
	clientTcp.remote_port = clientPort;
	clientTcp.local_port = espconn_port();
	
	clientConn.proto.tcp = &clientTcp;
	
	espconn_disconnect(&clientConn);
	
	espconn_gethostbyname(&clientConn, clientURL, &clientIP, clientIpFoundCallback);
	uart0_sendStr("\n##CLI.DNSLOOKUP#\n");
}

void clientDisconnect()
{
	espconn_disconnect(&clientConn);
	uart0_sendStr("\n##CLI.STOPPED#\n");
}
