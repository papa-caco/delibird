all: clean testServer

testServer:
	gcc -Wno-switch-enum -Werror -g -fPIC src/utils.c -c -o	debug/utils.o -lcommons -lpthread
	gcc -Wall -g -fPIC src/servidor.c -c -o debug/servidor.o -lcommons -lpthread
	gcc -Wall -g debug/utils.o debug/servidor.o -o bin/testServer -lcommons -lpthread

clean:
	rm -f ./debug/*.o ./bin/testServer