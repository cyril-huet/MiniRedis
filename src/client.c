#include "client.h"

#include "network.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

int client_run(const char *host, int port)
{
    char command[1024];
    char response[1024];
    int socket_fd = connect_to_server(host, port);

    if (socket_fd < 0)
    {
        perror("miniredis-cli");
        return 1;
    }

    while (fgets(command, sizeof(command), stdin) != NULL)
    {
        if (send_all(socket_fd, command, strlen(command)) < 0 ||
            read_line(socket_fd, response, sizeof(response)) <= 0)
        {
            close(socket_fd);
            return 1;
        }
        printf("%s\n", response);
        if (strncmp(command, "QUIT", 4) == 0 ||
            strncmp(command, "quit", 4) == 0)
        {
            break;
        }
    }

    close(socket_fd);
    return 0;
}
