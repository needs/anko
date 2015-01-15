#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <shared/network.h>

#define BUFSIZE 128

static int push_network(int fd, char *buf, size_t len)
{
	ssize_t ret;

	ret = send(fd, buf, len, 0);
	if (ret == -1) {
		fprintf(stderr, "send(%s): %s\n", buf, strerror(errno));
		return 0;
	}

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
		char buf[BUFSIZE];
		fgets(buf, BUFSIZE, stdin);
		push_network(fd, buf, strlen(buf));
	}

	close(fd);

	return EXIT_SUCCESS;
}
