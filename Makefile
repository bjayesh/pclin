CC=gcc

CFLAGS+=-Itfm -Wall -g

OBJS=pc.o pc_test.o

#all: pc_test

#pc.o: pc.c
#	$(CC) -c $(CFLAGS) $+

#.pc_test.o: pc_test.c
#	$(CC) -c $(CFLAGS) $+

pc_test: $(OBJS) buildtfm
	$(CC) -o $@ $(OBJS) -Ltfm -ltfm

buildltm:
	$(MAKE) -C ltm 2>/dev/null 

buildtfm:
	$(MAKE) -C tfm 2>/dev/null 

clean:
	-rm pc_test
	-rm *.o
	-$(MAKE) -C tfm clean
	-$(MAKE) -C ltm clean

