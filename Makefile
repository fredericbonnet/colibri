VPATH = platform/unix

CFLAGS=-O3
#CFLAGS=-g
OBJS = 	colibri.o \
	colAlloc.o \
	colGc.o \
	colPlatform.o \
	colRope.o \
	colWord.o \
	colVector.o \
	colList.o \
	colMap.o \
	colHash.o \
	colTrie.o

libcolibri.so: $(OBJS)
	gcc -pthread -shared -o libcolibri.so $(OBJS)
	strip libcolibri.so

.c.o: colibri.h colInt.h colPlat.h
	gcc $(CFLAGS) -pthread -DBUILD_colibri -DHAVE_STDINT_H -c $<

clean:
	rm -f *.o libcolibri.so
