/*
 * entrenador.c
 *
 *  Created on: 4 jun. 2020
 *      Author: utnso
 */


#include "entrenador.h"

//------------------------------FUNCIONES BÁSICAS DEL ENTRENADOR-------------------------------------------


void comportamiento_entrenador(t_entrenador* entrenador){

	int distancia;
	t_pokemon_entrenador* pokemon;
	t_posicion_entrenador* posicionEntrenador;
	t_entrenador* entrenador2;

	while(true){

		sem_wait(&(entrenador->sem_entrenador));

		switch (entrenador->estado_entrenador) {
			case MOVERSE_A_POKEMON:
				pokemon = buscarPokemonMasCercano(entrenador->posicion);
				sem_wait(&(sem_listas_pokemones));
				moverPokemonAReservados(pokemonesLibresEnElMapa,pokemonesReservadosEnElMapa, pokemon, entrenador->id);
				sem_post(&(sem_listas_pokemones));
				distancia = calcularDistancia(entrenador->posicion, pokemon->posicion);

					for(int i=0; i<distancia; i++){
						moverEntrenador(entrenador, pokemon->posicion);

					}


					entrenador->estado_entrenador = ATRAPAR;

					sem_post(&(sem_planificador_cplazo));

					//// SIGNAL A PLANIFICADOR?????????

				break;
		case MOVERSE_A_ENTRENADOR:
			posicionEntrenador = buscarEntrenadorAMoverse(entrenador);

			distancia = calcularDistancia(entrenador->posicion, posicionEntrenador);

			for(int i=0; i<distancia; i++){

			moverEntrenador(entrenador, posicionEntrenador);

			}

			entrenador->estado_entrenador = INTERCAMBIAR;

			sem_post(&(sem_planificador_cplazo));


							break;
			case ATRAPAR:
				pokemon = buscarPokemonMasCercano(entrenador->posicion);
				intentarAtraparPokemon(entrenador, pokemon);

				entrenador->estado_entrenador = ESPERAR_CAUGHT;

				sem_post(&(sem_planificador_cplazo));

				break;

			case INTERCAMBIAR:

				entrenador2 = buscarEntrenadorDelIntercambio(entrenador);

				intercambiarPokemon(entrenador, entrenador2);

				entrenador->estado_entrenador = ACABO_INTERCAMBIO;

				entrenador2->estado_entrenador = ACABO_INTERCAMBIO;

				sem_post(&(sem_planificador_cplazo));

				break;
				case -1:
					pthread_exit(NULL);
				}
		//FALTA DETERMINAR EL CASO DEL FINALIZADO DEL ENTRENADOR

	}

}

t_pokemon_entrenador* buscarPokemonMasCercano(t_posicion_entrenador* posicion_Entrenador){

	t_pokemon_entrenador* pokemonMasCercano;
	int distanciaMasCercana = 100000;
	int distanciaAux = 0;



	for (int i = 0; i < list_size(pokemonesLibresEnElMapa); i++) {


		t_pokemon_entrenador* pokLibreAux = ((t_pokemon_entrenador*) list_get(pokemonesLibresEnElMapa, i));


		distanciaAux = calcularDistancia(posicion_Entrenador, pokLibreAux->posicion );

		if(distanciaAux < distanciaMasCercana){
			distanciaMasCercana = distanciaAux;
			pokemonMasCercano = pokLibreAux;
		}

	}

	return pokemonMasCercano;

}

t_entrenador* buscarEntrenadorDelIntercambio(t_entrenador* entrenador){

	t_entrenador* entrenadorMasCercano;
	t_entrenador* entrenadorAux;
	int distanciaAux = 0;

	sem_wait(&(sem_cola_blocked));

	for (int i = 0; i < queue_size(colaBlockedEntrenadores); i++) {



		entrenadorAux = (t_entrenador*) queue_pop(colaBlockedEntrenadores);


		distanciaAux = calcularDistancia(entrenador->posicion, entrenadorAux->posicion);

		if(distanciaAux == 0 && entrenadorAux->estado_entrenador == DEADLOCK
				&& puedeIntercambiarPokemon(entrenador, entrenadorAux)){

			entrenadorMasCercano = entrenadorAux;

		}


		queue_push(colaBlockedEntrenadores, entrenadorAux);

		}

	sem_post(&(sem_cola_blocked));


	return entrenadorMasCercano;

}

