#include "webclient.h"

#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/tcp.h"

#include "esp_common.h"

#include "vs1053.h"

struct icyHeader header = {NULL, NULL, NULL, NULL, 0};
char *clientURL = NULL;
char *clientPath = NULL;
uint16_t clientPort = 80;
struct ip_addr ipAddress;

static const char* icyHeaders[] = { "icy-name:", "icy-notice1:", "icy-notice2:",  "icy-url:", "icy-genre:", "icy-br:", "icy-metaint:" };

static enum clientStatus cstatus;
static uint32_t metacount = 0;
static uint16_t metasize = 0;

static uint8_t connect = 0, playing = 0;


/* TODO:
	- MUTEXES !!!
	- METADATA HANDLING
	- IP SETTINGS
	- VS1053 - DELAY USING vTaskDelay
*/


///////////////
#define BUFFER_SIZE 10240

uint8_t buffer[BUFFER_SIZE];
uint16_t wptr = 0;
uint16_t rptr = 0;
uint8_t bempty = 1;

uint16_t getBufferFree() {
	if(wptr > rptr ) return BUFFER_SIZE - wptr + rptr;
	else if(wptr < rptr) return rptr - wptr;
	else if(bempty) return BUFFER_SIZE; else return 0;
}

uint16_t getBufferFilled() {
	return BUFFER_SIZE - getBufferFree();
}

uint16_t bufferWrite(uint8_t *data, uint16_t size) {
	uint16_t s = size, i = 0;
	for(i=0; i<s; i++) {
		if(getBufferFree() == 0) return i;
		buffer[wptr] = data[i];
		if(bempty) bempty = 0;
		wptr++;
		if(wptr == BUFFER_SIZE) wptr = 0;
	}
	return s;
}

uint16_t bufferRead(uint8_t *data, uint16_t size) {
	uint16_t s = size, i = 0;
	if(s > getBufferFilled()) s = getBufferFilled();
	for (i = 0; i < s; i++) {
		if(getBufferFilled() == 0) return i;
		data[i] = buffer[rptr];
		rptr++;
		if(rptr == BUFFER_SIZE) rptr = 0;
		if(rptr == wptr) bempty = 1;
	}
	return s;
}

void bufferReset() {
	wptr = 0;
	rptr = 0;
	bempty = 1;
}

///////////////

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
					if(header.members.mArr[header_num] != NULL) free(header.members.mArr[header_num]);
					header.members.mArr[header_num] = (char*)malloc((len+1)*sizeof(char));
					if(header.members.mArr[header_num] != NULL)
					{
						int i;
						for(i = 0; i<len+1; i++) header.members.mArr[header_num][i] = 0;
						strncpy(header.members.mArr[header_num], t, len);
					}
				}
				else // Numerical header field
				{
					char *buf = (char*) malloc((len+1)*sizeof(char));
					if(buf != NULL)
					{
						int i;
						for(i = 0; i<len+1; i++) buf[i] = 0;
						strncpy(buf, t, len);
						header.members.single.metaint = atoi(buf);
						free(buf);
					}
				}
			}
		}
	}
}

ICACHE_FLASH_ATTR uint16_t clientProcessMetadata(char* s, uint16_t size)
{
	uint16_t processed = 0;
	if(metasize == 0) { metasize = s[0]*16; processed = 1; }
	if(metasize == 0) return 1; // THERE IS NO METADATA
	
	if(processed == 1) // BEGINNING OF NEW METADATA; PREPARE MEMORY SPACE
	{
		if(header.members.single.metadata != NULL) free(header.members.single.metadata);
		header.members.single.metadata = (char*) malloc((metasize+1) * sizeof(char));
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
		printf("\n");
		printf(header.members.single.metadata);
	}
	return processed;
}

ICACHE_FLASH_ATTR void clientSetURL(char* url)
{
	int l = strlen(url)+1;
	if(clientURL != NULL) free(clientURL);
	clientURL = (char*) malloc(l*sizeof(char));
	if(clientURL != NULL) strcpy(clientURL, url);
	printf("\n##CLI.URLSET#\n");
}

