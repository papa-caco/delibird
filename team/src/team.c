/*
 * servidor.c
 *
 *  Created on: 3 mar. 2019
 *      Author: utnso
 */

#include "team.h"

int main(void) {

	//Semaforo y colas inicializadas
		sem_init(&(sem_cola_blocked), 0, 1);

		colaBlockedEntrenadores = queue_create();

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
		pokemonRandom2->cantidad = 2;
		pokemonRandom2->pokemon = "Messi";

		t_pokemon_entrenador* pokemonObtenido3 = malloc(
				sizeof(t_pokemon_entrenador));
		pokemonObtenido3->cantidad = 1;
		pokemonObtenido3->pokemon = "Bulbasaur";

		//Ahora creo los entrenadores con los datos necesarios para probar la función

		t_entrenador* unEntrenador = malloc(sizeof(t_entrenador));
		unEntrenador->id=1;
		unEntrenador->estado_entrenador=DEADLOCK;
		unEntrenador->posicion = malloc(sizeof(t_posicion_entrenador));
		unEntrenador->posicion->pos_x=5;
		unEntrenador->posicion->pos_y=10;
		unEntrenador->pokemonesObtenidos = list_create();
		list_add(unEntrenador->pokemonesObtenidos, pokemonObtenido3);
		list_add(unEntrenador->pokemonesObtenidos, pokemonObtenido1);
		unEntrenador->objetivoEntrenador = list_create();
		list_add(unEntrenador->objetivoEntrenador, pokemonObjetivo1);

		t_entrenador* unEntrenador2 = malloc(sizeof(t_entrenador));
		unEntrenador2->id = 2;
		unEntrenador2->estado_entrenador = DEADLOCK;
		unEntrenador2->posicion = malloc(sizeof(t_posicion_entrenador));
		unEntrenador2->posicion->pos_x = 5;
		unEntrenador2->posicion->pos_y = 10;
		unEntrenador2->pokemonesObtenidos = list_create();
		list_add(unEntrenador2->pokemonesObtenidos, pokemonObtenido2);
		list_add(unEntrenador2->pokemonesObtenidos, pokemonObtenido3);
		unEntrenador2->objetivoEntrenador = list_create();
		list_add(unEntrenador2->objetivoEntrenador, pokemonObjetivo2);

		t_entrenador* unEntrenador3 = malloc(sizeof(t_entrenador));
		unEntrenador3->pokemonesObtenidos = list_create();
		unEntrenador3->id = 3;
		unEntrenador3->estado_entrenador = DEADLOCK;
		unEntrenador3->posicion = malloc(sizeof(t_posicion_entrenador));
		unEntrenador3->posicion->pos_x = 0;
		unEntrenador3->posicion->pos_y = 2;
		list_add(unEntrenador3->pokemonesObtenidos, pokemonRandom1);
		unEntrenador3->objetivoEntrenador = list_create();
		list_add(unEntrenador3->objetivoEntrenador, pokemonRandom2);

		t_entrenador* unEntrenador4 = malloc(sizeof(t_entrenador));
		unEntrenador4->pokemonesObtenidos = list_create();
		unEntrenador4->id = 4;
		unEntrenador4->estado_entrenador = MOVERSE_A_POKEMON;
		unEntrenador4->posicion = malloc(sizeof(t_posicion_entrenador));
		unEntrenador4->posicion->pos_x = 20;
		unEntrenador4->posicion->pos_y = 4;
		list_add(unEntrenador4->pokemonesObtenidos, pokemonRandom1);
		unEntrenador4->objetivoEntrenador = list_create();
		list_add(unEntrenador4->objetivoEntrenador, pokemonRandom2);

		//queue_push(colaBlockedEntrenadores, unEntrenador);
		//Lo comento porque en realidad ese entrenador no está en blocked si no que está en exec
		queue_push(colaBlockedEntrenadores, unEntrenador4);
		queue_push(colaBlockedEntrenadores, unEntrenador3);
		queue_push(colaBlockedEntrenadores, unEntrenador2);

		printf("Los entrenadores iniciales en blocked, son: \n");

		for (int j = 0; j < queue_size(colaBlockedEntrenadores); j++) {
			t_entrenador* aux = (t_entrenador*) queue_pop(colaBlockedEntrenadores);
			int idEntrenador = aux->id;
			printf("El entrenador de id %d\n", idEntrenador);
			queue_push(colaBlockedEntrenadores, aux);
		}
		printf("----------------------------------\n");

		printf("Ejecuto la función para buscar al entrenador con el que debo intercambiar. Debería ser el de id 2\n");

		printf("----------------------------------\n");

		t_entrenador* entrenadorDelIntercambio = buscarEntrenadorDelIntercambio(unEntrenador);

		printf("El entrenador hallado es el de id: %d\n", entrenadorDelIntercambio->id);

		printf("----------------------------------\n");

		printf("Los entrenadores finales en blocked, son: \n");

		for (int j = 0; j < queue_size(colaBlockedEntrenadores); j++) {
			t_entrenador* aux = (t_entrenador*) queue_pop(colaBlockedEntrenadores);
			int idEntrenador = aux->id;
			printf("El entrenador de id %d\n", idEntrenador);
			queue_push(colaBlockedEntrenadores, aux);
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

	//return EXIT_SUCCESS;
}
