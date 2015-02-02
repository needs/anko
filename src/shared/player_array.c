#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "player_array.h"

int player_array_init(struct player_array *array, unsigned len)
{
	assert(array != NULL);

	array->entries = malloc(len * sizeof(*array->entries));
	if (!array->entries) {
		fprintf(stderr, "Allocating player array of length %u: %s\n",
		        len, strerror(errno));
		return 0;
	}

	array->len = len;
	array->last = &array->entries[0];
	array->current = &array->entries[0];
	array->current->player = PLAYER_ZERO;
	return 1;
}

void player_array_free(struct player_array *array)
{
	assert(array != NULL);
	free(array->entries);
}

static struct player_array_entry *get_next(struct player_array *array, struct player_array_entry *entry)
{
	if (entry == &array->entries[array->len - 1])
		return &array->entries[array->len - 1];
	else
		return entry + 1;
}

int player_array_forward(struct player_array *array)
{
	struct player_array_entry *old;

	assert(array != NULL);
	assert(array->entries != NULL);
	assert(array->current != NULL);
	assert(array->last != NULL);

	old = array->current;
	array->current = get_next(array, array->current);
	if (!array->current) {
		array->current = old;
		return 0;
	}

	array->current->seq = old->seq + 1;
	array->current->confirmed = 0;
	return 1;
}

struct player_array_entry *player_array_get_entry_by_seq(struct player_array *array, uint16_t seq)
{
	struct player_array_entry *entry = array->last;

	while (entry != array->current) {
		if (entry->seq == seq)
			return entry;
		entry = get_next(array, entry);
	}

	return entry->seq == seq ? entry : NULL;
}
