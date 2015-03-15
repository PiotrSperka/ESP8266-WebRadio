#include "c_types.h"

#define ICY_HEADERS_COUNT 6

struct icyHeader
{
	union
	{
		char* mArr[ICY_HEADERS_COUNT];
		struct
		{
			char* name;
			char* notice1;
			char* notice2;
			char* url;
			char* genre;
			char* bitrate;
		} single;
	} members;
};

void clientSetURL(char* url);
void clientSetPath(char* path);
void clientSetPort(uint16_t port);
struct icyHeader* clientGetHeader();
void clientConnect();
void clientDisconnect();