#include "data.h"

struct packet {
	unsigned char PacketType;
#define Received 'R'				//HELLO
#define Unreceived 'U'
	struct request req;
};