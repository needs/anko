#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#include <shared/network.h>
#include <shared/packet.h>
#include <shared/player_array.h>
#include <shared/netclient.h>
#include <game/player.h>

#define MAX_PLAYERS 16

struct netclient_list netclients_list;

static size_t build_packet(char *buf, struct player_array_entry *entry)
{
	struct packet packet = PACKET_ZERO;

	assert(entry != NULL);

	packet.ack = entry->seq;
	printf("Send packet with ack = %lu\n", (unsigned long)packet.ack);
	return pack_packet(buf, &packet) - buf;
}

static int push_network(int fd, struct player_array *array)
{
	ssize_t ret;
	size_t len;
	static char buf[MAX_PACKET_SIZE];
	struct player_array_entry *entry;
	struct netclient *client;

	entry = player_array_get_most_recent_entry(array);
	if (entry->acknowledged)
		return 1;

	len = build_packet(buf, entry);

	/* For now, only push to the first client (unsafe and dirty) */
	client = &netclients_list.clients[0];

	ret = sendto(fd, buf, len, 0,
	             (const struct sockaddr*)&client->addr, sizeof(client->addr));
	if (ret == -1) {
		fprintf(stderr, "sendto(): %s\n", strerror(errno));
		return 0;
	}

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

	to = player_array_get_unused_entry(array);
	if (!to) {
		printf("Snapshot list is too long, packet ignored\n");
		return;
	}

	to->seq = packet->seq_to;
	to->player = from->player;
	apply_player_diff(&packet->diff, &to->player);

	player_array_add(array, to);
	player_array_acknowledge_entry(array, from);
}

/* For debugging purpose */
static void print_netclient(struct netclient *client)
{
	int ret;
	char host[NI_MAXHOST], serv[NI_MAXSERV];

	ret = getnameinfo((struct sockaddr*)&client->addr, sizeof(client->addr),
	                  host, sizeof(host), serv, sizeof(serv), NI_DGRAM);
	if (ret)
		fprintf(stderr, "Cannot retrieve hostname and port: %s\n", gai_strerror(ret));
	else
		printf("\tIP: %s\n\tPort: %s\n", host, serv);
}

static int poll_network(int fd, struct player_array *array)
{
	static char buf[MAX_PACKET_SIZE];
	struct sockaddr_storage addr;
	socklen_t addrlen = sizeof(addr);
	ssize_t ret;
	struct netclient *client;
	struct packet packet;

	ret = recvfrom(fd, buf, MAX_PACKET_SIZE, 0, (struct sockaddr*)&addr, &addrlen);
	if (ret == -1) {
		fprintf(stderr, "recvfrom(): %s\n", strerror(errno));
		return 0;
	}

	/* Packets with wrong size are not an error, just ignore them */
	if (!check_packet_size(ret))
		return 1;

	client = get_netclient(&netclients_list, &addr);
	if (client) {
		puts("Already connected");
	} else {
		puts("New connection");

		client = add_netclient(&netclients_list, &addr);
		if (!client) {
			fprintf(stderr, "New connection rejected: server is full\n");
			return 0;
		}

		print_netclient(client);
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
	if (!init_netclient_list(&netclients_list, MAX_PLAYERS))
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
		printf("Player name: %s\n", player_array_get_most_recent_entry(&array)->player.name);
		sleep(1);
	}

	free_netclient_list(&netclients_list);
	close(fd);
	player_array_free(&array);

	return EXIT_SUCCESS;
}
