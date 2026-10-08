#ifndef NETWORK_H
#define NETWORK_H

#include <stddef.h>

int create_server_socket(int port);
int connect_to_server(const char *host, int port);
int send_all(int socket_fd, const char *text, size_t length);
int read_line(int socket_fd, char *buffer, size_t buffer_size);

#endif /* ! NETWORK_H */
