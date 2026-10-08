#define _POSIX_C_SOURCE 200809L

#include "network.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int create_server_socket(int port)
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;
    int enabled = 1;

    if (socket_fd < 0)
    {
        return -1;
    }

    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &enabled,
                   sizeof(enabled)) < 0)
    {
        close(socket_fd);
        return -1;
    }

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons((unsigned short)port);

    if (bind(socket_fd, (struct sockaddr *)&address, sizeof(address)) < 0 ||
        listen(socket_fd, 8) < 0)
    {
        close(socket_fd);
        return -1;
    }
    return socket_fd;
}

int connect_to_server(const char *host, int port)
{
    struct addrinfo hints;
    struct addrinfo *addresses = NULL;
    struct addrinfo *current = NULL;
    char port_text[16];
    int socket_fd = -1;

    snprintf(port_text, sizeof(port_text), "%d", port);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host, port_text, &hints, &addresses) != 0)
    {
        return -1;
    }

    for (current = addresses; current != NULL; current = current->ai_next)
    {
        socket_fd = socket(current->ai_family, current->ai_socktype,
                           current->ai_protocol);
        if (socket_fd < 0)
        {
            continue;
        }
        if (connect(socket_fd, current->ai_addr, current->ai_addrlen) == 0)
        {
            break;
        }
        close(socket_fd);
        socket_fd = -1;
    }

    freeaddrinfo(addresses);
    return socket_fd;
}

int send_all(int socket_fd, const char *text, size_t length)
{
    size_t sent = 0;

    while (sent < length)
    {
        ssize_t result = write(socket_fd, text + sent, length - sent);
        if (result < 0 && errno == EINTR)
        {
            continue;
        }
        if (result <= 0)
        {
            return -1;
        }
        sent += (size_t)result;
    }
    return 0;
}

int read_line(int socket_fd, char *buffer, size_t buffer_size)
{
    size_t index = 0;

    if (buffer_size == 0)
    {
        return -1;
    }

    while (index + 1 < buffer_size)
    {
        char character;
        ssize_t result = read(socket_fd, &character, 1);

        if (result < 0 && errno == EINTR)
        {
            continue;
        }
        if (result == 0)
        {
            if (index == 0)
            {
                return 0;
            }
            break;
        }
        if (result < 0)
        {
            return -1;
        }
        if (character == '\n')
        {
            break;
        }
        if (character != '\r')
        {
            buffer[index] = character;
            index++;
        }
    }

    buffer[index] = '\0';
    return (int)index;
}
