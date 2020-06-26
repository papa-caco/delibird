/*
 * servidor.c
 *
 *  Created on: 3 mar. 2019
 *      Author: utnso
 */

#include "team.h"

int main(void) {

	//Semaforo y colas inicializadas
	sem_init(&(sem_pokemonesGlobalesAtrapados), 0, 1);

	sem_init(&(sem_pokemonesLibresEnElMapa), 0, 1);

	sem_init(&(sem_pokemonesObjetivoGlobal), 0, 1);

	sem_init(&(sem_pokemonesReservados), 0, 1);

	pokemonesAtrapadosGlobal = list_create();

	objetivoGlobalEntrenadores = list_create();

	pokemonesReservadosEnElMapa = list_create();

	pokemonesLibresEnElMapa = list_create();

	//Ahora creo los pokemones que tendrán los entrenadores de la prueba

	t_pokemon_entrenador* pokemonObjetivo1 = malloc(
			sizeof(t_pokemon_entrenador));
	pokemonObjetivo1->cantidad = 2;
	pokemonObjetivo1->pokemon = "Pikachu";

	t_pokemon_entrenador* pokemonObtenido1 = malloc(
			sizeof(t_pokemon_entrenador));
	pokemonObtenido1->cantidad = 2;
	pokemonObtenido1->pokemon = "Pepito";

	t_pokemon_entrenador* pokemonObjetivo2 = malloc(
			sizeof(t_pokemon_entrenador));
	pokemonObjetivo2->cantidad = 1;
	pokemonObjetivo2->pokemon = "Pepito";

	t_pokemon_entrenador* pokemonObtenido2 = malloc(
			sizeof(t_pokemon_entrenador));
	pokemonObtenido2->cantidad = 3;
	pokemonObtenido2->pokemon = "Pikachu";

	t_pokemon_entrenador* pokemonObjetivo3 = malloc(
			sizeof(t_pokemon_entrenador));
	pokemonObjetivo3->cantidad = 2;
	pokemonObjetivo3->pokemon = "Bulbasaur";

	t_pokemon_entrenador* pokemonRandom1 = malloc(sizeof(t_pokemon_entrenador));
	pokemonRandom1->cantidad = 2;
	pokemonRandom1->pokemon = "Riquelme";

	t_pokemon_entrenador* pokemonRandom2 = malloc(sizeof(t_pokemon_entrenador));
	pokemonRandom2->cantidad = 1;
	pokemonRandom2->pokemon = "Messi";

	t_pokemon_entrenador* pokemonObtenido3 = malloc(
			sizeof(t_pokemon_entrenador));
	pokemonObtenido3->cantidad = 1;
	pokemonObtenido3->pokemon = "Bulbasaur";

	t_pokemon_entrenador_reservado* pokemonReservado1 = malloc(
			sizeof(t_pokemon_entrenador_reservado));
	pokemonReservado1->cantidad = 1;
	pokemonReservado1->pokemon = "Palermo";

	t_pokemon_entrenador_reservado* pokemonReservado2 = malloc(
			sizeof(t_pokemon_entrenador_reservado));
	pokemonReservado1->cantidad = 1;
	pokemonReservado1->pokemon = "ElGuille";

	list_add(pokemonesAtrapadosGlobal, pokemonObjetivo1);
	list_add(pokemonesAtrapadosGlobal, pokemonObjetivo2);
	list_add(pokemonesAtrapadosGlobal, pokemonObtenido2);

	list_add(pokemonesLibresEnElMapa, pokemonObjetivo3);
	list_add(pokemonesLibresEnElMapa, pokemonObtenido1);
	list_add(pokemonesLibresEnElMapa, pokemonObtenido3);

	list_add(pokemonesReservadosEnElMapa, pokemonReservado1);
	list_add(pokemonesReservadosEnElMapa, pokemonReservado2);

	list_add(objetivoGlobalEntrenadores, pokemonRandom1);
	list_add(objetivoGlobalEntrenadores, pokemonRandom2);

	printf(
			"Pregunto si me sirve agregar a Baiano, y me tiene que decir que no. \n");

	printf("---------------------------------\n");

	if (meSirvePokemon("Baiano") == 0) {
		printf("Baiano no te sirve!");
	}

	printf("---------------------------------\n");

	printf(
				"Pregunto si me sirve agregar a Riquelme, y me tiene que decir que sí porque me faltan 2. \n");

		if (meSirvePokemon("Riquelme") == 1) {
				printf("Riquelme SI te sirve!");
			}

		printf("---------------------------------\n");

	printf(
			"Pregunto si me sirve agregar a Messi, y me tiene que decir que sí porque me falta 1. \n");

	if (meSirvePokemon("Messi") == 1) {
		printf("Messi SI te sirve!");
	}

	printf("---------------------------------\n");






	/*iniciar_team();


	sem_wait(&sem_mutex_msjs);
	iniciar_entrenadores_and_objetivoGlobal();

	printf("----------------------------------\n");

	int id_recibido;


	sem_post(&sem_mutex_msjs);

	enviar_msjs_get_objetivos();

	//AL finalizar el programase deben liberar las listas de las colas de estados y la de objetivoGlobal
	//liberar_lista(objetivoGlobalEntrenadores);

	//liberar_cola(colaNewEntrenadores);

	sleep(1);

	inicio_server_team();

	//log_destroy(g_logger);
	//LAS VARIABLES "objetivoGlobal" y "new" TIENEN QUE SER VARIABLES GLOBALES, es decir que vamos a tener que sacar
	//su hardcodeo de las funciones "iniciar_entrenadores_and_objetivoGLobal" y "cargar_objetivoGLobal"

	 */

	return EXIT_SUCCESS;
}
