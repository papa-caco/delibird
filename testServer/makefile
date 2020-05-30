all: clean testServer

testServer:
	gcc -I"/home/utnso/tp-2020-1c-Los-Que-Aprueban/delibird" -Wno-switch-enum -Werror -g -fPIC src/utils.c -c -o	debug/utils.o -lcommons -lpthread
	gcc -I"/home/utnso/tp-2020-1c-Los-Que-Aprueban/delibird" -Wall -g -fPIC src/servidor.c -c -o debug/servidor.o -lcommons -lpthread
	gcc  -L"/home/utnso/tp-2020-1c-Los-Que-Aprueban/delibird/build" -Wall -g debug/utils.o debug/servidor.o -o bin/testServer -lcommons -lpthread -ldelibird

clean:
	rm -f ./debug/*.o ./bin/testServer