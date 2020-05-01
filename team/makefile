all: clean team

team:
	gcc -Wno-switch-enum -Werror -g -fPIC src/utilsTeam.c -c -o	debug/utilsTeam.o -lcommons -lpthread
	gcc -Wno-switch-enum -Werror -g -fPIC src/teamInitializer.c -c -o	debug/teamInitializer.o -lcommons -lpthread
	gcc -Wall -g -fPIC src/team.c -c -o debug/team.o -lcommons -lpthread
	gcc -Wall -g debug/utilsTeam.o debug/team.o -o bin/team -lcommons -lpthread

clean:
	rm -f ./debug/*.o ./bin/team