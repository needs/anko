#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "network.h"

static const struct addrinfo ADDRINFO_ZERO;

static int for_every_addr(char *node, char *service, int flags,
			  int (*callback)(struct addrinfo *addr))
{
	struct addrinfo *res = NULL, *iter, hint = ADDRINFO_ZERO;
	int ret, fd;

	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_DGRAM;
	hint.ai_flags = flags;

	if ((ret = getaddrinfo(node, service, &hint, &res)) != 0) {
		fprintf(stderr, "getaddrinfo(%s, %s): %s\n",
			node, service, gai_strerror(ret));
		return -1;
	}

	for (iter = res; iter != NULL; iter = iter->ai_next)
		if ((fd = callback(iter)) != -1)
			break;
	freeaddrinfo(res);
	if (iter == NULL)
		return -1;
	return fd;
}

static int callback_connect(struct addrinfo *addr)
{
	int fd;

	fd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
	if (fd == -1)
		return -1;
	if (connect(fd, addr->ai_addr, addr->ai_addrlen) == -1)
		return close(fd), -1;
	return fd;
}

int connect_to(char *node, char *service)
{
	int fd;

	fd = for_every_addr(node, service, 0, &callback_connect);
	if (fd == -1)
		fprintf(stderr, "Could not connect to %s, %s: %s\n",
			node, service, strerror(errno));
	return fd;
}

static int callback_bind(struct addrinfo *addr)
{
	int fd;

	fd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
	if (fd == -1)
		return -1;
	if (bind(fd, addr->ai_addr, addr->ai_addrlen) == -1)
		return close(fd), -1;
	return fd;
}

int bind_to(char *node, char *service)
{
	int fd;

	fd = for_every_addr(node, service, AI_PASSIVE, &callback_bind);
	if (fd == -1)
		fprintf(stderr, "Could not bind to %s, %s: %s\n",
			node, service, strerror(errno));
	return fd;
}
