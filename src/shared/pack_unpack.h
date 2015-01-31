#ifndef SHARED_NETWORK_H
#define SHARED_NETWORK_H

#include <string.h>
#include <arpa/inet.h>

/*
 * Inline the packing/unpacking functions because they are easily optimizable for
 * the compiler.
 */

static inline void pack_uint8(char **buf, uint8_t val)
{
	**buf = val;
	*buf = *buf + sizeof(val);
}

static inline void pack_uint16(char **buf, uint16_t val)
{
	val = htons(val);
	memcpy(*buf, &val, sizeof(val));
	*buf = *buf + sizeof(val);
}

static inline void pack_uint32(char **buf, uint32_t val)
{
	val = htonl(val);
	memcpy(*buf, &val, sizeof(val));
	*buf = *buf + sizeof(val);
}

static inline void pack_string(char **buf, char *str, size_t len)
{
	strncpy(*buf, str, len);
	*buf = *buf + len;
}

static inline uint8_t unpack_uint8(char **buf)
{
	uint8_t val;
	val = **buf;
	*buf = *buf + sizeof(val);
	return val;
}

static inline uint16_t unpack_uint16(char **buf)
{
	uint16_t val;
	memcpy(&val, *buf, sizeof(val));
	*buf = *buf + sizeof(val);
	val = ntohs(val);
	return val;
}

static inline uint32_t unpack_uint32(char **buf)
{
	uint32_t val;
	memcpy(&val, *buf, sizeof(val));
	*buf = *buf + sizeof(val);
	val = ntohl(val);
	return val;
}

static inline void unpack_string(char **buf, char *str, size_t len)
{
	strncpy(str, *buf, len);
	*buf = *buf + len;
}

#endif
