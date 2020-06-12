/*
 * teamInitializer.c
 *
 *  Created on: 30 abr. 2020
 *      Author: utnso
 */

#include "teamInitializer.h"

void iniciar_team(void){
	leer_config_team(RUTA_CONFIG_TEAM);
	iniciar_log_team();
	iniciar_cnt_msjs();
	status_conn_broker = true;
	sem_init(&sem_mutex_msjs, 0, 1);
	iniciar_suscripciones_broker();
	lanzar_reconexion_broker(g_logger);
}

t_list * extraer_posiciones_entrenadores() {

	t_list *posicionesEntrenadores = list_create();

	t_config* config = config_create(RUTA_CONFIG_TEAM);

	char ** listaConfig = config_get_array_value(config, "POSICIONES_ENTRENADORES");

	for (int procesados = 0; listaConfig[procesados] != NULL; procesados++) {


		char** posiciones = string_split(listaConfig[procesados], "|");

		int posicionX = atoi(posiciones[0]);
		int posicionY = atoi(posiciones[1]);

		t_posicion_entrenador *posicion = malloc(sizeof(t_posicion_entrenador));

		posicion->pos_x = posicionX;
		posicion->pos_y = posicionY;


		list_add(posicionesEntrenadores, posicion);


	}

	config_destroy(config);

	return posicionesEntrenadores;
}

t_list *extraer_pokemones_entrenadores(char* configKey){
	//Inicializo la lista, que contendrá listas de objetivos de todos los entrenadores
	t_list* objetivosEntrenadores = list_create();


	t_config* config = config_create(RUTA_CONFIG_TEAM);

	//Inicializo el array que contiene los objetivos separados por ","
	char ** arrayConfig = config_get_array_value(config, configKey);



	for (int i = 0; arrayConfig[i] != NULL; i++){
		printf("----------CAMBIO DE ENTRENADOR ------- \n");
		//Inicializo lista que contendrá objetivos
		t_list* objetivosUnEntrenador = list_create();


		//Ahora recibo un array con todos los pokemones (tal vez se repitan) que necesita
		//el entrenador "i" pero sin el "|"
		char** pokemonesObjetivo = string_split(arrayConfig[i], "|");



		for(int j=0; pokemonesObjetivo[j] != NULL; j++){
			t_pokemon_entrenador *objetivo = malloc(sizeof(t_pokemon_entrenador));


			t_pokemon_entrenador* pokemonEncontrado = list_buscar(objetivosUnEntrenador, pokemonesObjetivo[j]);

			if(pokemonEncontrado != NULL){
				printf("Encontro el pokemon %s \n", pokemonesObjetivo[j]);
				objetivo = pokemonEncontrado;
				objetivo->cantidad++;
			}
			else{
				printf("Hay un nuevo pokemon que es %s \n", pokemonesObjetivo[j]);
				objetivo->pokemon = pokemonesObjetivo[j];
				objetivo->cantidad = 1;

				list_add(objetivosUnEntrenador, objetivo);
			}

		}

		list_add(objetivosEntrenadores, objetivosUnEntrenador);

	}

	config_destroy(config);


	return objetivosEntrenadores;
}



void iniciar_entrenadores_and_objetivoGlobal(){
	colaNewEntrenadores = queue_create();
	objetivoGlobalEntrenadores = list_create();
	t_list* objetivosEntrenadores = extraer_pokemones_entrenadores("OBJETIVOS_ENTRENADORES");
	t_list* pokemonesObtenidos = extraer_pokemones_entrenadores("POKEMON_ENTRENADORES");
	t_list* posiciones = extraer_posiciones_entrenadores();
	cargar_objetivo_global(objetivosEntrenadores);


	for(int i=0; list_get(posiciones, i) != NULL; i++){
		t_entrenador* unEntrenador = malloc(sizeof(t_entrenador));
		unEntrenador->posicion = (t_posicion_entrenador*)list_get(posiciones, i);
		unEntrenador->objetivoEntrenador = (t_list*)list_get(objetivosEntrenadores, i);
		unEntrenador->pokemonesObtenidos = (t_list*)list_get(pokemonesObtenidos, i);
		queue_push(colaNewEntrenadores, unEntrenador);
	}
	//Al finalizar el programa vamos a tener que destruir la lista de entrenadores, lo cual implicará destruir
	//también cada una de las listas que creamos acá para llenar a cada uno

}

