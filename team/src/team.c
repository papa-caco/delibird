/*
 * servidor.c
 *
 *  Created on: 3 mar. 2019
 *      Author: utnso
 */

#include "team.h"

int main(void) {

	leer_config_team(RUTA_CONFIG_TEAM);

	sem_init(&mutex_ciclosCPU, 0, 1);


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

		t_pokemon_entrenador* pokemonObtenido3 = malloc(
				sizeof(t_pokemon_entrenador));
		pokemonObtenido3->cantidad = 1;
		pokemonObtenido3->pokemon = "Bulbasaur";

		t_entrenador* unEntrenador = malloc(sizeof(t_entrenador));
		unEntrenador->pokemonesObtenidos = list_create();
		list_add(unEntrenador->pokemonesObtenidos, pokemonObtenido3);
		list_add(unEntrenador->pokemonesObtenidos, pokemonObtenido1);
		unEntrenador->objetivoEntrenador = list_create();
		list_add(unEntrenador->objetivoEntrenador, pokemonObjetivo1);

		t_entrenador* unEntrenador2 = malloc(sizeof(t_entrenador));
		unEntrenador2->pokemonesObtenidos = list_create();
		list_add(unEntrenador2->pokemonesObtenidos, pokemonObtenido2);
		list_add(unEntrenador2->pokemonesObtenidos, pokemonObtenido3);
		unEntrenador2->objetivoEntrenador = list_create();
		list_add(unEntrenador2->objetivoEntrenador, pokemonObjetivo2);


		printf("Los pokemones obtenidos iniciales del entrenador 1 son: \n");

		for (int j = 0; list_get(unEntrenador->pokemonesObtenidos, j) != NULL;
				j++) {
			t_pokemon_entrenador* pok = (t_pokemon_entrenador*) list_get(
					unEntrenador->pokemonesObtenidos, j);
			char* nombrePok = pok->pokemon;
			printf("%s(%d)\n", nombrePok, pok->cantidad);
		}
		printf("----------------------------------\n");

		printf("Los pokemones obtenidos iniciales del entrenador 2 son: \n");

		for (int j = 0; list_get(unEntrenador2->pokemonesObtenidos, j) != NULL;
				j++) {
			t_pokemon_entrenador* pok = (t_pokemon_entrenador*) list_get(
					unEntrenador2->pokemonesObtenidos, j);
			char* nombrePok = pok->pokemon;
			printf("%s(%d)\n", nombrePok, pok->cantidad);
		}
		printf("----------------------------------\n");




		if (puedeIntercambiarPokemon(unEntrenador, unEntrenador2)) {
			intercambiarPokemon(unEntrenador, unEntrenador2);
		}




		printf("Los pokemones obtenidos finales del entrenador 1 son: \n");

		for (int j = 0; list_get(unEntrenador->pokemonesObtenidos, j) != NULL;
				j++) {
			t_pokemon_entrenador* pok = (t_pokemon_entrenador*) list_get(
					unEntrenador->pokemonesObtenidos, j);
			char* nombrePok = pok->pokemon;
			printf("%s(%d)\n", nombrePok, pok->cantidad);
		}
		printf("----------------------------------\n");

		printf("Los pokemones obtenidos finales del entrenador 2 son: \n");

		for (int j = 0; list_get(unEntrenador2->pokemonesObtenidos, j) != NULL;
				j++) {
			t_pokemon_entrenador* pok = (t_pokemon_entrenador*) list_get(
					unEntrenador2->pokemonesObtenidos, j);
			char* nombrePok = pok->pokemon;
			printf("%s(%d)\n", nombrePok, pok->cantidad);
		}
		printf("----------------------------------\n");









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