ICACHE_FLASH_ATTR void clientSetPath(char* path)
{
	int l = strlen(path)+1;
	if(clientPath != NULL) free(clientPath);
	clientPath = (char*) malloc(l*sizeof(char));
	if(clientPath != NULL) strcpy(clientPath, path);
	printf("\n##CLI.PATHSET#\n");
}

ICACHE_FLASH_ATTR void clientSetPort(uint16_t port)
{
	clientPort = port;
	printf("\n##CLI.PORTSET#\n");
}

ICACHE_FLASH_ATTR void clientConnect()
{
	cstatus = C_HEADER;
	metacount = 0;
	metasize = 0;
	
	if(netconn_gethostbyname(clientURL, &ipAddress) == ERR_OK) {
		connect = 1; // todo: semafor!!!
	} else {
		clientDisconnect();
	}
}

ICACHE_FLASH_ATTR void clientDisconnect()
{
	connect = 0;
	printf("\n##CLI.STOPPED#\n");
}

ICACHE_FLASH_ATTR void clientReceiveCallback(void *arg, char *pdata, unsigned short len)
{
	/* TODO:
		- What if header is in more than 1 data part?
		- Metadata processing
		- Buffer underflow handling (?)
	*/
	if(cstatus == C_HEADER)	{
		clientParseHeader(pdata);
		char *t1 = strstr(pdata, "\r\n\r\n"); // END OF HEADER
		if(t1 != NULL) {
			//processed = t1-pdata + 4;
			cstatus = C_DATA;
		}
		else return;
	} else {
		uint16_t l = 0;
		do {
			if(getBufferFree() < len) vTaskDelay(1);
			else l = bufferWrite(pdata, len);
		} while(l < len);
		
		if(!playing && getBufferFree() < BUFFER_SIZE/2) { 
			playing=1;
		}	
	}
}

void vsTask(void *pvParams) {
	uint8_t b[1024];
	while(1) {
		if(playing) {
			uint16_t size = bufferRead(b, 1024), s = 0;
			while(s < size) {
				s += VS1053_SendMusicBytes(b+s, size-s);
				vTaskDelay(2);
			}
		} else vTaskDelay(10);
	}
}

void clientTask(void *pvParams) {
	struct netconn *NetConn = NULL;
	struct netbuf *inbuf;
	
	err_t rc1, rc2, rc3;
	
	while(1) {
		
		if(connect) {

			NetConn = netconn_new(NETCONN_TCP);
			netconn_set_recvtimeout(NetConn, 1000);

			if(NetConn == NULL){
					/*No memory for new connection? */
					printf("No mem for new con\r\n");
			}
			rc1 = netconn_bind(NetConn, NULL, 666);        //3250          /* Adres IP i port local host'a */
			printf("netcon binded\r\n");
			rc2 = netconn_connect(NetConn, &ipAddress, clientPort);     // todo: !!!         /* Adres IP i port serwera */
			//rc2 = netconn_connect(NetConn, &ipaddrserv, 80); 
			printf("netcon connected\r\n");

			if(rc1 != ERR_OK || rc2 != ERR_OK){
					netconn_delete(NetConn);
					printf("connection error\r\n");
					clientDisconnect();
					continue;
			} else {
				char *getQuery = malloc(sizeof(char)*(strlen(clientPath) + 18));
				sprintf(getQuery, "GET %s HTTP/1.0\r\n\r\n", clientPath);
				netconn_write(NetConn, getQuery, strlen(getQuery), NETCONN_NOCOPY);
				free(getQuery);
				while(1){
					if(netconn_recv(NetConn, &inbuf) != ERR_OK) break;
					int BufLen = netbuf_len(inbuf);
					char *tmp = malloc(BufLen);
					netbuf_copy(inbuf, tmp, BufLen);
					netbuf_delete(inbuf);
					clientReceiveCallback(NULL, tmp, BufLen);

					free(tmp);
					if(connect == 0) break;
					vTaskDelay(1);
				}

			}

			playing = 0;
			bufferReset();
			netconn_close(NetConn);
			printf("netcon closed\r\n");
			netconn_delete(NetConn);
			printf("netcon deleted\r\n");
		
		}

		vTaskDelay(10);
	}
}
