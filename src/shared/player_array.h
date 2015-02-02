#ifndef SHARED_PLAYER_ARRAY_H
#define SHARED_PLAYER_ARRAY_H

#include <game/player.h>

#define PLAYER_ARRAY_DEFAULT_LEN 16

struct player_array_entry {
	uint16_t seq;
	unsigned char confirmed;
	struct player player;
};

struct player_array {
	unsigned len;
	struct player_array_entry *entries;
	struct player_array_entry *last, *current;
};

int player_array_init(struct player_array *array, unsigned len);
void player_array_free(struct player_array *array);
int player_array_forward(struct player_array *array);
struct player_array_entry *player_array_get_entry_by_seq(struct player_array *array, uint16_t seq);

#endif /* SHARED_PLAYER_ARRAY_H */
