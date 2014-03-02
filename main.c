#include <stdlib.h>
#include <time.h>
#include "display.h"


int main(void)
{
	srandom(time(NULL));
	display(NULL, 1, 1);
	return EXIT_SUCCESS;
}
