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

	close(fd);

	return EXIT_SUCCESS;
}
