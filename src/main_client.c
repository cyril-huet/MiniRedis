#include "client.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_help(void)
{
    printf("usage: miniredis-cli [-h host] [-p port]\n");
}

static int valid_port(int port)
{
    if (port < 1 || port > 65535)
    {
        return 0;
    }
    return 1;
}

int main(int argc, char **argv)
{
    const char *host = "127.0.0.1";
    int port = 6379;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--help") == 0)
        {
            print_help();
            return 0;
        }
        if (strcmp(argv[i], "-h") == 0 && i + 1 < argc)
        {
            i++;
            host = argv[i];
        }
        else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc)
        {
            i++;
            port = atoi(argv[i]);
        }
        else
        {
            print_help();
            return 2;
        }
    }

    if (valid_port(port) == 0)
    {
        fprintf(stderr, "miniredis-cli: invalid port\n");
        return 2;
    }
    return client_run(host, port);
}
