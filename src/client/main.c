#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <shared/network.h>
#include <shared/packet.h>
#include <shared/player_array.h>

static size_t build_packet(char *buf, struct player_array *array)
{
	struct packet packet;

	packet.seq_from = array->last->seq;
	packet.seq_to = array->current->seq;
	packet.ack = 0;
	printf("Send packet with seq_from = %lu, seq_to = %lu, ack = %lu\n",
	       (unsigned long)packet.seq_from, (unsigned long)packet.seq_to, (unsigned long)packet.ack);

	diff_player(&array->last->player, &array->current->player, &packet.diff);
	return pack_packet(buf, &packet) - buf;
}

static int push_network(int fd, struct player_array *array)
{
	ssize_t ret;
	static char buf[MAX_PACKET_SIZE];
	size_t len;

	if (array->last->confirmed)
		return 1;

	len = build_packet(buf, array);
	ret = send(fd, buf, len, 0);
	if (ret == -1) {
		fprintf(stderr, "send(len = %lu): %s\n", (unsigned long)len, strerror(errno));
		return 0;
	}
	array->last->confirmed = 1;

	return 1;
}

static void handle_packet(struct packet *packet, struct player_array *array)
{
	struct player_array_entry *entry;

	printf("Receive packet with ack = %lu\n", (unsigned long)packet->ack);

	entry = player_array_get_entry_by_seq(array, packet->ack);
	if (!entry)
		return;

	array->last = entry;
	array->last->confirmed = 0;
}

static int poll_network(int fd, struct player_array *array)
{
	ssize_t ret;
	static char buf[MAX_PACKET_SIZE];
	struct packet packet;

	ret = recvfrom(fd, buf, MAX_PACKET_SIZE, 0, NULL, NULL);
	if (ret == -1) {
		fprintf(stderr, "recvfrom(): %s\n", strerror(errno));
		return 0;
	}

	if (!check_packet_size(ret))
		return 1;

	unpack_packet(buf, &packet);
	handle_packet(&packet, array);

	return 1;
}

static int change_our_player_name(struct player_array *array, char *name)
{
	struct player *old;

	old = &array->current->player;
	if (!player_array_forward(array))
		return 0;

	array->current->player = *old;
	strcpy(array->current->player.name, name);
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
	if ((fd = connect_to(argv[1], argv[2])) == -1)
		return EXIT_FAILURE;

	change_our_player_name(&array, "regate");

	/*
	 * The main loop for the client should be:
	 *   1) poll_network(network, diff)
	 *   2) get_event(event)
	 *   3) push_network(network, event)
	 *   4) apply_diff(world, diff)
	 *   5) predict(world)
	 *   6) render(world)
	 */
	while (1) {
		push_network(fd, &array);
		poll_network(fd, &array);
		sleep(1);
	}

	close(fd);
	player_array_free(&array);

	return EXIT_SUCCESS;
}
