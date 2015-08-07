#include "webserver.h"
#include "serv-fs.h"
#include "webclient.h"

#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"

//const static char http_html_hdr[] = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
//const static char http_index_html[] = "<html><head><title>Congrats!</title></head><body><h1>Welcome to our lwIP HTTP server!</h1><p>This is a small test page, served by httpserver-netconn.</body></html>";

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
/*
char *str_replace(char *orig, char *rep, char *with) {
    char *result; // the return string
    char *ins;    // the next insert point
    char *tmp;    // varies
    int len_rep;  // length of rep
    int len_with; // length of with
    int len_front; // distance between rep and end of last rep
    int count;    // number of replacements

    if (!orig)
        return NULL;
    if (!rep)
        rep = "";
    len_rep = strlen(rep);
    if (!with)
        with = "";
    len_with = strlen(with);

    ins = orig;
    for (count = 0; tmp = strstr(ins, rep); ++count) {
        ins = tmp + len_rep;
    }

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}
*/

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
  //newstr = strdup (string);
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

ICACHE_FLASH_ATTR void serveFile(char* name, struct netconn *conn)
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
			netconn_write(conn, buf, strlen(buf), NETCONN_NOCOPY); // SEND HEADER
			netconn_write(conn, con, length, NETCONN_NOCOPY); // SEND CONTENT
			free(con);
		}
	}
	else
	{
		sprintf(buf, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n", (f!=NULL ? f->type : "text/plain"), 0);
		netconn_write(conn, buf, strlen(buf), NETCONN_NOCOPY); // SEND HEADER
	}
}

static void http_server_netconn_serve(struct netconn *conn)
{
  struct netbuf *inbuf;
  char *buf;
  u16_t buflen;
  err_t err;
  
  // Read the data from the port, blocking if nothing yet there. 
  // We assume the request (the part we care about) is in one netbuf
  err = netconn_recv(conn, &inbuf);
  
  if (err == ERR_OK) {
    netbuf_data(inbuf, (void**)&buf, &buflen);
    
	// Look for GET command
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
	}
  }
  // Close the connection (server closes in HTTP)
  netconn_close(conn);
  
  // Delete the buffer (netconn_recv gives us ownership,
  // so we have to make sure to deallocate the buffer)
  netbuf_delete(inbuf);
}


void serverTask(void *pvParams) {
  struct netconn *conn, *newconn;
  err_t err;
  
  // Create a new TCP connection handle
  conn = netconn_new(NETCONN_TCP);
  LWIP_ERROR("http_server: invalid conn", (conn != NULL), return;);
  
  // Bind to port 80 (HTTP) with default IP address
  netconn_bind(conn, NULL, 80);
  
  // Put the connection into LISTEN state
  netconn_listen(conn);
  
  do {
    err = netconn_accept(conn, &newconn);
    if (err == ERR_OK) {
      http_server_netconn_serve(newconn);
      netconn_delete(newconn);
    }
  } while(err == ERR_OK);
  LWIP_DEBUGF(HTTPD_DEBUG,
    ("http_server_netconn_thread: netconn_accept received error %d, shutting down",
    err));
  netconn_close(conn);
  netconn_delete(conn);	
}
