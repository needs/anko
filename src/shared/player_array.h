#ifndef SHARED_PLAYER_ARRAY_H
#define SHARED_PLAYER_ARRAY_H

#include <game/player.h>

#define PLAYER_ARRAY_DEFAULT_LEN 16

struct player_array_entry {
	unsigned char used, acknowledged;
	uint16_t seq;
	struct player player;
};

struct player_array {
	unsigned len;
	struct player_array_entry *entries;
};

/* Initialize the player array and preallocate it to handle number of entry
 * given in 'len'.  Add a dummy entry. */
int player_array_init(struct player_array *array, unsigned len);
void player_array_free(struct player_array *array);

/* Searsh for an entry with the given sequence number */
struct player_array_entry *player_array_get_entry_by_seq(struct player_array *array, uint16_t seq);

/* Get an unused entry */
struct player_array_entry *player_array_get_unused_entry(struct player_array *array);

/* Add a new entry, the entry must be unused, the sequence number must be unique */
void player_array_add(struct player_array *array, struct player_array_entry *entry);

/* Return the entry with the highest sequence number */
struct player_array_entry *player_array_get_most_recent_entry(struct player_array *array);

/* Acknowledge the entry and delete the entry who are less recent than the acknowledged ones */
void player_array_acknowledge_entry(struct player_array *array, struct player_array_entry *entry);

/* Searsh for an acknowledged entry, there should be always one */
struct player_array_entry *player_array_get_acknowledged_entry(struct player_array *array);

#endif /* SHARED_PLAYER_ARRAY_H */
