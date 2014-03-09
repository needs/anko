#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <getopt.h>

#include <client/config.h>

struct config_t config = {
	/* Default value, might be overrided */
	.screen_width  = 1024,
	.screen_height = 720,
	.board_width   = 50,
	.board_height  = 50,
	.sim_speed     = 1.0,
	.max_fps       = 1000,
	.gen_params = {
		.tree_density  = 0.7,
		.water_density = 0.2,
		.water_shatter_factor = 0.4
	}
};


static struct option long_options[] = {
	{ "help",             no_argument,       0, 'h' },
	{ "include",          required_argument, 0, 'i' },
	{ "speed",            required_argument, 0, 's' },
	{ "max-fps",          required_argument, 0, 'f' },
	{ "board-size",       required_argument, 0, 'b' },
	{ "screen-res",       required_argument, 0, 'r' },
	{ "tree-density",     required_argument, 0, 't' },
	{ "water-density",    required_argument, 0, 'w' },
	{ "water-shattering", required_argument, 0, 'k' },
	{ NULL, 0, NULL, 0 }
};


static char *desc[] = {
	"Display help",
	"Include and interpret a configuration file",
	"Simulation speed",
	"Maximum of Frames Per Second",
	"Size of the board (widthxheight)",
	"Size of window (widthxheight)",
	"Density of tree ([0;1])",
	"Density of water ([0;1])",
	"Shattering of water (]0;1]), 1 means no shattering",
};

static void usage(char *name);


int config_from_args(int argc, char **argv)
{
	/* Save context to make safe to call this when parsing options. */
	int soptind = optind, sopterr = opterr, soptopt = optopt;
	int opt;
	optind = 0;

	while ((opt = getopt_long(argc, argv, "i:f:s:b:r:t:w:k:h", long_options, NULL)) != -1) {
		switch(opt) {
		case 'i':
			if (config_from_file(optarg, 0))
				goto ret_fail;
			break;
		case 's':
			config.sim_speed = atof(optarg);
			break;
		case 'f':
			config.max_fps = atoi(optarg);
			break;
		case 'b':
			sscanf(optarg, "%dx%d", &config.board_width, &config.board_height);
			break;
		case 'r':
			sscanf(optarg, "%dx%d", &config.screen_width, &config.screen_height);
			break;
		case 't':
			config.gen_params.tree_density = atof(optarg);
			break;
		case 'w':
			config.gen_params.water_density = atof(optarg);
			break;
		case 'k':
			config.gen_params.water_shatter_factor = atof(optarg);
			break;
		case 'h':
			usage(argv[0]);
			goto ret_fail;
		default:
			goto ret_fail;
		}
	}

	if (optind < argc) {
		fprintf(stderr, "non-option elements: ");
		while (optind < argc)
			fprintf(stderr, "%s ", argv[optind++]);
		fputc('\n', stderr);
		goto ret_fail;
	}

	optind = soptind; opterr = sopterr; optopt = soptopt;
	return 0;
ret_fail:
	optind = soptind; opterr = sopterr; optopt = soptopt;
	return 1;
	
}


static void usage(char *name)
{
	struct option *opt = long_options;
	fprintf(stderr, "Usage: %s [OPTIONS]\n", name);
	fprintf(stderr, "Options:\n");
	while (opt->name != NULL) {
		fprintf(stderr, "  -%c, --%-24s %s.\n", opt->val, opt->name, desc[opt - long_options]);
		opt++;
	}
}


int config_from_file(char *path, int quiet)
{
	FILE *f;
	char opt[256], arg[256];
	int count;

	assert(path != NULL);

	if ((f = fopen(path, "r")) == NULL) {
		if (!quiet)
			perror(path);
		return 1;
	}

	while ((count = fscanf(f, "%253s %255s", opt, arg)) == 2) {
		/* Add '--' allowing, to be used by command-line parsing getopt. */
		memmove(opt + 2, opt, 254);
		opt[0] = '-'; opt[1] = '-';

		if (strcmp(opt, "help") &&
		    config_from_args(3, (char*[]){ path, opt, arg, NULL })) {
			fclose(f);
			return 2;
		}
	}

	if (count == 1) {
		fprintf(stderr, "Missing argument for option %s.\n", opt);
		fclose(f);
		return 2;
	} else if (ferror(f)) {
		if (!quiet)
			perror(path);
		fclose(f);
		return 1;
	}

	fclose(f);
	return 0;
}
