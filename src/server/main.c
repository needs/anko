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

#define BUFSIZE 128
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

static int poll_network(int fd)
{
	static char buf[BUFSIZE];
	struct sockaddr_storage addr;
	socklen_t addrlen = sizeof(addr);
	ssize_t ret;
	struct slot *slot;

	ret = recvfrom(fd, buf, BUFSIZE, MSG_TRUNC, (struct sockaddr*)&addr, &addrlen);
	if (ret == -1) {
		fprintf(stderr, "recvfrom(): %s\n", strerror(errno));
		return 0;
	}

	if (ret > BUFSIZE) {
		fprintf(stderr, "recvfrom(): Message truncated, more than %li bytes received.\n",
			(long)ret);
		return 0;
	}

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

	printf("Received packet of size %li\n", (long)ret);
	return 1;
}

int main(int argc, char **argv)
{
	int fd;

	if (argc < 2) {
		fprintf(stderr, "usage: %s <host> [<port>]\n", argv[0]);
		return EXIT_FAILURE;
	}

	if ((fd = bind_to(argv[1], argv[2])) == -1)
		return EXIT_FAILURE;

	/*
	 * The main loop for the server should be:
	 *   1) poll_network(network, diff)
	 *   2) logic(from, event, to)
	 *   3) push_network(network, to)
	 */

	while (1) {
		poll_network(fd);
	}

	close(fd);

	return EXIT_SUCCESS;
}
