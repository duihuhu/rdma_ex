CC=gcc
CFLAGS=-Wall -Werror -O2
INCLUDES=
LDFLAGS=-libverbs
LIBS=-pthread -lrdmacm

SRCS=main.c ib.c sock.c server.c client.c
OBJS=$(SRCS:.c=.o)
PROG=rdma-init

all: $(PROG)

debug: CFLAGS=-Wall -Werror -g -DDEBUG
debug: $(PROG)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)

clean:
	$(RM) *.o *~ $(PROG)
