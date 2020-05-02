all: clean gameboy

gameboy:
	gcc -Wno-switch-enum -Werror -g -fPIC src/utils.c -c -o debug/utils.o -lcommons -lpthread
	gcc -Wall -Wextra -g -fPIC src/gameboy.c -c -o debug/gameboy.o -lcommons -lpthread
	gcc -Wall -Wextra -g debug/utils.o debug/gameboy.o -o bin/gameboy -lcommons -lpthread

clean:
	rm -f ./debug/*.o ./bin/gameboy
