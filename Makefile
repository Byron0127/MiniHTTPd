CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -g

SRCS = src/main.c src/server.c src/http.c src/mime.c src/files.c
OBJS = $(SRCS:.c=.o)
EXEC = minihttpd

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)
