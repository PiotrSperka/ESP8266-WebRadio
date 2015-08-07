#include "c_types.h"

#define ICY_HEADERS_COUNT 7

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
			char* metadata;
			int metaint;
		} single;
	} members;
};

enum clientStatus { C_HEADER, C_METADATA, C_DATA };

void clientSetURL(char* url);
void clientSetPath(char* path);
void clientSetPort(uint16_t port);
struct icyHeader* clientGetHeader();
void clientConnect();
void clientDisconnect();
void clientTask(void *pvParams);

void vsTask(void *pvParams) ;
