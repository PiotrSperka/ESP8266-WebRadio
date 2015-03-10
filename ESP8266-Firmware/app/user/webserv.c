#include "user/webserv.h"
#include "osapi.h"
#include "c_stdio.h"
#include "c_string.h"
#include "c_stdlib.h"
#include "espconn.h"

#include "user/serv-fs.h"

static struct espconn servConn;
static esp_tcp servTcp;

struct servFile* findFile(char* name)
{
	struct servFile* f = (struct servFile*)&indexFile;
	while(1)
	{
		if(strcmp(f->name, name) == 0) return f;
		else f = f->next;
		if(f == NULL) return NULL;
	}
}

void serveFile(char* name)
{
	int length;
	char buf[96];
	const char *content;
	
	struct servFile* f = findFile(name);
	
	if(f != NULL)
	{
		length = f->size;
		content = f->content;
	}
	else length = 0;

	c_sprintf(buf, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n", (f!=NULL ? f->type : "text/plain"), length);
	espconn_sent(&servConn,buf,strlen(buf)); // SEND HEADER
	if(length > 0)
	{
		espconn_sent(&servConn,(uint8_t*)content,length); // SEND CONTENT
	}
}

void serverReceiveCallback(void *arg, char *pdata, unsigned short len)
{
	// DEBUG
	//int i;
	//for(i=0; i<len; i++) uart0_putc(pdata[i]);
	//
	char *c;
	if( (c = strstr(pdata, "GET ")) != NULL)
	{
		char fname[32];
		uint8_t i;
		for(i=0; i<32; i++) fname[i] = 0;
		c += 4;
		char* c_end = strstr(c, " ");
		if(c_end == NULL) return;
		uint8_t len = c_end-c;
		if(len > 32) return;
		strncpy(fname, c, len);
		serveFile(fname);
	}
}

void serverSentCallback(void *arg)
{
	
}

void serverReconnectCallback(void *arg, int8_t err)
{
	
}

void serverDisconnectCallback(void *arg)
{
	
}

void serverConnectCallback(void *arg)
{
	//char *transmission = "HTTP/1.0 200 OK\r\n\r\n";
  	//sint8 d = espconn_sent(&servConn,transmission,strlen(transmission));
}

void serverInit()
{
	servConn.type = ESPCONN_TCP;
	servConn.state = ESPCONN_NONE;
	servTcp.local_port = 80;
	servConn.proto.tcp = &servTcp;
	
	espconn_regist_connectcb(&servConn, serverConnectCallback);
	
	espconn_regist_disconcb(&servConn, serverDisconnectCallback);
	espconn_regist_recvcb(&servConn, serverReceiveCallback);
	espconn_regist_sentcb(&servConn, serverSentCallback);
	espconn_regist_reconcb(&servConn, serverReconnectCallback);
	
	espconn_accept(&servConn);
	uart0_sendStr("\n##SRV.STARTED#\n");
}

void serverDisconnect()
{
	espconn_disconnect(&servConn);
	uart0_sendStr("\n##SRV.STOPPED#\n");
}