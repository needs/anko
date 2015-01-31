#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <shared/network.h>
#include <shared/packet.h>

static const char STRING_TO_SEND[] = "Hello world!";
static unsigned char got_ack;

static int push_network(int fd)
{
	ssize_t ret;
	static char buf[PACKET_SIZE];
	struct packet packet;

	if (got_ack)
		return 1;

	packet.ack = 30001;
	memcpy(packet.buf, STRING_TO_SEND, sizeof(STRING_TO_SEND));
	pack_packet(buf, &packet);

	ret = send(fd, buf, PACKET_SIZE, 0);
	if (ret == -1) {
		fprintf(stderr, "send(%s): %s\n", buf, strerror(errno));
		return 0;
	}

	return 1;
}

static int poll_network(int fd)
{
	ssize_t ret;
	static char buf[PACKET_SIZE];
	struct packet packet;

	ret = recvfrom(fd, buf, PACKET_SIZE, 0, NULL, NULL);
	if (ret == -1) {
		fprintf(stderr, "recvfrom(): %s\n", strerror(errno));
		return 0;
	}

	if ((unsigned)ret > PACKET_SIZE) {
		fprintf(stderr, "recvfrom(): Packet rejected: too large (%li bytes).\n",
			(long)ret);
		return 0;
	} else if ((unsigned)ret < PACKET_SIZE) {
		fprintf(stderr, "recvfrom(): Packet rejected: too small (%li bytes).\n",
			(long)ret);
		return 0;
	}

	unpack_packet(buf, &packet);
	printf("Received packet with ACK %lu\n", (unsigned long)packet.ack);
	got_ack = 1;

	return 1;
}

int main(int argc, char **argv)
{
	int fd;

	if (argc < 2) {
		fprintf(stderr, "usage: %s <host> [<port>]\n", argv[0]);
		return EXIT_FAILURE;
	}

	if ((fd = connect_to(argv[1], argv[2])) == -1)
		return EXIT_FAILURE;

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
		push_network(fd);
		poll_network(fd);
		sleep(1);
	}

	close(fd);

	return EXIT_SUCCESS;
}
