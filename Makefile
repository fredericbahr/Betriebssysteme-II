TARGET = smile
SRCS   = template.c init.c cleanup.c worker.c daemon/daemon.c signale/signale.c
OBJS   = template.o init.o cleanup.o worker.o daemon.o signale.o

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
