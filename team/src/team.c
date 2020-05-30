/*
 * servidor.c
 *
 *  Created on: 3 mar. 2019
 *      Author: utnso
 */

#include "team.h"

int main(void) {
	iniciar_team();

	/*t_list * listita = extraer_posiciones_entrenadores();



	 for (int procesados = 0; procesados < 3; procesados++) {

	 t_posicion_entrenador* posicion = list_get(listita, procesados);

	 printf("-------------------El contenido de la posicion DENTRO DE LA LISTA es %d \n",
	 posicion ->pos_x);
	 printf("-------------------El contenido de la posicion DENTRO DE LA LISTA es %d \n",
	 posicion ->pos_y);

	 }

	 list_destroy(listita);
	 */
	/*t_list * objetivosEntrenadores = extraer_pokemones_entrenadores("POKEMON_ENTRENADORES");


	 for (int procesados = 0; procesados < 3; procesados++) {
	 printf("Los objetivos del entrenador numero %d son: \n", procesados);

	 t_list* objetivosUnEntrenadorPrint = list_get(objetivosEntrenadores, procesados);

	 for(int k = 0; list_get(objetivosUnEntrenadorPrint, k) != NULL; k++){
	 t_pokemon_entrenador* pokemonPrint = list_get(objetivosUnEntrenadorPrint, k);

	 printf("-------------------El pokemon es %s y su cantidad es %d \n",
	 pokemonPrint ->pokemon, pokemonPrint -> cantidad);
	 }
	 }

	 liberar_lista(objetivosEntrenadores);
	 */

	iniciar_entrenadores_and_objetivoGlobal();

	printf("----------------------------------\n");

	for (int i = 0; queue_is_empty(colaNewEntrenadores) == 0; i++) {
		t_entrenador* unEntrenador = (t_entrenador*) queue_pop(colaNewEntrenadores);
		printf("EL ENTRENADOR %d SE ENCUENTRA EN (%d, %d) \n", i + 1,
				unEntrenador->posicion->pos_x, unEntrenador->posicion->pos_y);
		printf("SUS POKEMONES OBJETIVO SON \n");
		for (int j = 0; list_get(unEntrenador->objetivoEntrenador, j) != NULL;
				j++) {
			t_pokemon_entrenador* pok = (t_pokemon_entrenador*) list_get(
					unEntrenador->objetivoEntrenador, j);
			char* nombrePok = pok->pokemon;
			printf("%s(%d)\n", nombrePok, pok->cantidad);
			for (int l = 0; l < pok->cantidad; l ++) {
				int id_recibido;
				t_msg_get_broker *msg_get = malloc(sizeof(t_msg_get_broker));
				msg_get->size_pokemon = strlen(nombrePok) + 1;
				msg_get->pokemon = malloc(sizeof(msg_get->pokemon));
				memcpy(msg_get->pokemon, nombrePok, msg_get->size_pokemon);
				id_recibido = enviar_mensaje_get(msg_get);
				//TODO Usar id_mensaje que envía BROKER en alguna función de TEAM
				eliminar_msg_get_broker(msg_get);
			}
		}
		printf("LOS POKEMONES QUE YA TIENE SON: \n");
		for (int j = 0; list_get(unEntrenador->pokemonesObtenidos, j) != NULL;
				j++) {
			t_pokemon_entrenador* pok = (t_pokemon_entrenador*) list_get(
					unEntrenador->pokemonesObtenidos, j);
			char* nombrePok = pok->pokemon;
			printf("%s(%d)\n", nombrePok, pok->cantidad);
		}
		printf("----------------------------------\n");
	}
		printf("La cantidad de objetivoGlobal es: %d \n", list_size(objetivoGlobalEntrenadores));
	for (int procesados = 0; list_get(objetivoGlobalEntrenadores, procesados) !=NULL ; procesados++) {
		printf("-----------------Los objetivos globales son: ------------------\n");

			t_pokemon_entrenador* pokemonPrint = list_get(
					objetivoGlobalEntrenadores, procesados);

			printf("-------------------El pokemon es %s y su cantidad es %d \n",
					pokemonPrint->pokemon, pokemonPrint->cantidad);

	}

	liberar_lista(objetivoGlobalEntrenadores);

	liberar_cola(colaNewEntrenadores);

	t_tipo_mensaje cola_appeared = APPEARED_POKEMON;
	t_tipo_mensaje cola_caught = CAUGHT_POKEMON;
	t_tipo_mensaje cola_localized = LOCALIZED_POKEMON;

	int status_appeared = pthread_create(&tid_appeared, NULL,(void*) inicio_suscripcion, &(cola_appeared));
	if (status_appeared != 0) {
		log_error(g_logger, "Thread create returned %d | %s", status_appeared, strerror(status_appeared));
	} else {
		pthread_detach(tid_appeared);
	}

	int status_caught = pthread_create(&tid_caught, NULL,(void*) inicio_suscripcion, &(cola_caught));
	if (status_caught != 0) {
		log_error(g_logger, "Thread create returned %d | %s", status_caught, strerror(status_caught));
	} else {
		pthread_detach(tid_caught);
	}

	int status_localized = pthread_create(&tid_localized, NULL,(void*) inicio_suscripcion, &(cola_localized));
	if (status_localized != 0) {
		log_error(g_logger, "Thread create returned %d | %s", status_localized, strerror(status_localized));
	} else {
		pthread_detach(tid_localized);
	}

	inicio_server_team();

	//log_destroy(g_logger);
	//LAS VARIABLES "objetivoGlobal" y "new" TIENEN QUE SER VARIABLES GLOBALES, es decir que vamos a tener que sacar
	//su hardcodeo de las funciones "iniciar_entrenadores_and_objetivoGLobal" y "cargar_objetivoGLobal"

	return EXIT_SUCCESS;
}
