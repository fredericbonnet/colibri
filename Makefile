VPATH = platform/unix

OBJS = 	colibri.o \
	colAlloc.o \
	colGc.o \
	colPlatform.o \
	colRope.o \
	colWord.o \
	colCollection.o

libcolibri.so: $(OBJS)
	gcc -pthread -shared -o libcolibri.so $(OBJS)
	strip libcolibri.so

.c.o: colibri.h colInt.h colPlat.h
	gcc -O3 -pthread -DBUILD_colibri -DHAVE_STDINT_H -c $<

clean:
	rm -f *.o libcolibri.so
