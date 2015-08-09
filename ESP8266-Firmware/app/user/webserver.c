#include "webserver.h"
#include "serv-fs.h"
#include "webclient.h"
#include "vs1053.h"

#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

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
  char* newstr = (char*)malloc((length+1)*sizeof(char));
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
    free(string);
    return newstr;
  }
  if( replacement == NULL ) replacement = "";
  newstr = my_strdup(string, length);
  
  while ( (tok = strstr ( newstr, substr ))){
    oldstr = newstr;
    newstr = malloc ( strlen ( oldstr ) - strlen ( substr ) + strlen ( replacement ) + 1 );
    /*failed to alloc mem, free old string and return NULL */
    if ( newstr == NULL ){
      free (oldstr);
      return NULL;
    }
    memcpy ( newstr, oldstr, tok - oldstr );
    memcpy ( newstr + (tok - oldstr), replacement, strlen ( replacement ) );
    memcpy ( newstr + (tok - oldstr) + strlen( replacement ), tok + strlen ( substr ), strlen ( oldstr ) - strlen ( substr ) - ( tok - oldstr ) );
    memset ( newstr + strlen ( oldstr ) - strlen ( substr ) + strlen ( replacement ) , 0, 1 );
    free (oldstr);
  }
  free(string);
  return newstr;
}


ICACHE_FLASH_ATTR char* serverParseCGI(char* html, int length)
{
  struct icyHeader *header = clientGetHeader();
  char* h = html;
  char buf[15];

  h = str_replace(h, "#ICY-NAME#", header->members.single.name, length);
  h = str_replace(h, "#ICY-NOTICE1#", header->members.single.notice1, strlen(h));
  h = str_replace(h, "#ICY-NOTICE2#", header->members.single.notice2, strlen(h));
  h = str_replace(h, "#ICY-GENRE#", header->members.single.genre, strlen(h));
  h = str_replace(h, "#ICY-URL#", header->members.single.url, strlen(h));
  h = str_replace(h, "#ICY-BITRATE#", header->members.single.bitrate, strlen(h));
  
  sprintf(buf, "%d", 254-VS1053_GetVolume());
  h = str_replace(h, "#SOUND-VOL#", buf, strlen(h));
  sprintf(buf, "%d", VS1053_GetTreble());
  h = str_replace(h, "#SOUND-TREBLE#", buf, strlen(h));
  sprintf(buf, "%d", VS1053_GetBass());
  h = str_replace(h, "#SOUND-BASS#", buf, strlen(h));

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

ICACHE_FLASH_ATTR void serveFile(char* name, int conn)
{
	int length;
	char buf[128];
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
		char *con = (char*)malloc(length*sizeof(char));
		if(con != NULL)
		{
			fread(con, (uint32_t)content, length);
			if(f->cgi == 1) {
				con = serverParseCGI(con, length);
				length = strlen(con);
			}
			sprintf(buf, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %d\r\nConnection: close\r\n\r\n", (f!=NULL ? f->type : "text/plain"), length);
			write(conn, buf, strlen(buf));
			write(conn, con, length);
			free(con);
		}
	}
	else
	{
		sprintf(buf, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n", (f!=NULL ? f->type : "text/plain"), 0);
		write(conn, buf, strlen(buf));
	}
}

char* getParameterFromResponse(char* param, char* data, uint16_t data_length) {
	char* p = strstr(data, param);
	if(p > 0) {
		p += strlen(param);
		char* p_end = strstr(p, "&");
		if(p_end <= 0) p_end = data_length + data;
		if(p_end > 0) {
			char* t = malloc(p_end-p + 1);
			int i;
			for(i=0; i<(p_end-p + 1); i++) t[i] = 0;
			strncpy(t, p, p_end-p);
			t = str_replace(t, "%2F", "/", strlen(t));
			return t;
		}
	} else return NULL;
}

ICACHE_FLASH_ATTR void handlePOST(char* name, char* data, int data_size, int conn) {
	if(strcmp(name, "/instant_play") == 0) {
		if(data_size > 0) { 
			char* url = getParameterFromResponse("url=", data, data_size);
			char* path = getParameterFromResponse("path=", data, data_size);
			char* port = getParameterFromResponse("port=", data, data_size);
			if(url != NULL && path != NULL && port != NULL) {
				clientDisconnect();
				while(clientIsConnected()) vTaskDelay(10);
				clientSetURL(url);
				clientSetPath(path);
				clientSetPort(atoi(port));
				clientConnect();
				while(!clientIsConnected()) vTaskDelay(10);
			}
			if(url) free(url);
			if(path) free(path);
			if(port) free(port);
		}
	} else if(strcmp(name, "/sound") == 0) {
		if(data_size > 0) { 
			char* vol = getParameterFromResponse("vol=", data, data_size);
			char* bass = getParameterFromResponse("bass=", data, data_size);
			char* treble = getParameterFromResponse("treble=", data, data_size);
			if(vol) {
				VS1053_SetVolume(254-atoi(vol));
				free(vol);
			}
			if(bass) {
				VS1053_SetBass(atoi(bass));
				free(bass);
			}
			if(treble) {
				VS1053_SetTreble(atoi(treble));
				free(treble);
			}
		}
	}
	
	serveFile("/", conn); // Return back to index.html
}

void httpServerHandleConnection(int conn, char* buf, uint16_t buflen) {
	char *c;
	if( (c = strstr(buf, "GET ")) != NULL)
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
		serveFile(fname, conn);
	} else if( (c = strstr(buf, "POST ")) != NULL) {
		char fname[32];
		uint8_t i;
		for(i=0; i<32; i++) fname[i] = 0;
		c += 5;
		char* c_end = strstr(c, " ");
		if(c_end == NULL) return;
		uint8_t len = c_end-c;
		if(len > 32) return;
		strncpy(fname, c, len);
		printf("Name: %s\n", fname);
		// DATA
		char* d_start = strstr(buf, "\r\n\r\n");
		if(d_start > 0) {
			d_start += 4;
			uint16_t len = buflen - (d_start-buf);
			handlePOST(fname, d_start, len, conn);
		}
	}	
}

void serverTask(void *pvParams) {
	struct sockaddr_in server_addr, client_addr;
	int server_sock, client_sock;
	socklen_t sin_size;
	
	while (1) {
        bzero(&server_addr, sizeof(struct sockaddr_in));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(80);

        int recbytes;

        do {
            if (-1 == (server_sock = socket(AF_INET, SOCK_STREAM, 0))) {
                break;
            }

            if (-1 == bind(server_sock, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr))) {
                break;
            }

            if (-1 == listen(server_sock, 5)) {
                break;
            }

            sin_size = sizeof(client_addr);

            while(1) {
                if ((client_sock = accept(server_sock, (struct sockaddr *) &client_addr, &sin_size)) < 0) {
                    continue;
                }

                char *buf = (char *)zalloc(2048);
                while ((recbytes = read(client_sock , buf, 2047)) > 0) { // For now we assume max. 2047 bytes for request
					httpServerHandleConnection(client_sock, buf, recbytes);
                }
                free(buf);

                if (recbytes <= 0) {
                    close(client_sock);
                }
            }
        } while (0);
    }	
}
