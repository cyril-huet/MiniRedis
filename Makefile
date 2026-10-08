# Compiler and flags
CC = cc
CFLAGS = -std=c99 -Wall -Wextra -Werror -pedantic -Wvla
INCLUDES = -Iinclude

# Programs
SERVER = miniredis-server
CLIENT = miniredis-cli

# Source files
COMMON_SRC = src/network.c src/protocol.c src/store.c
SERVER_SRC = src/main_server.c src/server.c $(COMMON_SRC)
CLIENT_SRC = src/main_client.c src/client.c src/network.c

SERVER_OBJ = $(SERVER_SRC:.c=.o)
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)

all: $(SERVER) $(CLIENT)

$(SERVER): $(SERVER_OBJ)
	$(CC) $(CFLAGS) $(SERVER_OBJ) -o $(SERVER)

$(CLIENT): $(CLIENT_OBJ)
	$(CC) $(CFLAGS) $(CLIENT_OBJ) -o $(CLIENT)

%.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

check: all
	sh tests/test.sh

format:
	clang-format -i $(SERVER_SRC) $(CLIENT_SRC) include/*.h

check-format:
	clang-format --dry-run -Werror $(SERVER_SRC) $(CLIENT_SRC) include/*.h

clean:
	rm -f $(SERVER_OBJ) $(CLIENT_OBJ)

fclean: clean
	rm -f $(SERVER) $(CLIENT)

re: fclean all

.PHONY: all check format check-format clean fclean re
