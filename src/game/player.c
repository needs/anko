#include <shared/pack_unpack.h>
#include "player.h"

static void discard_invalid_fields(struct player_diff *diff)
{
	if (diff->fields & PLAYER_DIFF_FIELD_FLAG) {
		if (diff->flags >= LAST_PLAYER_FLAG)
			diff->fields &= ~PLAYER_DIFF_FIELD_FLAG;
	}

	if (diff->fields & PLAYER_DIFF_FIELD_NAME) {
		/* TODO: Check for non-alphanumeric characters */
		if (diff->name[MAX_PLAYER_NAME_LENGTH - 1] != '\0')
			diff->fields &= ~PLAYER_DIFF_FIELD_NAME;
	}

	if (diff->fields & PLAYER_DIFF_FIELD_TEAM) {
		if (diff->team >= LAST_PLAYER_TEAM)
			diff->fields &= ~PLAYER_DIFF_FIELD_TEAM;
	}
}

void apply_player_diff_list(struct player_diff_list *list, struct player *players)
{
	unsigned i;

	assert(list != NULL);
	assert(players != NULL);

	for (i = 0; i < list->len; i++) {
		unsigned id = list->diffs[i].id;
		struct player_diff *diff = &list->diffs[i];

		discard_invalid_fields(diff);
		if (diff->fields & PLAYER_DIFF_FIELD_FLAG)
			players[id].flags = diff->flags;
		if (diff->fields & PLAYER_DIFF_FIELD_NAME)
			strcpy(players[id].name, diff->name);
		if (diff->fields & PLAYER_DIFF_FIELD_TEAM)
			players[id].team = diff->team;
		if (diff->fields & PLAYER_DIFF_FIELD_X)
			players[id].x = diff->x;
		if (diff->fields & PLAYER_DIFF_FIELD_Y)
			players[id].y = diff->y;
	}
}

char *unpack_player_diff_list(char *buf, struct player_diff_list *list)
{
	unsigned i;

	list->len = unpack_uint8(&buf);
	if (list->len > MAX_PLAYERS)
		return NULL;

	for (i = 0; i < len; i++) {
		struct player_diff *diff = &list->diffs[i];
		diff->id = unpack_uint8(&buf);
		diff->fields = unpack_uint8(&buf);

		if (diff->fields & PLAYER_DIFF_FIELD_FLAG)
			diff->flags = unpack_uint8(&buf);
		if (diff->fields & PLAYER_DIFF_FIELD_NAME)
			unpack_string(&buf, diff->name, MAX_PLAYER_NAME_LENGTH);
		if (diff->fields & PLAYER_DIFF_FIELD_TEAM)
			diff->team = unpack_uint8(&buf);
		if (diff->fields & PLAYER_DIFF_FIELD_X)
			diff->x = unpack_uint32(&buf);
		if (diff->fields & PLAYER_DIFF_FIELD_Y)
			diff->y = unpack_uint32(&buf);
	}

	return buf;
}

char *pack_player_diff_list(char *buf, struct player_diff_list *list)
{
	unsigned i;

	pack_uint8(&buf, list->len);
	for (i = 0; i < list->len; i++) {
		struct player_diff *diff = &list->diffs[i];
		pack_uint8(&buf, diff->id);
		pack_uint8(&buf, diff->fields);

		if (diff->fields & PLAYER_DIFF_FIELD_FLAG)
			pack_uint8(&buf, diff->flags);
		if (diff->fields & PLAYER_DIFF_FIELD_NAME)
			pack_string(&buf, diff->name, MAX_PLAYER_NAME_LENGTH);
		if (diff->fields & PLAYER_DIFF_FIELD_TEAM)
			pack_uint8(&buf, diff->team);
		if (diff->fields & PLAYER_DIFF_FIELD_X)
			pack_uint32(&buf, diff->x);
		if (diff->fields & PLAYER_DIFF_FIELD_Y)
			pack_uint32(&buf, y);
	}

	return buf;
}

void diff_player_list(struct player *from, struct player *to, struct player_diff_list *list)
{
	unsigned i;

	list->len = 0;
	for (i = 0; i < MAX_PLAYERS; i++) {
		struct player_diff diff;

		diff.fields = 0;

		if (from[i].flags != to[i].flags) {
			diff.flags = to[i].flags;
			diff.fields |= PLAYER_DIFF_FIELD_FLAGS;
		}

		if (strcmp(from[i].name, to[i].name)) {
			strncpy(diff.name, to[i].name, MAX_PLAYER_NAME_LENGTH);
			diff.fields |= PLAYER_DIFF_FIELD_NAME;
		}

		if (from[i].team != to[i].team) {
			diff.team = to[i].team;
			diff.fields |= PLAYER_DIFF_FIELD_TEAM;
		}

		if (from[i].x != to[i].x) {
			diff.x = to[i].x;
			diff.fields |= PLAYER_DIFF_FIELD_X;
		}

		if (from[i].y != to[i].y) {
			diff.y = to[i].y;
			diff.fields |= PLAYER_DIFF_FIELD_Y;
		}

		if (diff.fields) {
			diff.id = i;
			list->diffs[list->len] = diff;
			list->len++;
		}
	}
}
