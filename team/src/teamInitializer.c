/*
 * teamInitializer.c
 *
 *  Created on: 30 abr. 2020
 *      Author: utnso
 */

//#include "teamInitializer.h"
#include "utilsTeam.h"
#include "entrenador.h"


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
			objetivo->posicion = malloc(sizeof(t_pokemon_entrenador));
			objetivo->posicion->pos_x = 0;
			objetivo->posicion->pos_y = 0;

			t_pokemon_entrenador* pokemonEncontrado = list_buscar(objetivosUnEntrenador, pokemonesObjetivo[j]);

			if(pokemonEncontrado != NULL){
				printf("Encontro el pokemon %s \n", pokemonesObjetivo[j]);
				objetivo = pokemonEncontrado;
				objetivo->cantidad++;
			}
			else{
				printf("Hay un nuevo pokemon que es %s \n", pokemonesObjetivo[j]);
				objetivo->pokemon = malloc(strlen(pokemonesObjetivo[j]) +1);
				memcpy(objetivo->pokemon, pokemonesObjetivo[j], strlen(pokemonesObjetivo[j]) + 1);
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
	pokemonesAtrapadosGlobal = list_create();

	t_list* objetivosEntrenadores = extraer_pokemones_entrenadores("OBJETIVOS_ENTRENADORES");
	printf("-------YA TERMNINO DE EXTRAER LOS OBJETIVOS------\n");
	t_list* pokemonesObtenidos = extraer_pokemones_entrenadores("POKEMON_ENTRENADORES");
	printf("-------YA TERMNINO DE EXTRAER LOS OBTENIDOS------\n");
	t_list* posiciones = extraer_posiciones_entrenadores();
	printf("-------YA TERMNINO DE EXTRAER LAS POSICIONES DE LOS ENTRENADORES------\n");
	cargar_objetivo_global(objetivosEntrenadores);
	printf("-------YA TERMNINO DE CARGAR EL OBJETIVO GLOBAL------\n");
	cargar_obtenidos_global(pokemonesObtenidos);
	printf("-------YA TERMNINO DE CARGAR EL OBTENIDOS GLOBAL------\n");

	for(int i=0; list_get(posiciones, i) != NULL; i++){
		t_entrenador* unEntrenador = malloc(sizeof(t_entrenador));
		unEntrenador->posicion = (t_posicion_entrenador*)list_get(posiciones, i);
		unEntrenador->objetivoEntrenador = (t_list*)list_get(objetivosEntrenadores, i);
		unEntrenador->pokemonesObtenidos = (t_list*)list_get(pokemonesObtenidos, i);
		unEntrenador->id = i;
		unEntrenador->ciclosCPU = 0;
		unEntrenador->estado_entrenador = MOVERSE_A_POKEMON;
		sem_init(&(unEntrenador->mutex_entrenador), 0, 1);
		sem_init(&(unEntrenador->sem_entrenador), 0, 0);
		pthread_create(&(unEntrenador->hilo_entrenador), NULL, (void*) comportamiento_entrenador, unEntrenador);
		pthread_detach(unEntrenador->hilo_entrenador);
		queue_push(colaNewEntrenadores, unEntrenador);
	}
	//Al finalizar el programa vamos a tener que destruir la lista de entrenadores, lo cual implicará destruir
	//también cada una de las listas que creamos acá para llenar a cada uno
	cantidadDeEntrenadores = queue_size(colaNewEntrenadores);

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
				t_pokemon_entrenador *pokemonNuevo = malloc(sizeof(t_pokemon_entrenador));
				char* nombrePokemonNuevo = ((t_pokemon_entrenador*)list_get(objetivosUnEntrenador, j))->pokemon;
				pokemonNuevo->pokemon = malloc(strlen(nombrePokemonNuevo)+1);
				memcpy(pokemonNuevo->pokemon, nombrePokemonNuevo, strlen(nombrePokemonNuevo)+1);
				pokemonNuevo -> cantidad = ((t_pokemon_entrenador*)list_get(objetivosUnEntrenador, j))->cantidad;
				list_add(objetivoGlobalEntrenadores, pokemonNuevo);
				}

		}
	}
}

