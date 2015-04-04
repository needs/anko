#include <errno.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <netinet/in.h>
#include "netclient.h"

int init_netclient_list(struct netclient_list *list, unsigned len)
{
	assert(list != NULL);
	assert(len > 0);

	list->clients = calloc(len, sizeof(*list->clients));
	if (!list->clients) {
		fprintf(stderr, "Initializing network clients list of length %u: %s\n",
		        len, strerror(errno));
		return 0;
	}

	list->len = len;
	return 1;
}

void free_netclient_list(struct netclient_list *list)
{
	assert(list != NULL);
	free(list->clients);
}

static int addr_is_same(
	const struct sockaddr_storage *addr1, const struct sockaddr_storage *addr2)
{
	assert(addr1 != NULL);
	assert(addr2 != NULL);
	assert(addr1->ss_family == AF_INET || addr1->ss_family == AF_INET6);
	assert(addr2->ss_family == AF_INET || addr2->ss_family == AF_INET6);

	if (addr1->ss_family != addr2->ss_family)
		return 0;

	if (addr1->ss_family == AF_INET) {
		const struct sockaddr_in *in1 = (struct sockaddr_in*)addr1;
		const struct sockaddr_in *in2 = (struct sockaddr_in*)addr2;

		if (in1->sin_port != in2->sin_port)
			return 0;
		if (memcmp(&in1->sin_addr.s_addr, &in2->sin_addr.s_addr,
		           sizeof(in1->sin_addr.s_addr)))
			return 0;
	} else {
		const struct sockaddr_in6 *in1 = (struct sockaddr_in6*)addr1;
		const struct sockaddr_in6 *in2 = (struct sockaddr_in6*)addr2;

		if (in1->sin6_port != in2->sin6_port)
			return 0;
		if (memcmp(&in1->sin6_addr.s6_addr, &in2->sin6_addr.s6_addr,
		           sizeof(in2->sin6_addr.s6_addr)))
			return 0;
	}

	return 1;
}

struct netclient *get_netclient(struct netclient_list *list, struct sockaddr_storage *addr)
{
	unsigned i;

	assert(list != NULL);
	assert(addr != NULL);

	for (i = 0; i < list->len; i++)
		if (list->clients[i].connected && addr_is_same(&list->clients[i].addr, addr))
			return &list->clients[i];

	return NULL;
}


struct netclient *add_netclient(struct netclient_list *list, struct sockaddr_storage *addr)
{
	struct netclient *client = NULL;
	unsigned i;

	assert(list != NULL);
	assert(addr != NULL);

	for (i = 0; i < list->len && !client; i++)
		if (!list->clients[i].connected)
			client = &list->clients[i];
	if (!client)
		return NULL;

	client->connected = 1;
	client->addr = *addr;
	return client;
}

void del_netclient(struct netclient_list *list, struct netclient *client)
{
	assert(list != NULL);
	assert(client != NULL);
	assert(client->connected);
	/* TODO: assert client is in the list */

	client->connected = 0;
}
