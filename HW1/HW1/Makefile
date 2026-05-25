CC=arm-none-linux-gnueabi-gcc -static
CFLAGS=-I. -pthread -g
LDFLAGS=-pthread -g

SRCS = main.c ipc.c devicecontrol.c
OBJS = $(SRCS:.c=.o)
DEPS = main.h ipc.h devicecontrol.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(OBJS)
	$(CC) -o homework $(OBJS) $(LDFLAGS)

clean:
	rm -f $(OBJS) homework

debug: $(OBJS)
	$(CC) -o homework $(OBJS) $(LDFLAGS)
	gdb -tui ./homework