all: clean gameCard

gameCard:
	gcc -I"../delibird" -Wno-switch-enum -Werror -g -fPIC src/utils_gc.c -c -o	debug/utils_gc.o -lcommons -lpthread -ldelibird
	
	gcc -I"../delibird" -Wno-switch-enum -Werror -g -fPIC src/suscripcion.c -c -o	debug/suscripcion.o -lcommons -lpthread -ldelibird
	gcc -I"../delibird" -Wno-switch-enum -Werror -g -fPIC src/tall_grass.c -c -o  debug/tall_grass.o -lcommons -lpthread -ldelibird

	gcc -I"../delibird" -Wall -Wextra -g -fPIC src/gameCard.c -c -o debug/gameCard.o -lcommons -lpthread -ldelibird
	gcc -L"../delibird/build" -Wall -Wextra -g debug/tall_grass.o debug/suscripcion.o debug/utils_gc.o  debug/gameCard.o -o bin/gameCard -lcommons -lpthread -ldelibird
	
clean:
	rm -f ./debug/*.o ./bin/gameCard
