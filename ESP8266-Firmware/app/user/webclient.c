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

static const char* icyHeaders[] = { "icy-name:", "icy-notice1:", "icy-notice2:",  "icy-url:", "icy-genre:", "icy-br:", "icy-metaint:" };

static enum clientStatus cstatus;
static uint32_t metacount = 0;
static uint16_t metasize = 0;

ICACHE_FLASH_ATTR struct icyHeader* clientGetHeader()
{
	return &header;
}

ICACHE_FLASH_ATTR void clientParseHeader(char* s)
{
	// icy-notice1 icy-notice2 icy-name icy-genre icy-url icy-br
	uint8_t header_num;
	for(header_num=0; header_num<ICY_HEADERS_COUNT; header_num++)
	{
		char *t;
		t = strstr(s, icyHeaders[header_num]);
		if( t != NULL )
		{
			t += strlen(icyHeaders[header_num]);
			char *t_end = strstr(t, "\r\n");
			if(t_end != NULL)
			{
				uint16_t len = t_end - t;
				if(header_num != 6) // Text header field
				{
					if(header.members.mArr[header_num] != NULL) c_free(header.members.mArr[header_num]);
					header.members.mArr[header_num] = (char*)c_malloc((len+1)*sizeof(char));
					if(header.members.mArr[header_num] != NULL)
					{
						int i;
						for(i = 0; i<len+1; i++) header.members.mArr[header_num][i] = 0;
						strncpy(header.members.mArr[header_num], t, len);
					}
				}
				else // Numerical header field
				{
					char *buf = (char*) c_malloc((len+1)*sizeof(char));
					if(buf != NULL)
					{
						int i;
						for(i = 0; i<len+1; i++) buf[i] = 0;
						strncpy(buf, t, len);
						header.members.single.metaint = atoi(buf, 10);
						c_free(buf);
					}
				}
			}
		}
	}
}

ICACHE_FLASH_ATTR uint16_t clientProcessData(char* s, uint16_t size)
{
	int i;
	for(i=0; i<size; i++) uart_tx_one_char(0, s[i]);
	return size;
}

ICACHE_FLASH_ATTR uint16_t clientProcessMetadata(char* s, uint16_t size)
{
	uint16_t processed = 0;
	if(metasize == 0) { metasize = s[0]*16; processed = 1; }
	if(metasize == 0) return 1; // THERE IS NO METADATA
	
	if(processed == 1) // BEGINNING OF NEW METADATA; PREPARE MEMORY SPACE
	{
		if(header.members.single.metadata != NULL) c_free(header.members.single.metadata);
		header.members.single.metadata = (char*) c_malloc((metasize+1) * sizeof(char));
		if(header.members.single.metadata == NULL)
		{
			cstatus = C_DATA;
			return metasize;
		}
		int i;
		for(i=0; i<metasize+1; i++) header.members.single.metadata[i] = 0;
	}
	uint16_t startpos = 0;
	while(header.members.single.metadata[startpos] != 0) startpos++; // FIND ENDING OF METADATA
	if((size-processed) >= metasize)
	{
		int i;
		for(i=0; i<metasize; i++) header.members.single.metadata[startpos+i] = s[processed+i];
		processed += metasize;
	}
	else
	{
		int i;
		for(i=0; i<(size-processed); i++) header.members.single.metadata[startpos+i] = s[processed+i];
		processed += (size-processed);
		metasize -= (size-processed);
	}
	if(metasize == 0) {
		cstatus = C_DATA; // METADATA READ - BACK TO STREAM DATA
		// DEBUG
		uart0_sendStr("\n");
		uart0_sendStr(header.members.single.metadata);
	}
	return processed;
}

ICACHE_FLASH_ATTR void clientSetURL(char* url)
{
	int l = strlen(url)+1;
	if(clientURL != NULL) c_free(clientURL);
	clientURL = (char*) c_malloc(l*sizeof(char));
	if(clientURL != NULL) strcpy(clientURL, url);
	uart0_sendStr("\n##CLI.URLSET#\n");
}

