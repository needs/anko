#include <client/commands.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef char*(*exec_fun)(int, char**,game_t*);

typedef struct {
	char *cmd;
	exec_fun exec;
} cmd_handler;

static void free_table(char ** table);
static char** create_table(char *s, int *argc);
int wordcount(char *s);

// Command Handlers
char* cmd_regen_map(int argc, char **argv, game_t* game);

cmd_handler handlers[] =
{
	{ "regen", NULL },
	{ NULL, NULL}
};

int is_command(char *s)
{
	int i = 0;
	char ** argv = create_table(s, NULL);
	int result = 0;
	if(!argv)
		return 0;
	
	while(handlers[i].cmd && !result)
	{
		if(!strcmp(argv[0], handlers[i].cmd))
		    result = 1;
		i++;
	}
	free_table(argv);
	return result;
}

char* execute_command(char *s, game_t *game)
{
	int argc, i = 0;
	char **argv = create_table(s, &argc);
	char * result = "command not found";
	if(!argv)
		return "error executing command";
	
	while(handlers[i].cmd)
	{
		if(!strcmp(argv[0], handlers[i].cmd) && handlers[i].exec)
			result = handlers[i].exec(argc, argv, game);
		i++;
	}
	free_table(argv);
	return result;
}

static void free_table(char ** table)
{
	free(*table);
	free(table);
}

char** create_table(char *s, int *argc)
{
	int i = 0;
	char *arg;
	int wc = wordcount(s);
	char *new = strdup(s);

	if(!new)
		return NULL;
	
	char** table = malloc(sizeof(char*)*(wc+1));

	if(!table)
		return NULL;

	arg = strtok(new, " ");

	table[i] = arg;
	i++;
	while(arg != NULL)
	{
		arg = strtok(NULL, " ");

		table[i] = arg;
		i++;
	}
	
	table[i] = NULL;
	
	if(argc) *argc = wc;
	return table;
}

int wordcount(char *s)
{
	int count = 0;
	int flag = 0;
	while(*s) 
	{
		if(*s == ' ' || *s == '\t' || *s == '\n')
		{
			if(flag) 
			{ 
				count++; 
				flag = 0; 
			}
		}
		else
			flag = 1;
		s++;
	}
	if(flag) count++;
	return count;
}

char* cmd_regen_map(int argc, char **argv, game* game)
{
	(void)argc;
	(void)argv;
    regenerate_map(game);
	return "Map regenerated";
}
