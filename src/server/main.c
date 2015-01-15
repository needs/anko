#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <shared/network.h>

#define BUFSIZE 128

static int poll_network(int fd)
{
	static char buf[BUFSIZE];
	struct sockaddr_storage addr;
	socklen_t addrlen = sizeof(addr);
	ssize_t ret;

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
