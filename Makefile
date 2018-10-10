CC=gcc

CFLAGS+=-Iltm -Wall -g

OBJS=pc.o pc_test.o

#all: pc_test

#pc.o: pc.c
#	$(CC) -c $(CFLAGS) $+

#.pc_test.o: pc_test.c
#	$(CC) -c $(CFLAGS) $+

#pc_test: $(OBJS) buildtfm
#	$(CC) -o $@ $(OBJS) -Ltfm -ltfm

pc_test: $(OBJS) buildltm
	$(CC) -o $@ $(OBJS) -Lltm -lltm

buildltm:
	$(MAKE) -C ltm 2>/dev/null 

buildtfm:
	$(MAKE) -C tfm 2>/dev/null 

clean:
	-rm pc_test
	-rm *.o
	-$(MAKE) -C tfm clean
	-$(MAKE) -C ltm clean