void cargar_objetivo_global(t_list* objetivosEntrenadores){
	for(int i=0; list_get(objetivosEntrenadores, i) != NULL; i++){
		t_list* objetivosUnEntrenador = (t_list*)list_get(objetivosEntrenadores, i);


		for(int j=0; list_get(objetivosUnEntrenador, j) != NULL; j++){
			t_pokemon_entrenador *pokemonNuevo = malloc(sizeof(t_pokemon_entrenador));
			t_pokemon_entrenador* pokemonEncontrado = list_buscar(objetivoGlobalEntrenadores, ((t_pokemon_entrenador*)list_get(objetivosUnEntrenador, j))->pokemon);
			if(pokemonEncontrado != NULL){
				pokemonEncontrado -> cantidad+=((t_pokemon_entrenador*)list_get(objetivosUnEntrenador, j))->cantidad;
			}
			else{
				pokemonNuevo -> pokemon = ((t_pokemon_entrenador*)list_get(objetivosUnEntrenador, j))->pokemon;
				pokemonNuevo -> cantidad = ((t_pokemon_entrenador*)list_get(objetivosUnEntrenador, j))->cantidad;
				list_add(objetivoGlobalEntrenadores, pokemonNuevo);
				}

		}
	}
}

//------------FUNCION QUE BUSCA UN ELEMENTO DENTRO DE UNA LISTA Y SI LO ENCUENTRA, LO RETORNA, SI NO, NULL

t_pokemon_entrenador* list_buscar(t_list* lista, char* elementoAbuscar){

	for(int i = 0; list_get(lista, i) != NULL; i++){

		printf("El valor de la lista procesados es %s \n", ((t_pokemon_entrenador*)list_get(lista, i))->pokemon);
		printf("El elemento a comparar es %s \n", elementoAbuscar);
		if(strcmp(((t_pokemon_entrenador*)list_get(lista, i))->pokemon, elementoAbuscar) == 0){

			printf("Entro al if \n");
			return (t_pokemon_entrenador*)list_get(lista, i);

		}
	}
	return NULL;
}

//--------------FUNCIONES DE DESTRUCCIÓN DE ELEMENTOS DE CADA LISTA UTILIZADA--------------------
void destroy_posicion_entrenador(t_posicion_entrenador* posicion){
	free(posicion);
}

void destroy_pokemon_entrenador(t_pokemon_entrenador* pokemon){
	//free(pokemon->pokemon);
	free(pokemon);
}

void destroy_pokemonProcesado(char* pokemonProcesado){
	free(pokemonProcesado);
}

void destroy_entrenador(t_entrenador* entrenador){
	destroy_posicion_entrenador(entrenador->posicion);
	liberar_lista_de_pokemones(entrenador->pokemonesObtenidos);
	liberar_lista_de_pokemones(entrenador->objetivoEntrenador);
	free(entrenador);
}

void print_pokemones_objetivo(t_pokemon_entrenador *poke)
{
	printf("(%d|%s)\n",poke->cantidad,poke->pokemon);
}

void enviar_msjs_get_por_clase_de_pokemon(t_pokemon_entrenador *poke)
{
	for (int i = 0; i < poke->cantidad; i ++) {
		enviar_get_pokemon_broker(poke->pokemon, g_logger);
	}
}

void liberar_lista_de_pokemones(t_list* lista){
	liberar_lista(lista);
}

void liberar_lista(t_list* lista) {

    int contador = 0;
    while (list_get(lista, contador) != NULL) {
        free(list_get(lista,contador));
        contador++;
    }

    free(lista);
}

void liberar_cola(t_queue* cola) {

    while (queue_is_empty(cola)==0) {
        free(queue_pop(cola));
    }

    free(cola);
}



//Con las tres funciones anteriores ya podemos crear dos funciones: Una que defina el objetivo
//global del Team, y otra que inicialice a cada uno de los entrenadores con sus posiciones
//y sus objetivos particulares
