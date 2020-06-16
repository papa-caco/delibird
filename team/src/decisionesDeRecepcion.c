/*
 * decisionesDeRecepcion.c
 *
 *  Created on: 16 jun. 2020
 *      Author: utnso
 */

#include "decisionesDeRecepcion.h"

char meSirvePokemon(char* nombrePokemon){

	int cantidadPokemon;

	int cantidadPokemonObjetivoGlobal;

	t_pokemon_entrenador*  pokemonABuscar = list_buscar(objetivoGlobalEntrenadores, nombrePokemon);

	if(pokemonABuscar!= NULL){

					cantidadPokemonObjetivoGlobal = pokemonABuscar->cantidad;
	}else {
		return 0;
	}

	pokemonABuscar = list_buscar(pokemonesLibresEnElMapa, nombrePokemon);

	if(pokemonABuscar!= NULL){

		cantidadPokemon+= pokemonABuscar->cantidad;
	}

	pokemonABuscar = list_buscar(pokemonesReservadosEnElMapa, nombrePokemon);

	if(pokemonABuscar!= NULL){

			cantidadPokemon+= pokemonABuscar->cantidad;
		}

	pokemonABuscar = list_buscar(pokemonesAtrapadosGlobal, nombrePokemon);

		if(pokemonABuscar!= NULL){

				cantidadPokemon+= pokemonABuscar->cantidad;
			}



	return 	cantidadPokemonObjetivoGlobal > cantidadPokemon;
}




