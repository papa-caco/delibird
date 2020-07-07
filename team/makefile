all: clean team

team:
	gcc -I"/home/utnso/tp-2020-1c-Los-Que-Aprueban/delibird" -Wno-switch-enum -Werror -g -fPIC src/utilsTeam.c -c -o debug/utilsTeam.o -lcommons -lpthread -ldelibird
	gcc -I"/home/utnso/tp-2020-1c-Los-Que-Aprueban/delibird" -Wno-switch-enum -Werror -g -fPIC src/teamInitializer.c -c -o	debug/teamInitializer.o -lcommons -lpthread -ldelibird
	gcc -I"/home/utnso/tp-2020-1c-Los-Que-Aprueban/delibird" -Wno-switch-enum -Werror -g -fPIC src/team.c -c -o debug/team.o -lcommons -lpthread -ldelibird
	gcc -I"/home/utnso/tp-2020-1c-Los-Que-Aprueban/delibird" -Wno-switch-enum -Werror -g -fPIC src/entrenador.c -c -o debug/entrenador.o -lcommons -lpthread -ldelibird
	gcc -I"/home/utnso/tp-2020-1c-Los-Que-Aprueban/delibird" -Wno-switch-enum -Werror -g -fPIC src/decisionesDeRecepcion.c -c -o debug/decisionesDeRecepcion.o -lcommons -lpthread -ldelibird
	gcc -I"/home/utnso/tp-2020-1c-Los-Que-Aprueban/delibird" -Wno-switch-enum -Werror -g -fPIC src/planificadorACortoPlazo.c -c -o debug/planificadorACortoPlazo.o -lcommons -lpthread -ldelibird
	gcc -I"/home/utnso/tp-2020-1c-Los-Que-Aprueban/delibird" -Wno-switch-enum -Werror -g -fPIC src/planificadorAMedianoPlazo.c -c -o debug/planificadorAMedianoPlazo.o -lcommons -lpthread -ldelibird
	gcc -L"/home/utnso/tp-2020-1c-Los-Que-Aprueban/delibird/build" -Wall -Werror -g debug/team.o debug/utilsTeam.o debug/teamInitializer.o debug/entrenador.o debug/decisionesDeRecepcion.o debug/planificadorACortoPlazo.o debug/planificadorAMedianoPlazo.o -o bin/team -lcommons -lpthread -ldelibird

clean:
	rm -f ./debug/*.o ./bin/team