#ifndef SHARED_NETCLIENT_H
#define SHARED_NETCLIENT_H

#include <sys/socket.h>

/*
 * Define and provide helper function to manager network client.
 * Also provide some facitlities to handle a list of network clients.
 *
 * A network client is also called netclient.
 */

struct netclient {
	unsigned char connected;
	struct sockaddr_storage addr;
};

struct netclient_list {
	unsigned len;
	struct netclient *clients;
};

/* Initilialize a network client list of length `len`. */
int init_netclient_list(struct netclient_list *list, unsigned len);

/* Free memory used by a network client list. */
void free_netclient_list(struct netclient_list *list);

/* Searsh for a network client with the given addresse, NULL of any. */
struct netclient *get_netclient(struct netclient_list *list, struct sockaddr_storage *addr);

/* Add a new network client to the given list and return it.  NULL if list is full. */
struct netclient *add_netclient(struct netclient_list *list, struct sockaddr_storage *addr);

/* Remove a client from the list, `client` must be in the list. */
void del_netclient(struct netclient_list *list, struct netclient *client);

#endif /* SHARED_NETCLIENT_H */
