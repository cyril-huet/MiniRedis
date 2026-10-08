#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stddef.h>

#include "store.h"

int handle_command(struct store *store, const char *line, char *response,
                   size_t response_size, int *close_client);

#endif /* ! PROTOCOL_H */