ICACHE_FLASH_ATTR void clientSetPath(char* path)
{
	int l = strlen(path)+1;
	if(clientPath != NULL) c_free(clientPath);
	clientPath = (char*) c_malloc(l*sizeof(char));
	if(clientPath != NULL) strcpy(clientPath, path);
	uart0_sendStr("\n##CLI.PATHSET#\n");
}

ICACHE_FLASH_ATTR void clientSetPort(uint16_t port)
{
	clientPort = port;
	uart0_sendStr("\n##CLI.PORTSET#\n");
}

ICACHE_FLASH_ATTR void clientReceiveCallback(void *arg, char *pdata, unsigned short len)
{
	//uint16_t processed = 0;
	clientProcessData(pdata, len);
	if(cstatus == C_HEADER)	{
		clientParseHeader(pdata);
		char *t1 = strstr(pdata, "\r\n\r\n"); // END OF HEADER
		if(t1 != NULL) {
			//processed = t1-pdata + 4;
			cstatus = C_DATA;
		}
		else return;
	}
/*	while(processed < len)
	{
		if(header.members.single.metaint == 0) {
			clientProcessData(pdata, len-processed); // NO METADATA
			processed += (len-processed);
		}
		else
		{
			uint16_t maxdata = len-processed;
			if(cstatus == C_DATA)
			{
				if((header.members.single.metaint - metacount) >= maxdata)
				{
					clientProcessData(pdata+processed, maxdata);
					metacount += maxdata;
					processed += maxdata;
				}
				else
				{
					clientProcessData(pdata+processed, (header.members.single.metaint - metacount) );
					metacount += (header.members.single.metaint - metacount);
					processed += (header.members.single.metaint - metacount);
				}
				
				if(metacount == header.members.single.metaint)
				{
					metacount = 0;
					cstatus = C_METADATA;
				}
				continue;
			}
			else if(cstatus == C_METADATA)
			{
				uint16_t t = clientProcessMetadata(pdata+processed, maxdata);
				processed += t;
			}
		}
	}*/
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
	uint16_t len = strlen(clientPath)+34;
	uint16_t i;
	char *data = (char*) c_malloc(len*sizeof(char));
	if(data != NULL)
	{
		for(i=0; i<len; i++) data[i] = 0;
		//c_sprintf(data, "GET %s HTTP/1.0\r\nicy-metadata:1\r\n\r\n", clientPath);
		c_sprintf(data, "GET %s HTTP/1.0\r\n\r\n", clientPath); // NO METADATA FOR DEBUG PURPOSES
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

ICACHE_FLASH_ATTR void clientConnect()
{
	cstatus = C_HEADER;
	metacount = 0;
	metasize = 0;
	
	clientConn.type = ESPCONN_TCP;
	clientConn.state = ESPCONN_NONE;
	clientTcp.remote_port = clientPort;
	clientTcp.local_port = espconn_port();
	
	clientConn.proto.tcp = &clientTcp;
	
	espconn_disconnect(&clientConn);
	
	espconn_gethostbyname(&clientConn, clientURL, &clientIP, clientIpFoundCallback);
	uart0_sendStr("\n##CLI.DNSLOOKUP#\n");
}

ICACHE_FLASH_ATTR void clientDisconnect()
{
	espconn_disconnect(&clientConn);
	int i;
	for(i=0; i<ICY_HEADERS_COUNT; i++)
	{
		if(i<6)
		{
			if(header.members.mArr[i] != NULL)
			{
				c_free(header.members.mArr[i]);
				header.members.mArr[i] = NULL;
			}
		}
		else
		{
			header.members.single.metaint = 0;
		}
	}
	uart0_sendStr("\n##CLI.STOPPED#\n");
}
