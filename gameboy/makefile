all: clean gameboy

gameboy:
	gcc -I"/home/utnso/tp-2020-1c-Los-Que-Aprueban/delibird" -Wno-switch-enum -Werror -g -fPIC src/gameboy_utils.c -c -o debug/gameboy_utils.o -lcommons -lpthread -ldelibird
	gcc -I"/home/utnso/tp-2020-1c-Los-Que-Aprueban/delibird" -Wall -Wextra -g -fPIC src/gameboy.c -c -o debug/gameboy.o -lcommons -lpthread
	gcc -L"/home/utnso/tp-2020-1c-Los-Que-Aprueban/delibird/build" -Wall -Wextra -g debug/gameboy_utils.o debug/gameboy.o -o bin/gameboy -lcommons -lpthread -ldelibird

clean:
	rm -f ./debug/*.o ./bin/gameboy
