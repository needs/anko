#include <stdlib.h>
#include "display.h"
#include "generator.h"

static const int WIDTH = 20;
static const int HEIGHT = 20;

int main(void)
{
	state_t **board;

	if ((board = generate(HEIGHT, WIDTH)) == NULL)
		return EXIT_FAILURE;

	display(board, HEIGHT, WIDTH);

	return EXIT_SUCCESS;
}
