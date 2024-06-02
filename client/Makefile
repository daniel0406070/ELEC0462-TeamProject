client: client.c client.h com.o display.o msgctr.o
	gcc -o client client.c client.h com.o display.o msgctr.o -lncurses -lpthread

com.o: com.c com.h totalset.h typectr.h
	gcc -c com.c com.h totalset.h typectr.h

display.o: display.c display.h totalset.h typectr.h
	gcc -c display.c display.h totalset.h typectr.h -lncurses

msgctr.o: msgctr.c msgctr.h totalset.h typectr.h
	gcc -c msgctr.c msgctr.h totalset.h typectr.h -c

clean:
	rm -f client com.o display.o msgctr.o display.h.gch com.h.gch msgctr.h.gch totalset.h.gch typectr.h.gch