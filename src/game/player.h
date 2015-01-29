#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#define MAX_PLAYERS 16

enum player_team {
	PLAYER_TEAM_SPECTATOR = 0,
	PLAYER_TEAM_BURNER,
	PLAYER_TEAM_ARBRIST,
	LAST_PLAYER_TEAM,
};

enum {
	PLAYER_FLAG_ONLINE = (1 << 0),
	LAST_PLAYER_FLAG = (2 << 0)
}

#define MAX_PLAYER_NAME_LENGTH 16

struct player {
	unsigned flags;
	char name[MAX_PLAYER_NAME_LENGTH];
	enum player_team team;
	float x, y;
};

enum {
	PLAYER_DIFF_FIELD_FLAG = (1 << 0),
	PLAYER_DIFF_FIELD_NAME = (1 << 1),
	PLAYER_DIFF_FIELD_TEAM = (1 << 2),
	PLAYER_DIFF_FIELD_X = (1 << 3),
	PLAYER_DIFF_FIELD_Y = (1 << 4)
};

struct player_diff {
	uint8_t id;
	uint8_t fields;

	uint8_t flag;
	uint8_t name[MAX_PLAYER_NAME_LENGTH];
	uint8_t team;
	uint32_t x, y;
};

struct player_diff_list {
	uint8_t len;
	struct player_diff diffs[MAX_PLAYERS];
};

static const struct player PLAYER_LIST_ZERO;

void apply_player_diff_list(struct player_diff_list *list, struct player *players);
char *unpack_player_diff_list(char *buf, struct player_diff_list *list);
char *pack_player_diff_list(char *buf, struct player_diff_list *list);
void diff_player_list(struct player *from, struct player *to, struct player_list_diff *list);

#endif
