all: clean team

team:
	gcc -I"/home/utnso/tp-2020-1c-Los-Que-Aprueban/delibird" -Wno-switch-enum -Werror -g -fPIC src/utilsTeam.c -c -o	debug/utilsTeam.o -lcommons -lpthread -ldelibird
	gcc -I"/home/utnso/tp-2020-1c-Los-Que-Aprueban/delibird"-Wno-switch-enum -Werror -g -fPIC src/teamInitializer.c -c -o	debug/teamInitializer.o -lcommons -lpthread -ldelibird
	gcc -I"/home/utnso/tp-2020-1c-Los-Que-Aprueban/delibird" -Wall -g -fPIC src/team.c -c -o debug/team.o -lcommons -lpthread -ldelibird
	gcc -L"/home/utnso/tp-2020-1c-Los-Que-Aprueban/delibird/build" -Wall -g debug/utilsTeam.o debug/team.o debug/teamInitializer.o -o bin/team -lcommons -lpthread -ldelibird

clean:
	rm -f ./debug/*.o ./bin/team