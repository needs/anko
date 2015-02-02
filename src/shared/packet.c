#include <assert.h>
#include <stdio.h>

#include "packet.h"
#include "pack_unpack.h"
#include <game/player.h>

char *pack_packet(char *buf, struct packet *packet)
{
	assert(buf != NULL);
	assert(packet != NULL);

	pack_uint16(&buf, packet->seq_from);
	pack_uint16(&buf, packet->seq_to);
	pack_uint16(&buf, packet->ack);
	buf = pack_player_diff(buf, &packet->diff);

	return buf;
}

char *unpack_packet(char *buf, struct packet *packet)
{
	assert(buf != NULL);
	assert(packet != NULL);

	packet->seq_from = unpack_uint16(&buf);
	packet->seq_to = unpack_uint16(&buf);
	packet->ack = unpack_uint16(&buf);
	buf = unpack_player_diff(buf, &packet->diff);

	return buf;
}

int check_packet_size(ssize_t size)
{
	assert(size >= 0);

	if ((unsigned)size > MAX_PACKET_SIZE) {
		fprintf(stderr, "Packet rejected: too large (%li bytes).\n",
			(long)size);
		return 0;
	} else if ((unsigned)size < MIN_PACKET_SIZE) {
		fprintf(stderr, "Packet rejected: too small (%li bytes).\n",
			(long)size);
		return 0;
	}

	return 1;
}