t_posicion_entrenador* buscarEntrenadorAMoverse(t_entrenador* entrenador){

	t_posicion_entrenador* posicionAMoverse;
	t_entrenador* entrenadorAux;

	sem_wait(&(sem_cola_blocked));

	for (int i = 0; i < queue_size(colaBlockedEntrenadores); i++) {



		entrenadorAux = (t_entrenador*) queue_pop(colaBlockedEntrenadores);


		if(entrenadorAux->estado_entrenador == DEADLOCK
				&& puedeIntercambiarPokemon(entrenador, entrenadorAux)){

			posicionAMoverse = entrenadorAux->posicion;

		}


		queue_push(colaBlockedEntrenadores, entrenadorAux);

		}

	sem_post(&(sem_cola_blocked));


	return posicionAMoverse;

}

void moverPokemonAReservados(t_list* listaQueContieneElPokemon,
		t_list* listaReceptoraDelPokemon, t_pokemon_entrenador* pokemonAMover, int idReservador){

	t_pokemon_entrenador* pokemonAux;

	t_pokemon_entrenador_reservado* pokemonAAgregar = malloc(sizeof(t_pokemon_entrenador_reservado));
	pokemonAAgregar->cantidad = 1;
	pokemonAAgregar->pokemon = pokemonAMover->pokemon;
	pokemonAAgregar->posicion = malloc(sizeof(t_posicion_entrenador));
	pokemonAAgregar->posicion->pos_x = pokemonAMover->posicion->pos_x;
	pokemonAAgregar->posicion->pos_y = pokemonAMover->posicion->pos_y;
	pokemonAAgregar->id_entrenadorReserva = idReservador;

	list_add(listaReceptoraDelPokemon, pokemonAAgregar);

	int indice;

	for(int i=0; i < list_size(listaQueContieneElPokemon); i++){

		pokemonAux = ((t_pokemon_entrenador*) list_get(listaQueContieneElPokemon, i));

		if(pokemonAux == pokemonAMover){
			indice=i;
		}
	}

	if(pokemonAMover->cantidad == 1){
		pokemonAux = list_remove(listaQueContieneElPokemon, indice);
		free(pokemonAux->posicion);
		free(pokemonAux);

	}else{

		pokemonAMover->cantidad--;
	}



}


/////MOVER ENTRENADOR///////////////////

void moverEntrenador(t_entrenador* entrenador, t_posicion_entrenador* posicionAMoverse){

	//AGREGAR SEMAFORO MUTEX DEL ENTRENADOR EN PARTICULAR  (ADENTRO O AFUERA?????)

	if(entrenador->posicion->pos_x != posicionAMoverse->pos_x){
		if(entrenador -> posicion -> pos_x > posicionAMoverse->pos_x ){
				entrenador->posicion->pos_x --;
			}else if(entrenador->posicion->pos_x < posicionAMoverse->pos_x){
				entrenador->posicion->pos_x ++;
			}
	}else {
		if(entrenador -> posicion -> pos_y > posicionAMoverse->pos_y ){
				entrenador->posicion->pos_y --;
			}else if(entrenador->posicion->pos_y < posicionAMoverse->pos_y){
				entrenador->posicion->pos_y ++;
			}
	}


	sleep(g_config_team->retardo_ciclo_cpu);
	sem_wait(&mutex_ciclosCPU);
	ciclosCPU++;
	sem_post(&mutex_ciclosCPU);


}

int calcularDistancia(t_posicion_entrenador* posicionActual, t_posicion_entrenador* posicionAMoverse){
	 int distanciaTotal = 0;
	    if (posicionActual->pos_x > posicionAMoverse->pos_x){
	        distanciaTotal += posicionActual->pos_x - posicionAMoverse->pos_x;
	    } else {
	        distanciaTotal += posicionAMoverse->pos_x -posicionActual->pos_x;
	    }

	    if(posicionActual->pos_y > posicionAMoverse->pos_y){
	        distanciaTotal += posicionActual->pos_y - posicionAMoverse->pos_y;
	    } else {
	        distanciaTotal += posicionAMoverse->pos_y - posicionActual->pos_y ;
	    }

	    return distanciaTotal;

}

