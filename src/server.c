#include "server.h"

#include "network.h"
#include "protocol.h"

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static void serve_client(int client_socket, struct store *store)
{
    char line[1024];
    char response[1024];
    int close_client = 0;

    while (close_client == 0)
    {
        int result = read_line(client_socket, line, sizeof(line));
        if (result <= 0)
        {
            break;
        }
        close_client = 0;
        if (handle_command(store, line, response, sizeof(response),
                           &close_client) < 0)
        {
            break;
        }
        if (send_all(client_socket, response, strlen(response)) < 0)
        {
            break;
        }
    }
}

int server_run(int port)
{
    struct store store;
    int server_socket = create_server_socket(port);

    if (server_socket < 0)
    {
        perror("miniredis-server");
        return 1;
    }

    store_init(&store);
    printf("MiniRedis is listening on port %d\n", port);
    fflush(stdout);

    while (1)
    {
        int client_socket = accept(server_socket, NULL, NULL);
        if (client_socket < 0)
        {
            continue;
        }
        serve_client(client_socket, &store);
        close(client_socket);
    }
}
