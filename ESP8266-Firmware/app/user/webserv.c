#include "user/webserv.h"
#include "user/webclient.h"
#include "osapi.h"
#include "c_stdio.h"
#include "c_string.h"
#include "c_stdlib.h"
#include "espconn.h"

#include "user/serv-fs.h"

static struct espconn servConn;
static esp_tcp servTcp;

#define INTERNAL_FLASH_START_ADDRESS    0x40200000

ICACHE_FLASH_ATTR uint32_t fread( void *to, uint32_t fromaddr, uint32_t size )
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

ICACHE_FLASH_ATTR char* my_strdup(char* string, int length)
{
  char* newstr = (char*)c_malloc((length+1)*sizeof(char));
  if(newstr != NULL)
  {
    int i;
    for(i=0; i<length+1; i++) if(i < length) newstr[i] = string[i]; else newstr[i] = 0;
  }
  return newstr;
}

ICACHE_FLASH_ATTR char* str_replace ( char *string, const char *substr, const char *replacement, int length ){
  char *tok = NULL;
  char *newstr = NULL;
  char *oldstr = NULL;
  /* if either substr or replacement is NULL, duplicate string a let caller handle it */
  if ( substr == NULL ) {
    newstr = my_strdup(string, length);
    c_free(string);
    return newstr;
  }
  if( replacement == NULL ) replacement = "";
  //newstr = strdup (string);
  newstr = my_strdup(string, length);
  
  while ( (tok = strstr ( newstr, substr ))){
    oldstr = newstr;
    newstr = c_malloc ( strlen ( oldstr ) - strlen ( substr ) + strlen ( replacement ) + 1 );
    /*failed to alloc mem, free old string and return NULL */
    if ( newstr == NULL ){
      c_free (oldstr);
      return NULL;
    }
    c_memcpy ( newstr, oldstr, tok - oldstr );
    c_memcpy ( newstr + (tok - oldstr), replacement, strlen ( replacement ) );
    c_memcpy ( newstr + (tok - oldstr) + strlen( replacement ), tok + strlen ( substr ), strlen ( oldstr ) - strlen ( substr ) - ( tok - oldstr ) );
    c_memset ( newstr + strlen ( oldstr ) - strlen ( substr ) + strlen ( replacement ) , 0, 1 );
    c_free (oldstr);
  }
  c_free(string);
  return newstr;
}

ICACHE_FLASH_ATTR char* serverParseCGI(char* html, int length)
{
  struct icyHeader *header = clientGetHeader();
  char* h = html;

  h = str_replace(h, "#ICY-NAME#", header->members.single.name, length);
  h = str_replace(h, "#ICY-NOTICE1#", header->members.single.notice1, strlen(h));
  h = str_replace(h, "#ICY-NOTICE2#", header->members.single.notice2, strlen(h));
  h = str_replace(h, "#ICY-GENRE#", header->members.single.genre, strlen(h));
  h = str_replace(h, "#ICY-URL#", header->members.single.url, strlen(h));
  h = str_replace(h, "#ICY-BITRATE#", header->members.single.bitrate, strlen(h));

  return h;
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

	if(length > 0)
	{
		char *con = (char*)c_malloc(length*sizeof(char));
		if(con != NULL)
		{
			fread(con, (uint32_t)content, length);
      if(f->cgi == 1) {
        con = serverParseCGI(con, length);
        length = strlen(con);
      }
      c_sprintf(buf, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n", (f!=NULL ? f->type : "text/plain"), length);
	    espconn_sent(&servConn,buf,strlen(buf)); // SEND HEADER
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
  else
  {
    c_sprintf(buf, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n", (f!=NULL ? f->type : "text/plain"), 0);
	  espconn_sent(&servConn,buf,strlen(buf)); // SEND HEADER
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
