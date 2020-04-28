all: clean team

team:
	gcc -Wno-switch-enum -Werror -g -fPIC src/utilsTeam.c -c -o	debug/utilsTeam.o -lcommons -lpthread
	gcc -Wall -Wextra -g -fPIC src/team.c -c -o debug/team.o -lcommons
	gcc -Wall -Wextra -g debug/utilsTeam.o debug/team.o -o bin/team -lcommons

clean:
	rm -f ./debug/*.o ./bin/team
	