//////ATRAPAR UN POKEMON//////////////////

void intentarAtraparPokemon(t_entrenador* entrenador, t_pokemon_entrenador* pokemon){

	enviar_catch_pokemon_broker(entrenador->posicion->pos_x, entrenador->posicion->pos_y, pokemon->pokemon, g_logger, entrenador->id);

}

//////INTERCAMBIAR UN POKEMON/////////////

void intercambiarPokemon(t_entrenador* entrenador1, t_entrenador* entrenador2) {

	t_list* pokemonesInnecesariosDT1 = pokemonesInnecesarios(entrenador1);
	t_list* pokemonesInnecesariosDT2 = pokemonesInnecesarios(entrenador2);
	t_list* pokemonesPendientesDT1 = pokemonesPendientes(entrenador1);
	t_list* pokemonesPendientesDT2 = pokemonesPendientes(entrenador2);

	//NO HAY QUE LIBERAR ESTOS CUATRO!
	t_pokemon_entrenador* pokemonInnecesario1 = malloc(sizeof(t_pokemon_entrenador));
	//t_pokemon_entrenador* pokemonPendiente2 = malloc(sizeof(t_pokemon_entrenador));
	t_pokemon_entrenador* pokemonInnecesario2 = malloc(sizeof(t_pokemon_entrenador));
	//t_pokemon_entrenador* pokemonPendiente1 = malloc(sizeof(t_pokemon_entrenador));

	//Evalúo que le sirva al entrenador 2
	for (int i = 0; i < list_size(pokemonesInnecesariosDT1); i++) {

		t_pokemon_entrenador* pokInnecesarioAux = ((t_pokemon_entrenador*) list_get(pokemonesInnecesariosDT1, i));
		pokemonInnecesario1->cantidad=pokInnecesarioAux->cantidad;
		pokemonInnecesario1->pokemon=pokInnecesarioAux->pokemon;


		for (int j = 0; j < list_size(pokemonesPendientesDT2); j++) {
			t_pokemon_entrenador* pokemonPendiente2 =
					((t_pokemon_entrenador*) list_get(pokemonesPendientesDT2, j));

			if (strcmp(pokemonInnecesario1->pokemon, pokemonPendiente2->pokemon) == 0) {

				agregarPokemon(entrenador2, pokemonInnecesario1);
				quitarPokemon(entrenador1, pokemonInnecesario1);

			}

		}
	}

	//Evalúo que le sirva al entrenador 1
	for (int i = 0; i < list_size(pokemonesInnecesariosDT2); i++) {

		t_pokemon_entrenador* pokInnecesarioAux = ((t_pokemon_entrenador*) list_get(pokemonesInnecesariosDT2, i));
		pokemonInnecesario2->cantidad=pokInnecesarioAux->cantidad;
		pokemonInnecesario2->pokemon=pokInnecesarioAux->pokemon;


		for (int j = 0; j < list_size(pokemonesPendientesDT1); j++) {
			t_pokemon_entrenador* pokemonPendiente1 =
					((t_pokemon_entrenador*) list_get(pokemonesPendientesDT1, j));

			if (strcmp(pokemonInnecesario2->pokemon, pokemonPendiente1->pokemon) == 0) {

				agregarPokemon(entrenador1, pokemonInnecesario2);
				quitarPokemon(entrenador2, pokemonInnecesario2);

			}

		}
	}
	//VER SI TENEMOS QUE HACERLO AFUERA POR LA PLANIFICACION RR DE QUANTUM
		sleep((g_config_team->retardo_ciclo_cpu)*5);
		sem_wait(&mutex_ciclosCPU);
		ciclosCPU+=5;
		sem_post(&mutex_ciclosCPU);

	liberar_lista_de_pokemones(pokemonesInnecesariosDT1);
	liberar_lista_de_pokemones(pokemonesInnecesariosDT2);
	liberar_lista_de_pokemones(pokemonesPendientesDT1);
	liberar_lista_de_pokemones(pokemonesPendientesDT2);

}

