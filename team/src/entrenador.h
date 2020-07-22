/*
 * entrenador.h
 *
 *  Created on: 4 jun. 2020
 *      Author: utnso
 */

#ifndef SRC_ENTRENADOR_H_
#define SRC_ENTRENADOR_H_

#include<delibird/estructuras.h>
#include<delibird/mensajeria.h>
#include<delibird/serializaciones.h>
#include<delibird/conexiones.h>


#include "utilsTeam.h"

void comportamiento_entrenador(t_entrenador* entrenador);

t_pokemon_entrenador* buscarPokemonMasCercano(t_posicion_entrenador* posicion_Entrenador);

t_entrenador* buscarEntrenadorDelIntercambio(t_entrenador* entrenador);

t_posicion_entrenador* buscarEntrenadorAMoverse(t_entrenador* entrenador);

void moverEntrenador(t_entrenador* entrenador, t_posicion_entrenador* posicionAMoverse);

t_pokemon_entrenador_reservado* moverPokemonAReservados(t_pokemon_entrenador* pokemonAMover, int idReservador);

int intentarAtraparPokemon(t_entrenador* entrenador, t_pokemon_entrenador_reservado* pokemon);

char elIntercambioEsIdeal(t_entrenador* entrenador1, t_entrenador* entrenador2);

char puedoIntercambiar(t_entrenador* entrenadorPorEjecutar, t_entrenador* otroEntrenador);

t_list* pokemonesInnecesarios(t_entrenador* entrenador);

t_list* pokemonesPendientes(t_entrenador* entrenador);

int calcularDistancia(t_posicion_entrenador* posicionActual, t_posicion_entrenador* posicionAMoverse);

void intercambiarIdealPokemon(t_entrenador* entrenador1, t_entrenador* entrenador2);

void intercambiarNormalPokemon(t_entrenador* entrenador1, t_entrenador* entrenador2);

void agregarPokemon(t_entrenador* entrenador, t_pokemon_entrenador* pokemon);

void quitarPokemon(t_entrenador* entrenador, t_pokemon_entrenador* pokemon);




#endif /* SRC_ENTRENADOR_H_ */
