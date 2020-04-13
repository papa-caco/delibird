all: clean gameboy

gameboy:
	gcc -Wall -Wextra -g -fPIC src/utils.c -c -o	debug/utils.o -lcommons
	gcc -Wall -Wextra -g -fPIC src/gameboy.c -c -o debug/gameboy.o -lcommons
	gcc -Wall -Wextra -g debug/utils.o debug/gameboy.o -o bin/gameboy -lcommons

clean:
	rm -f ./debug/*.o ./bin/gameboy
