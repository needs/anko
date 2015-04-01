#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "player_array.h"

/* Initialize the player array and preallocate it to handle number of entry
 * given in 'len'.  Add a dummy entry. */
int player_array_init(struct player_array *array, unsigned len)
{
	struct player_array_entry *dummy;

	assert(array != NULL);

	array->entries = calloc(len, sizeof(*array->entries));
	if (!array->entries) {
		fprintf(stderr, "Allocating player array of length %u: %s\n",
		        len, strerror(errno));
		return 0;
	}
	array->len = len;

	/*
	 * Add a dummy entry, since calloc() is used, all fields is already set
	 * to zero, so we can add it as it is.
	 */
	dummy = player_array_get_unused_entry(array);
	player_array_add(array, dummy);
	player_array_acknowledge_entry(array, dummy);

	return 1;
}

void player_array_free(struct player_array *array)
{
	assert(array != NULL);
	free(array->entries);
}

/* Searsh for an entry with the given sequence number */
struct player_array_entry *player_array_get_entry_by_seq(struct player_array *array, uint16_t seq)
{
	unsigned i;

	assert(array != NULL);

	for (i = 0; i < array->len; i++)
		if (array->entries[i].used && array->entries[i].seq == seq)
			return &array->entries[i];
	return NULL;
}

/* Get an unused entry */
struct player_array_entry *player_array_get_unused_entry(struct player_array *array)
{
	unsigned i;

	assert(array != NULL);

	for (i = 0; i < array->len; i++)
		if (!array->entries[i].used) {
			array->entries[i].acknowledged = 0;
			return &array->entries[i];
		}
	return NULL;
}

/* Add a new entry, the entry must be unused, the sequence number must be unique */
void player_array_add(struct player_array *array, struct player_array_entry *entry)
{
	assert(array != NULL);
	assert(!entry->used);
	/* TODO: assert sequence number is valid and entry is in array->entries */

	entry->used = 1;
}

/* Return the entry with the highest sequence number */
struct player_array_entry *player_array_get_most_recent_entry(struct player_array *array)
{
	struct player_array_entry *entry = NULL;
	unsigned i;

	assert(array != NULL);

	for (i = 0; i < array->len; i++)
		if (array->entries[i].used)
			if (!entry || entry->seq < array->entries[i].seq)
				entry = &array->entries[i];

	return entry;
}

/* Acknowledge the entry and delete the entry who are less recent than the acknowledged ones */
void player_array_acknowledge_entry(struct player_array *array, struct player_array_entry *entry)
{
	unsigned i;

	assert(array != NULL);
	assert(entry != NULL);

	entry->acknowledged = 1;

	for (i = 0; i < array->len; i++)
		if (array->entries[i].used && array->entries[i].seq < entry->seq)
			array->entries[i].used = 0;
}

/* Searsh for an acknowledged entry, there should be always one */
struct player_array_entry *player_array_get_acknowledged_entry(struct player_array *array)
{
	unsigned i;

	assert(array != NULL);

	for (i = 0; i < array->len; i++)
		if (array->entries[i].used && array->entries[i].acknowledged)
			break;

	assert(i < array->len);
	return &array->entries[i];
}
