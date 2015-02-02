#include <assert.h>

#include <shared/pack_unpack.h>
#include "player.h"

static void discard_invalid_fields(struct player_diff *diff)
{
	if (diff->fields & PLAYER_DIFF_FIELD_FLAGS) {
		if (diff->flags >= LAST_PLAYER_FLAG)
			diff->fields &= ~PLAYER_DIFF_FIELD_FLAGS;
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

void apply_player_diff(struct player_diff *diff, struct player *player)
{
	discard_invalid_fields(diff);
	if (diff->fields & PLAYER_DIFF_FIELD_FLAGS)
		player->flags = diff->flags;
	if (diff->fields & PLAYER_DIFF_FIELD_NAME)
		memcpy(player->name, diff->name, MAX_PLAYER_NAME_LENGTH);
	if (diff->fields & PLAYER_DIFF_FIELD_TEAM)
		player->team = diff->team;
	if (diff->fields & PLAYER_DIFF_FIELD_X)
		player->x = diff->x;
	if (diff->fields & PLAYER_DIFF_FIELD_Y)
		player->y = diff->y;
}

void apply_player_diff_list(struct player_diff_list *list, struct player *players)
{
	unsigned i;

	assert(list != NULL);
	assert(players != NULL);

	for (i = 0; i < list->len; i++)
		apply_player_diff(&list->diffs[i], &players[list->diffs[i].id]);
}

char *unpack_player_diff(char *buf, struct player_diff *diff)
{
	diff->id = unpack_uint8(&buf);
	diff->fields = unpack_uint8(&buf);

	if (diff->fields & PLAYER_DIFF_FIELD_FLAGS)
		diff->flags = unpack_uint8(&buf);
	if (diff->fields & PLAYER_DIFF_FIELD_NAME)
		unpack_string(&buf, (char*)diff->name, MAX_PLAYER_NAME_LENGTH);
	if (diff->fields & PLAYER_DIFF_FIELD_TEAM)
		diff->team = unpack_uint8(&buf);
	if (diff->fields & PLAYER_DIFF_FIELD_X)
		diff->x = unpack_uint32(&buf);
	if (diff->fields & PLAYER_DIFF_FIELD_Y)
		diff->y = unpack_uint32(&buf);
	return buf;
}

char *unpack_player_diff_list(char *buf, struct player_diff_list *list)
{
	unsigned i;

	list->len = unpack_uint8(&buf);
	if (list->len > MAX_PLAYERS)
		return NULL;

	for (i = 0; i < list->len; i++)
		buf = unpack_player_diff(buf, &list->diffs[i]);

	return buf;
}

char *pack_player_diff(char *buf, struct player_diff *diff)
{
	pack_uint8(&buf, diff->id);
	pack_uint8(&buf, diff->fields);

	if (diff->fields & PLAYER_DIFF_FIELD_FLAGS)
		pack_uint8(&buf, diff->flags);
	if (diff->fields & PLAYER_DIFF_FIELD_NAME)
		pack_string(&buf, (char*)diff->name, MAX_PLAYER_NAME_LENGTH);
	if (diff->fields & PLAYER_DIFF_FIELD_TEAM)
		pack_uint8(&buf, diff->team);
	if (diff->fields & PLAYER_DIFF_FIELD_X)
		pack_uint32(&buf, diff->x);
	if (diff->fields & PLAYER_DIFF_FIELD_Y)
		pack_uint32(&buf, diff->y);
	return buf;
}

char *pack_player_diff_list(char *buf, struct player_diff_list *list)
{
	unsigned i;

	pack_uint8(&buf, list->len);
	for (i = 0; i < list->len; i++)
		buf = pack_player_diff(buf, &list->diffs[i]);
	return buf;
}

void diff_player(struct player *from, struct player *to, struct player_diff *diff)
{
	diff->fields = 0;

	if (from->flags != to->flags) {
		diff->flags = to->flags;
		diff->fields |= PLAYER_DIFF_FIELD_FLAGS;
	}

	if (strcmp(from->name, to->name)) {
		strncpy((char*)diff->name, to->name, MAX_PLAYER_NAME_LENGTH);
		diff->fields |= PLAYER_DIFF_FIELD_NAME;
	}

	if (from->team != to->team) {
		diff->team = to->team;
		diff->fields |= PLAYER_DIFF_FIELD_TEAM;
	}

	if (from->x != to->x) {
		diff->x = to->x;
		diff->fields |= PLAYER_DIFF_FIELD_X;
	}

	if (from->y != to->y) {
		diff->y = to->y;
		diff->fields |= PLAYER_DIFF_FIELD_Y;
	}
}

void diff_player_list(struct player *from, struct player *to, struct player_diff_list *list)
{
	unsigned i;

	list->len = 0;
	for (i = 0; i < MAX_PLAYERS; i++) {
		struct player_diff diff;

		diff_player(&from[i], &to[i], &diff);

		if (diff.fields) {
			diff.id = i;
			list->diffs[list->len] = diff;
			list->len++;
		}
	}
}
