#ifndef GAME_PACKET_H_
#define GAME_PACKET_H_

#include <stdint.h>
#include <stdlib.h>
#include <game/player.h>

#define MAX_PACKET_SIZE sizeof(struct packet)
#define MIN_PACKET_SIZE (3 * sizeof(uint16_t))

struct packet {
	uint16_t seq_from, seq_to, ack;
	struct player_diff diff;
};

static const struct packet PACKET_ZERO;

char *pack_packet(char *buf, struct packet *packet);
char *unpack_packet(char *buf, struct packet *packet);
int check_packet_size(ssize_t size);

#endif /* GAME_PACKET_H_ */
