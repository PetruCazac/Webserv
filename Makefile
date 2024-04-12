# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -g -Werror

# Executables
EXECUTABLES = server client showip pollserver

# Default target
all: $(EXECUTABLES)

# Individual targets
pollserver: pollserver.o
	$(CC) $(CFLAGS) -o pollserver pollserver.o
server: server.o
	$(CC) $(CFLAGS) -o server server.o

client: client.o
	$(CC) $(CFLAGS) -o client client.o

showip: showip.o
	$(CC) $(CFLAGS) -o showip showip.o

# Pattern rule for object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean object files
clean:
	rm -f *.o

# Full clean (includes executables)
fclean: clean
	rm -f $(EXECUTABLES)

# Rebuild everything
re: fclean all

.PHONY: all clean fclean re
