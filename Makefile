.DEFAULT_GOAL := all

# constants
IFLAGS = -I. 

CFLAGS = -g ${IFLAGS} -O0 -fPIC
LFLAGS = -lpthread

MKDIR  = mkdir -p

TARGET = sched_test


# objects
target_objs = sched_test.o \
	      metrics.o


%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@


${TARGET}: ${target_objs}
	$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)

clean:
	$(RM) ${target_objs}
	$(RM) ${TARGET}


all: ${TARGET}

