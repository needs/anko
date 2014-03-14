#include <stdio.h>
#include <stdlib.h>

#include <server/config.h>

int main(int argc, char** argv)
{
	if(!config_from_file("server.cfg", 1) == 2)
		return EXIT_FAILURE;
	if(!config_from_args(argc, argv))
		return EXIT_FAILURE;
	
	return EXIT_SUCCESS;
}
