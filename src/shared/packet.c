#include <assert.h>

#include "packet.h"
#include "pack_unpack.h"

char *pack_packet(char *buf, struct packet *packet)
{
	assert(buf != NULL);
	assert(packet != NULL);

	pack_uint16(&buf, packet->ack);
	memcpy(buf, packet->buf, BUFSIZE);

	return buf + BUFSIZE;
}

char *unpack_packet(char *buf, struct packet *packet)
{
	assert(buf != NULL);
	assert(packet != NULL);

	packet->ack = unpack_uint16(&buf);
	memcpy(packet->buf, buf, BUFSIZE);

	return buf + BUFSIZE;
}
