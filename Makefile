CC=gcc
CCOPTS=--std=gnu99 -Wall -D_LIST_DEBUG_ 
AR=ar

OBJS=bitmap.o\
     buddyallocator.o

HEADERS=bitmap.h buddyallocator.h

LIBS=lib.a

BINS= tester

.phony: clean all


all:	$(LIBS) $(BINS)

%.o:	%.c $(HEADERS)
	$(CC) $(CCOPTS) -c -o $@  $<

lib.a: $(OBJS) 
	$(AR) -rcs $@ $^
	$(RM) $(OBJS)

tester: tester.o $(LIBS)
	$(CC) $(CCOPTS) -o $@ $^ -lm

clean:
	rm -rf *.o *~ $(LIBS) $(BINS)
