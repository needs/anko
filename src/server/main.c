#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include <shared/network.h>
#include <shared/packet.h>
#include <shared/player_array.h>
#include <game/player.h>

#define MAX_PLAYERS 16

struct slot {
	unsigned char is_used;
	struct sockaddr_storage addr;
};

static struct slot slots[16];

static int sockaddr_storage_is_same(
	const struct sockaddr_storage *addr1, const struct sockaddr_storage *addr2)
{
	assert(addr1 != NULL);
	assert(addr2 != NULL);
	assert(addr1->ss_family == AF_INET || addr1->ss_family == AF_INET6);
	assert(addr2->ss_family == AF_INET || addr2->ss_family == AF_INET6);

	if (addr1->ss_family != addr2->ss_family)
		return 0;

	if (addr1->ss_family == AF_INET) {
		const struct sockaddr_in *in1 = (struct sockaddr_in*)addr1;
		const struct sockaddr_in *in2 = (struct sockaddr_in*)addr2;

		if (in1->sin_port != in2->sin_port)
			return 0;
		if (memcmp(&in1->sin_addr.s_addr, &in2->sin_addr.s_addr, sizeof(*in1)))
			return 0;
	} else {
		const struct sockaddr_in6 *in1 = (struct sockaddr_in6*)addr1;
		const struct sockaddr_in6 *in2 = (struct sockaddr_in6*)addr2;

		if (in1->sin6_port != in2->sin6_port)
			return 0;
		if (memcmp(&in1->sin6_addr.s6_addr, &in2->sin6_addr.s6_addr, sizeof(*in1)))
			return 0;
	}

	return 1;
}

static struct slot *get_addr_slot(struct sockaddr_storage *addr)
{
	struct slot *unused_slot = NULL;
	unsigned i;

	for (i = 0; i < MAX_PLAYERS; i++) {
		if (!slots[i].is_used) {
			if (!unused_slot)
				unused_slot = &slots[i];
		} else if (sockaddr_storage_is_same(&slots[i].addr, addr)) {
			return &slots[i];
		}
	}
	return unused_slot;
}

static size_t build_packet(char *buf, struct player_array *array)
{
	struct packet packet = PACKET_ZERO;

	packet.ack = array->current->seq;
	printf("Send packet with ack = %lu\n", (unsigned long)packet.ack);
	return pack_packet(buf, &packet) - buf;
}

static int push_network(int fd, struct player_array *array)
{
	ssize_t ret;
	size_t len;
	static char buf[MAX_PACKET_SIZE];

	if (array->current->confirmed)
		return 1;

	len = build_packet(buf, array);
	ret = sendto(fd, buf, len, 0,
	             (const struct sockaddr*)&slots[0].addr, sizeof(slots[0].addr));
	if (ret == -1) {
		fprintf(stderr, "sendto(): %s\n", strerror(errno));
		return 0;
	}
	array->current->confirmed = 1;

	return 1;
}

static void handle_packet(struct packet *packet, struct player_array *array)
{
	struct player_array_entry *from, *to;

	printf("Receive packet with seq_from = %lu, seq_to = %lu, ack = %lu\n",
	       (unsigned long)packet->seq_from, (unsigned long)packet->seq_to, (unsigned long)packet->ack);

	from = player_array_get_entry_by_seq(array, packet->seq_from);
	if (!from) {
		printf("Source snapshot not already received by the client, diff can't be applied\n");
		return;
	}

	to = player_array_get_entry_by_seq(array, packet->seq_to);
	if (to) {
		printf("Destination snapshot already received, ignoring diff\n");
		return;
	}

	/*
	 * In the futur, in may be possible to free some old snapshot, likely
	 * the one who have less chance to be used again.  For now, just fail.
	 */
	if (!player_array_forward(array)) {
		printf("Snapshot list is too long, packet ignored\n");
		return;
	}

	to = array->current;
	to->seq = packet->seq_to;
	apply_player_diff(&packet->diff, &to->player);

	to->confirmed = 0;
	from->confirmed = 1;
}

static int poll_network(int fd, struct player_array *array)
{
	static char buf[MAX_PACKET_SIZE];
	struct sockaddr_storage addr;
	socklen_t addrlen = sizeof(addr);
	ssize_t ret;
	struct slot *slot;
	struct packet packet;

	ret = recvfrom(fd, buf, MAX_PACKET_SIZE, 0, (struct sockaddr*)&addr, &addrlen);
	if (ret == -1) {
		fprintf(stderr, "recvfrom(): %s\n", strerror(errno));
		return 0;
	}

	if (!check_packet_size(ret))
		return 1;

	slot = get_addr_slot(&addr);
	if (!slot) {
		fprintf(stderr, "New connection rejected: server is full\n");
		return 0;
	} else if (!slot->is_used) {
		puts("New connection");
		slot->addr = addr;
		slot->is_used = 1;
	} else {
		puts("Already connected");
	}

	unpack_packet(buf, &packet);
	handle_packet(&packet, array);

	return 1;
}

int main(int argc, char **argv)
{
	int fd;
	struct player_array array;

	if (argc < 2) {
		fprintf(stderr, "usage: %s <host> [<port>]\n", argv[0]);
		return EXIT_FAILURE;
	}

	if (!player_array_init(&array, PLAYER_ARRAY_DEFAULT_LEN))
		return EXIT_FAILURE;
	if ((fd = bind_to(argv[1], argv[2])) == -1)
		return EXIT_FAILURE;

	/*
	 * The main loop for the server should be:
	 *   1) poll_network(network, diff)
	 *   2) logic(from, event, to)
	 *   3) push_network(network, to)
	 */

	while (1) {
		poll_network(fd, &array);
		push_network(fd, &array);
		printf("Player name: %s\n", array.current->player.name);
		sleep(1);
	}

	close(fd);
	player_array_free(&array);

	return EXIT_SUCCESS;
}
