/*
 * entrenador.c
 *
 *  Created on: 4 jun. 2020
 *      Author: utnso
 */


#include "entrenador.h"


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

char puedeIntercambiarPokemon(t_entrenador* entrenador1, t_entrenador* entrenador2){
	t_pokemon_entrenador* pokemon1= NULL;
	t_pokemon_entrenador* pokemon2= NULL;

	t_list* pokemonesInnecesariosDT1 = pokemonesInnecesarios(entrenador1);
	t_list* pokemonesInnecesariosDT2 = pokemonesInnecesarios(entrenador2);
	t_list* pokemonesPendientesDT1 = pokemonesPendientes(entrenador1);
	t_list* pokemonesPendientesDT2 = pokemonesPendientes(entrenador2);

	for(int i=0; list_size(pokemonesInnecesariosDT1); i++){

		t_pokemon_entrenador* pokemonInnecesario= ((t_pokemon_entrenador*)list_get(pokemonesInnecesariosDT1, i))->pokemon;

		for(int j=0; list_size(pokemonesPendientesDT2); j++){
			t_pokemon_entrenador* pokemonPendiente= ((t_pokemon_entrenador*)list_get(pokemonesPendientesDT2, j))->pokemon;

			if(strcmp(pokemonInnecesario,pokemonPendiente)== 0){


						}

			}
	}

}

void intentarAtraparPokemon(t_entrenador* entrenador, t_pokemon_entrenador* pokemon){

	enviar_catch_pokemon_broker(entrenador->posicion->pos_x, entrenador->posicion->pos_y, pokemon->pokemon, g_logger);

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

t_list* pokemonesInnecesarios(t_entrenador* entrenador){

	t_list* pokemonesInnesarios = list_create();

	for(int i=0; i< list_size(entrenador->pokemonesObtenidos); i++){
		char esObjetivo= 0;
		int cantidadObjetivo=0;

		t_pokemon_entrenador* pokemonObtenido= ((t_pokemon_entrenador*)list_get(entrenador->pokemonesObtenidos, i))->pokemon;

		for(int j=0; j< list_size(entrenador->objetivoEntrenador); j++){
			t_pokemon_entrenador* pokemonObjetivo= ((t_pokemon_entrenador*)list_get(entrenador->objetivoEntrenador, j))->pokemon;

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

t_list* objetivosPendientes(t_entrenador* entrenador){

	t_list* pokemonesPendientes = list_create();

		for(int i=0; i< list_size(entrenador->objetivoEntrenador); i++){
			char esPendiente= 0;
			int cantidadObjetivo=0;
			t_pokemon_entrenador* pokemonObjetivo= ((t_pokemon_entrenador*)list_get(entrenador->objetivoEntrenador, i))->pokemon;

			for(int j=0; j< list_size(entrenador->pokemonesObtenidos); j++){
				t_pokemon_entrenador* pokemonObtenido= ((t_pokemon_entrenador*)list_get(entrenador->pokemonesObtenidos, j))->pokemon;

				if(strcmp(pokemonObtenido->pokemon,pokemonObjetivo->pokemon)== 0 &&
						(pokemonObtenido->cantidad< pokemonObjetivo->cantidad)){

					cantidadObjetivo= pokemonObjetivo->cantidad -pokemonObtenido->cantidad;
					esPendiente=1;
				}


			}

			if(!esPendiente){

				t_pokemon_entrenador* pokemonPendiente = malloc(sizeof(t_pokemon_entrenador));
				pokemonPendiente->cantidad = ((t_pokemon_entrenador*)list_get(entrenador->objetivoEntrenador, i))->cantidad;
				pokemonPendiente->pokemon = ((t_pokemon_entrenador*)list_get(entrenador->objetivoEntrenador, i))->pokemon;
				list_add(pokemonesPendientes, pokemonPendiente);

			}else if(cantidadObjetivo>0){
				t_pokemon_entrenador* pokemonPendiente = malloc(sizeof(t_pokemon_entrenador));
				pokemonPendiente->cantidad = cantidadObjetivo;
				pokemonPendiente->pokemon = ((t_pokemon_entrenador*)list_get(entrenador->objetivoEntrenador, i))->pokemon;
				list_add(pokemonesPendientes, pokemonPendiente);
			}

		}
		return pokemonesPendientes;
}