void agregarPokemon(t_entrenador* entrenador, t_pokemon_entrenador* pokemon){

	char loEncontro = 0;

	for(int i=0; i < list_size(entrenador->pokemonesObtenidos); i++){
		t_pokemon_entrenador* pokemonABuscar =
						((t_pokemon_entrenador*) list_get(entrenador->pokemonesObtenidos, i));
		if(strcmp(pokemonABuscar->pokemon, pokemon->pokemon)==0){
			pokemonABuscar->cantidad++;
			loEncontro=1;
		}
	}

	if(loEncontro==0){
		t_pokemon_entrenador* pokemonAAgregar = malloc(sizeof(t_pokemon_entrenador));
		pokemonAAgregar->cantidad=1;
		pokemonAAgregar->pokemon=pokemon->pokemon;
		list_add(entrenador->pokemonesObtenidos, pokemonAAgregar);
	}
}

void quitarPokemon(t_entrenador* entrenador, t_pokemon_entrenador* pokemon) {
	printf("Entro a quitarPOkemon \n");
	char hayQueEliminarPokemon = 0;
	int indicePokemon;

	for (int i = 0; i < list_size(entrenador->pokemonesObtenidos); i++) {
		t_pokemon_entrenador* pokemonABuscar =
				((t_pokemon_entrenador*) list_get(
						entrenador->pokemonesObtenidos, i));
		if (strcmp(pokemonABuscar->pokemon, pokemon->pokemon) == 0) {
			if(pokemonABuscar->cantidad > 1){
				pokemonABuscar->cantidad--;
			}
			else{
				hayQueEliminarPokemon = 1;
				indicePokemon=i;
			}


		}
	}

	if (hayQueEliminarPokemon == 1) {
		t_pokemon_entrenador* pokemonAEliminar=list_remove(entrenador->pokemonesObtenidos, indicePokemon);
		//free(pokemonAEliminar->pokemon);
		free(pokemonAEliminar);
	}
}

///FALTA LIBERAR LAS 4 LISTAS
char puedeIntercambiarPokemon(t_entrenador* entrenador1,
		t_entrenador* entrenador2) {

	t_list* pokemonesInnecesariosDT1 = pokemonesInnecesarios(entrenador1);
	t_list* pokemonesInnecesariosDT2 = pokemonesInnecesarios(entrenador2);
	t_list* pokemonesPendientesDT1 = pokemonesPendientes(entrenador1);
	t_list* pokemonesPendientesDT2 = pokemonesPendientes(entrenador2);

	char leSirveAlDT1 = 0;
	char leSirveAlDT2 = 0;

	//Evalúo que le sirva al entrenador 2
	for (int i = 0; i < list_size(pokemonesInnecesariosDT1); i++) {

		char* pokemonInnecesario = ((t_pokemon_entrenador*) list_get(
				pokemonesInnecesariosDT1, i))->pokemon;

		for (int j = 0; j < list_size(pokemonesPendientesDT2); j++) {
			char* pokemonPendiente = ((t_pokemon_entrenador*) list_get(
					pokemonesPendientesDT2, j))->pokemon;

			if (strcmp(pokemonInnecesario, pokemonPendiente) == 0) {

				leSirveAlDT2 = 1;

			}

		}
	}

	//Evalúo que le sirva al entrenador 1
	for (int i = 0; i < list_size(pokemonesInnecesariosDT2); i++) {

		char* pokemonInnecesario = ((t_pokemon_entrenador*) list_get(
				pokemonesInnecesariosDT2, i))->pokemon;

		for (int j = 0; j < list_size(pokemonesPendientesDT1); j++) {
			char* pokemonPendiente = ((t_pokemon_entrenador*) list_get(
					pokemonesPendientesDT1, j))->pokemon;

			if (strcmp(pokemonInnecesario, pokemonPendiente) == 0) {

				leSirveAlDT1 = 1;

			}

		}
	}

	liberar_lista_de_pokemones(pokemonesInnecesariosDT1);
	liberar_lista_de_pokemones(pokemonesInnecesariosDT2);
	liberar_lista_de_pokemones(pokemonesPendientesDT1);
	liberar_lista_de_pokemones(pokemonesPendientesDT2);

	//Para poder intercambiar, a los dos les debe interesar el intercambio
	return leSirveAlDT1 && leSirveAlDT2;

}




