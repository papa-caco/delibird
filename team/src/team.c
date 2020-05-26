/*
 * servidor.c
 *
 *  Created on: 3 mar. 2019
 *      Author: utnso
 */

#include "team.h"

int main(void) {
	//iniciar_team();

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

	//log_destroy(g_logger);
	//LAS VARIABLES "objetivoGlobal" y "new" TIENEN QUE SER VARIABLES GLOBALES, es decir que vamos a tener que sacar
	//su hardcodeo de las funciones "iniciar_entrenadores_and_objetivoGLobal" y "cargar_objetivoGLobal"

	return EXIT_SUCCESS;
}
