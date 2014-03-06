#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "../generator.h"

struct config_t {
	int screen_width, screen_height;
	int board_width, board_height;
	float sim_speed;
	gen_params_t gen_params;
};

extern struct config_t config;

int config_from_args(int argc, char **argv);
int config_from_file(char *path, int quiet);

#endif /* _CONFIG_H_ */
