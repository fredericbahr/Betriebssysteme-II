TARGET = daemon
SRCS   = template.c daemon.c
OBJS   = template.o daemon.o

BACKUP = *.bak core *~
DEBUG  = -g -D_DEBUG

CC     = gcc
CFLAGS = -Wall -DLINUX ${DEBUG}

AR        = ar
ARFLAGS   = -r


$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LIBS)

$(OBJS): $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) $(INCL) -c $(SRCS)

clean:
	-rm -f $(OBJS) $(LIBOBJS) $(TARGET) $(LIBTARGET) $(BACKUP)
