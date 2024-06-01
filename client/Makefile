client_exe: typeracer.h client_exe.c communicateServer.h communicateServer.o displayScreen.h displayScreen.o
	gcc -o client_exe typeracer.h client_exe.c communicateServer.h communicateServer.o displayScreen.h displayScreen.o

communicateServer.o: communicateServer.h communicateServer.c
	gcc -o communicateServer.o communicateServer.h communicateServer.c

displayScreen.o: displayScreen.h displayScreen.c
	gcc -o displayScreen.o displayScreen.h displayScreen.c -lcurses
