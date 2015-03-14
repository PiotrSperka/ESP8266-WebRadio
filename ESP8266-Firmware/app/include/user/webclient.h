#include "c_types.h"

struct icyHeader
{
	char* name;
	char* notice;
	char* url;
	char* genre;
	uint16_t bitrate;
};

void clientSetURL(char* url);
void clientSetPath(char* path);
void clientSetPort(uint16_t port);
struct icyHeader* clientGetHeader();
void clientConnect();
void clientDisconnect();