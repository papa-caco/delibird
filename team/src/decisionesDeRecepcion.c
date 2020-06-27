/*
 * decisionesDeRecepcion.c
 *
 *  Created on: 16 jun. 2020
 *      Author: utnso
 */

#include "decisionesDeRecepcion.h"

char meSirvePokemon(char* nombrePokemon){

	int cantidadPokemon=0;

	int cantidadPokemonObjetivoGlobal=0;

	sem_wait(&sem_pokemonesObjetivoGlobal);

	t_pokemon_entrenador*  pokemonABuscar = list_buscar(objetivoGlobalEntrenadores, nombrePokemon);


	if(pokemonABuscar!= NULL){

		printf("Lo encontró en objetivo global \n");

					cantidadPokemonObjetivoGlobal = pokemonABuscar->cantidad;
					sem_post(&sem_pokemonesObjetivoGlobal);
	}else {
		sem_post(&sem_pokemonesObjetivoGlobal);
		return 0;
	}

	sem_wait(&sem_pokemonesReservados);

		printf("Lo busca en pokemones reservados\n");

		cantidadPokemon += cantidadDeEspecieEnReservados(nombrePokemon);

		printf("Ya agregó la cantidad de reservados que encontró \n");

		sem_post(&sem_pokemonesReservados);


	//Esta lógica fue cambiada porque no está toda la especie en una sola instancia con la cantidad total

	sem_wait(&sem_pokemonesLibresEnElMapa);

	printf("Lo busca en en pokemones libres \n");

	cantidadPokemon += cantidadDeEspecieEnPokemonesLibres(nombrePokemon);

	sem_post(&sem_pokemonesLibresEnElMapa);


	//Esta lógica también fue cambiada por la misma razón

	//sem_wait(&sem_pokemonesReservados);

	//printf("Lo busca en pokemones reservados\n");

	//cantidadPokemon += cantidadDeEspecieEnReservados(nombrePokemon);

	//printf("Ya agregó la cantidad de reservados que encontró");

	//sem_post(&sem_pokemonesReservados);


	//Esto está bien


	sem_wait(&sem_pokemonesGlobalesAtrapados);

	pokemonABuscar = list_buscar(pokemonesAtrapadosGlobal, nombrePokemon);

		if(pokemonABuscar!= NULL){

				printf("Lo encontró en globales atrapados \n");

				cantidadPokemon+= pokemonABuscar->cantidad;

				sem_post(&sem_pokemonesGlobalesAtrapados);
			}


		printf("Está por devolver el valor de si le sirvió o no \n");
	return 	cantidadPokemonObjetivoGlobal > cantidadPokemon;
}

int cantidadDeEspecieEnReservados(char* nombreBuscado) {

	printf("Lo empieza a buscar en reservados \n");

	int cantidadEncontrada = 0;
	t_pokemon_entrenador_reservado* pokAux;

	for (int i = 0; list_get(pokemonesReservadosEnElMapa, i) != NULL; i++) {
		printf("Entra al for \n");

			pokAux = (t_pokemon_entrenador_reservado*) list_get(pokemonesReservadosEnElMapa, i);

			if(pokAux->pokemon == nombreBuscado){
				cantidadEncontrada += pokAux->cantidad;
			}
	}
	printf("Salió del for \n");

	return cantidadEncontrada;
}

int cantidadDeEspecieEnPokemonesLibres(char* nombreBuscado) {
	int cantidad_Encontrada = 0;
	t_pokemon_entrenador* pokAux;

	for (int i = 0; list_get(pokemonesLibresEnElMapa, i) != NULL; i++) {

		pokAux = (t_pokemon_entrenador*) list_get(
				pokemonesLibresEnElMapa, i);

		if (pokAux->pokemon == nombreBuscado) {
			cantidad_Encontrada += pokAux->cantidad;
		}
	}

	return cantidad_Encontrada;
}


