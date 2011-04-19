VPATH = platform/unix

OBJS = 	colibri.o \
	colAlloc.o \
	colGc.o \
	colPlat.o \
	colRope.o \
	colWord.o \
	colList.o

libcolibri.so: $(OBJS)
	gcc -pthread -shared -o libcolibri.so $(OBJS)

.c.o: colibri.h colInt.h colPlat.h
	gcc -O3 -pthread -DBUILD_colibri -DHAVE_STDINT_H -c $<

clean:
	rm -f *.o libcolibri.so
