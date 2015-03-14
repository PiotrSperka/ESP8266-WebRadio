#include "user/webserv.h"
#include "osapi.h"
#include "c_stdio.h"
#include "c_string.h"
#include "c_stdlib.h"
#include "espconn.h"

#include "user/serv-fs.h"

static struct espconn servConn;
static esp_tcp servTcp;

#define INTERNAL_FLASH_START_ADDRESS    0x40200000

uint32_t fread( void *to, uint32_t fromaddr, uint32_t size )
{
  fromaddr -= INTERNAL_FLASH_START_ADDRESS;
  int r;
  WRITE_PERI_REG(0x60000914, 0x73);
  r = spi_flash_read(fromaddr, (uint32 *)to, size);
  if(0 == r)
    return size;
  else{
    return 0;
  }
}


ICACHE_FLASH_ATTR struct servFile* findFile(char* name)
{
	struct servFile* f = (struct servFile*)&indexFile;
	while(1)
	{
		if(strcmp(f->name, name) == 0) return f;
		else f = f->next;
		if(f == NULL) return NULL;
	}
}

ICACHE_FLASH_ATTR void serveFile(char* name)
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
		char *con = (char*)c_malloc(length*sizeof(char));
		if(con != NULL)
		{
			fread(con, (uint32_t)content, length);
			espconn_sent(&servConn, (uint8_t*)con, length); // SEND CONTENT
			/*uint32_t tsnd = 0;
			while(1)
			{
				int tlen = 4096, temp=0;
				uint32_t taddr = (uint32_t)content + tsnd;
				if(length < 4096) tlen = length;
				fread(con, taddr, tlen);
				espconn_sent(&servConn,(uint8_t*)con,tlen); // SEND CONTENT
				length -= tlen;
				tsnd += tlen;
				if(length == 0) break;
			}*/
			c_free(con);
		}
	}
}

ICACHE_FLASH_ATTR void serverReceiveCallback(void *arg, char *pdata, unsigned short len)
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

ICACHE_FLASH_ATTR void serverSentCallback(void *arg)
{
}

ICACHE_FLASH_ATTR void serverReconnectCallback(void *arg, int8_t err)
{
}

ICACHE_FLASH_ATTR void serverDisconnectCallback(void *arg)
{
}

ICACHE_FLASH_ATTR void serverConnectCallback(void *arg)
{
}

ICACHE_FLASH_ATTR void serverInit()
{
	servConn.type = ESPCONN_TCP;
	servConn.state = ESPCONN_NONE;
	servTcp.local_port = 80;
	servConn.proto.tcp = &servTcp;
	
	espconn_disconnect(&servConn);
	
	espconn_regist_connectcb(&servConn, serverConnectCallback);
	
	espconn_regist_disconcb(&servConn, serverDisconnectCallback);
	espconn_regist_recvcb(&servConn, serverReceiveCallback);
	espconn_regist_sentcb(&servConn, serverSentCallback);
	espconn_regist_reconcb(&servConn, serverReconnectCallback);
	
	espconn_accept(&servConn);
	uart0_sendStr("\n##SRV.STARTED#\n");
}

ICACHE_FLASH_ATTR void serverDisconnect()
{
	espconn_disconnect(&servConn);
	uart0_sendStr("\n##SRV.STOPPED#\n");
}
