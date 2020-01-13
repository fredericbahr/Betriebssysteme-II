BACKUP = *.bak core *~
DEBUG  = -g -D_DEBUG

CC     = gcc
CFLAGS = -Wall -DLINUX ${DEBUG}

AR        = ar
ARFLAGS   = -r

qclient: mclient.o
	$(CC) -o qclient mclient.o $(LIBS)
	$(CC) $(CFLAGS) $(INCL) -c mclient.c

clean:
	-rm -f $(OBJS) $(LIBOBJS) $(TARGET) $(LIBTARGET) $(BACKUP)
