#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include <stdint.h>

#define PACKET_SIZE sizeof(struct packet)
#define BUFSIZE 128

struct packet {
	uint16_t ack;
	uint8_t buf[BUFSIZE];
};

char *pack_packet(char *buf, struct packet *packet);
char *unpack_packet(char *buf, struct packet *packet);

#endif
