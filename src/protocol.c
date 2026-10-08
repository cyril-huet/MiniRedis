#include "protocol.h"

#include <stdio.h>
#include <string.h>

static char *next_word(char **text)
{
    char *start = *text;
    char *end = NULL;

    while (*start == ' ' || *start == '\t')
    {
        start++;
    }
    if (*start == '\0')
    {
        *text = start;
        return NULL;
    }

    end = start;
    while (*end != '\0' && *end != ' ' && *end != '\t')
    {
        end++;
    }
    if (*end != '\0')
    {
        *end = '\0';
        end++;
    }
    *text = end;
    return start;
}

static char *remaining_text(char **text)
{
    char *value = *text;

    while (*value == ' ' || *value == '\t')
    {
        value++;
    }
    *text = value;
    return value;
}

static void uppercase(char *text)
{
    for (int i = 0; text[i] != '\0'; i++)
    {
        if (text[i] >= 'a' && text[i] <= 'z')
        {
            text[i] = (char)(text[i] - 'a' + 'A');
        }
    }
}

static int answer(char *response, size_t size, const char *text)
{
    int result = snprintf(response, size, "%s\n", text);
    if (result < 0 || (size_t)result >= size)
    {
        return -1;
    }
    return 0;
}

int handle_command(struct store *store, const char *line, char *response,
                   size_t response_size, int *close_client)
{
    char command_line[1024];
    char *cursor = command_line;
    char *command = NULL;
    char *key = NULL;
    char *value = NULL;
    const char *stored = NULL;
    char number[64];

    if (strlen(line) >= sizeof(command_line))
    {
        return answer(response, response_size, "ERR command is too long");
    }
    strcpy(command_line, line);
    command = next_word(&cursor);
    if (command == NULL)
    {
        return answer(response, response_size, "ERR empty command");
    }
    uppercase(command);

    if (strcmp(command, "PING") == 0)
    {
        return answer(response, response_size, "PONG");
    }
    if (strcmp(command, "QUIT") == 0)
    {
        *close_client = 1;
        return answer(response, response_size, "OK");
    }

    key = next_word(&cursor);
    if (strcmp(command, "SET") == 0)
    {
        value = remaining_text(&cursor);
        if (key == NULL || value[0] == '\0')
        {
            return answer(response, response_size,
                          "ERR SET needs a key and a value");
        }
        if (store_set(store, key, value) < 0)
        {
            return answer(response, response_size, "ERR out of memory");
        }
        return answer(response, response_size, "OK");
    }

    if (key == NULL &&
        (strcmp(command, "GET") == 0 || strcmp(command, "DEL") == 0 ||
         strcmp(command, "EXISTS") == 0 || strcmp(command, "INCR") == 0))
    {
        return answer(response, response_size, "ERR key is missing");
    }
    if (strcmp(command, "GET") == 0)
    {
        stored = store_get(store, key);
        if (stored == NULL)
        {
            return answer(response, response_size, "(nil)");
        }
        return answer(response, response_size, stored);
    }
    if (strcmp(command, "DEL") == 0)
    {
        snprintf(number, sizeof(number), "%d", store_delete(store, key));
        return answer(response, response_size, number);
    }
    if (strcmp(command, "EXISTS") == 0)
    {
        snprintf(number, sizeof(number), "%d", store_exists(store, key));
        return answer(response, response_size, number);
    }
    if (strcmp(command, "INCR") == 0)
    {
        if (store_increment(store, key, number, sizeof(number)) < 0)
        {
            return answer(response, response_size,
                          "ERR value is not an integer");
        }
        return answer(response, response_size, number);
    }
    return answer(response, response_size, "ERR unknown command");
}
