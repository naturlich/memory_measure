.PHONY: all clean

CC = gcc
CFLAGS = -Wall -O2 -g -std=gnu99

PROG=memory-measure
SRCS+=main.c
OBJS=$(patsubst %.c,%.o,$(SRCS))

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

clean: clean-$(PROG)
clean-$(PROG):
	rm -rf $(PROG) $(OBJS)