void cargar_obtenidos_global(t_list* pokemonesObtenidos){

	for(int i=0; list_get(pokemonesObtenidos, i) != NULL; i++){
			t_list* obtenidosUnEntrenador = (t_list*)list_get(pokemonesObtenidos, i);


			for(int j=0; list_get(obtenidosUnEntrenador, j) != NULL; j++){
				t_pokemon_entrenador *pokemonNuevo = malloc(sizeof(t_pokemon_entrenador));
				t_pokemon_entrenador* pokemonEncontrado = list_buscar(pokemonesAtrapadosGlobal, ((t_pokemon_entrenador*)list_get(obtenidosUnEntrenador, j))->pokemon);
				if(pokemonEncontrado != NULL){
					pokemonEncontrado -> cantidad+=((t_pokemon_entrenador*)list_get(obtenidosUnEntrenador, j))->cantidad;
				}
				else{
					t_pokemon_entrenador *pokemonNuevo = malloc(sizeof(t_pokemon_entrenador));
					char* nombrePokemonNuevo = ((t_pokemon_entrenador*)list_get(obtenidosUnEntrenador, j))->pokemon;
					pokemonNuevo->pokemon = malloc(strlen(nombrePokemonNuevo)+1);
					memcpy(pokemonNuevo->pokemon, nombrePokemonNuevo, strlen(nombrePokemonNuevo)+1);
					pokemonNuevo -> cantidad = ((t_pokemon_entrenador*)list_get(obtenidosUnEntrenador, j))->cantidad;
					list_add(pokemonesAtrapadosGlobal, pokemonNuevo);
					}

			}
		}
}

//------------FUNCION QUE BUSCA UN ELEMENTO DENTRO DE UNA LISTA Y SI LO ENCUENTRA, LO RETORNA, SI NO, NULL

t_pokemon_entrenador* list_buscar(t_list* lista, char* elementoAbuscar){

	for(int i = 0; list_get(lista, i) != NULL; i++){

		//printf("El valor de la lista procesados es %s \n", ((t_pokemon_entrenador*)list_get(lista, i))->pokemon);
		//printf("El elemento a comparar es %s \n", elementoAbuscar);
		t_pokemon_entrenador *pokAux = list_get(lista, i);

		if(strcmp(pokAux->pokemon, elementoAbuscar) == 0){

			//printf("Entro al if \n");
			return list_get(lista, i);

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

	int contador = 0;
	t_pokemon_entrenador* pokemon;
	    while (list_get(lista, contador) != NULL) {

	    	pokemon= (t_pokemon_entrenador*) list_get(lista,contador);
	    	free(pokemon->posicion);
	        free(pokemon);
	        contador++;
	    }

	    free(lista);
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


void iniciar_variables_globales(){

	ciclosCPU = 0;

	cantidadCambiosDeContexto = 0;

	finalizarProceso = 0;

	colaReadyEntrenadores = queue_create();

	colaBlockedEntrenadores = queue_create();

	colaExitEntrenadores =queue_create();

	pokemonesLibresEnElMapa = list_create();

	pokemonesReservadosEnElMapa = list_create();

	pokemonesAtrapadosGlobal = list_create();

	idCorrelativosCatch = list_create();

	idCorrelativosGet = list_create();

	pokemonesLlegadosDelBroker = list_create();



	//--------------SEMAFOROS LISTAS DE POKEMONES------------------------------

	sem_init(&sem_pokemonesGlobalesAtrapados,0,1);

	sem_init(&sem_pokemonesReservados,0,1);

	sem_init(&sem_pokemonesLibresEnElMapa,0,1);

	sem_init(&sem_pokemonesObjetivoGlobal,0,1);



	//---------------SEMAFOROS COLAS DE ENTRENADORES---------------------------

	sem_init(&sem_cola_blocked,0,1);

	sem_init(&sem_cola_new,0,1);

	sem_init(&sem_cola_ready,0,1);

	sem_init(&sem_cola_exit,0,1);


	//--------------SEMAFOROS PLANIFICADORES-----------------------------------

	sem_init(&sem_planificador_cplazoReady,0,0);

	sem_init(&sem_planificador_cplazoEntrenador,0,0);

	sem_init(&sem_planificador_mplazo,0,1);

	sem_init(&sem_hay_pokemones_mapa,1,0);

	sem_init(&sem_activacionPlanificadorMPlazo,0,0);

	sem_init(&sem_terminar_todo,0,0);

	///---MUTEX UTILS--------


	//sem_init(&mutex_listaPokemonesLlegadosDelBroker,0,1);

	pthread_mutex_init(&mutex_listaPokemonesLlegadosDelBroker, NULL);

	printf("INICIALIZO SEMAFORO");

	sem_init(&mutex_idCorrelativosGet,0,1);

	sem_init(&mutex_ciclosCPU,0,1);

	sem_init(&mutex_idCorrelativos,0,1);

	sem_init(&mutex_entrenador,0,1);



}
