all: clean gameCard

gameCard:
#	gcc -Wall -Wextra -g -fPIC src/utilsGc.c -c -o	debug/utilsGc.o -lcommons -lpthread
	gcc -Wno-switch-enum -Werror -g -fPIC src/utilsGc.c -c -o	debug/utilsGc.o -lcommons -lpthread
#	gcc -Wall -Wextra -g -fPIC src/servidor.c -c -o debug/servidor.o -lcommons -lpthread
#	gcc -Wall -Wextra -g -fPIC src/cliente.c -c -o debug/cliente.o -lcommons -lpthread
	gcc -Wall -Wextra -g -fPIC src/gameCard.c -c -o debug/gameCard.o -lcommons -lpthread
	gcc -Wall -Wextra -g debug/utilsGc.o  debug/gameCard.o -o bin/gameCard -lcommons -lpthread
clean:
	rm -f ./debug/*.o ./bin/gameCard