t_list* pokemonesInnecesarios(t_entrenador* entrenador){

	t_list* pokemonesInnesarios = list_create();

	for(int i=0; i< list_size(entrenador->pokemonesObtenidos); i++){
		char esObjetivo= 0;
		int cantidadObjetivo=0;

		t_pokemon_entrenador* pokemonObtenido= ((t_pokemon_entrenador*)list_get(entrenador->pokemonesObtenidos, i));

		for(int j=0; j< list_size(entrenador->objetivoEntrenador); j++){
			t_pokemon_entrenador* pokemonObjetivo= ((t_pokemon_entrenador*)list_get(entrenador->objetivoEntrenador, j));

			if(strcmp(pokemonObtenido->pokemon,pokemonObjetivo->pokemon)== 0 &&
					(pokemonObtenido->cantidad> pokemonObjetivo->cantidad)){

				cantidadObjetivo= pokemonObtenido->cantidad - pokemonObjetivo->cantidad;
				esObjetivo=1;
			}


		}

		if(!esObjetivo){

			t_pokemon_entrenador* pokemonInnesario = malloc(sizeof(t_pokemon_entrenador));
			pokemonInnesario->cantidad = ((t_pokemon_entrenador*)list_get(entrenador->pokemonesObtenidos, i))->cantidad;
			pokemonInnesario->pokemon = ((t_pokemon_entrenador*)list_get(entrenador->pokemonesObtenidos, i))->pokemon;
			list_add(pokemonesInnesarios, pokemonInnesario);

		}else if(cantidadObjetivo>0){
			t_pokemon_entrenador* pokemonInnesario = malloc(sizeof(t_pokemon_entrenador));
			pokemonInnesario->cantidad = cantidadObjetivo;
			pokemonInnesario->pokemon = ((t_pokemon_entrenador*)list_get(entrenador->pokemonesObtenidos, i))->pokemon;
			list_add(pokemonesInnesarios, pokemonInnesario);
		}

	}
	return pokemonesInnesarios;
}

t_list* pokemonesPendientes(t_entrenador* entrenador) {

	t_list* pokemonesPendientes = list_create();

	for (int i = 0; i < list_size(entrenador->objetivoEntrenador); i++) {
		char esPendiente = 0;
		int cantidadObjetivo = 0;
		t_pokemon_entrenador* pokemonObjetivo =
				((t_pokemon_entrenador*) list_get(
						entrenador->objetivoEntrenador, i));

		for (int j = 0; j < list_size(entrenador->pokemonesObtenidos); j++) {
			t_pokemon_entrenador* pokemonObtenido =
					((t_pokemon_entrenador*) list_get(
							entrenador->pokemonesObtenidos, j));

			if (strcmp(pokemonObtenido->pokemon, pokemonObjetivo->pokemon)
					== 0) {

				if (pokemonObtenido->cantidad < pokemonObjetivo->cantidad) {
					cantidadObjetivo = pokemonObjetivo->cantidad
							- pokemonObtenido->cantidad;
					esPendiente = 1;
				}
				else{
					cantidadObjetivo = -1;
				}


			}

		}

		if (esPendiente == 0 && cantidadObjetivo != -1) {

			t_pokemon_entrenador* pokemonPendiente = malloc(
					sizeof(t_pokemon_entrenador));
			pokemonPendiente->cantidad = ((t_pokemon_entrenador*) list_get(
					entrenador->objetivoEntrenador, i))->cantidad;
			pokemonPendiente->pokemon = ((t_pokemon_entrenador*) list_get(
					entrenador->objetivoEntrenador, i))->pokemon;
			list_add(pokemonesPendientes, pokemonPendiente);

		} else if (cantidadObjetivo > 0) {
			t_pokemon_entrenador* pokemonPendiente = malloc(
					sizeof(t_pokemon_entrenador));
			pokemonPendiente->cantidad = cantidadObjetivo;
			pokemonPendiente->pokemon = ((t_pokemon_entrenador*) list_get(
					entrenador->objetivoEntrenador, i))->pokemon;
			list_add(pokemonesPendientes, pokemonPendiente);
		}

	}
	return pokemonesPendientes;


}
