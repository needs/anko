#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <shared/network.h>

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

	close(fd);

	return EXIT_SUCCESS;
}
