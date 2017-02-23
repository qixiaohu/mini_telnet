cc:=gcc
OBJECT:=main.o server.o fun.o

server:$(OBJECT)
	$(cc) $^ -o $@
server.o:server.c server.h fun.h
	$(cc) -c $<
main.o:main.c server.h fun.h
	$(cc) -c $<
fun.o:fun.c fun.h
	$(cc) -c $<
.PHONY clean:
clean:
	rm *.o
