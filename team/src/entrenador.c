/*
 * entrenador.c
 *
 *  Created on: 4 jun. 2020
 *      Author: utnso
 */


#include "entrenador.h"

//------------------------------FUNCIONES BÁSICAS DEL ENTRENADOR-------------------------------------------

/////MOVER ENTRENADOR///////////////////

void moverEntrenador(t_entrenador* entrenador, t_posicion_entrenador* posicionAMoverse){

	//AGREGAR SEMAFORO DEL ENTRENADOR EN PARTICULAR

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

	enviar_catch_pokemon_broker(entrenador->posicion->pos_x, entrenador->posicion->pos_y, pokemon->pokemon, g_logger);

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
