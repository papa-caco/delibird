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


#include "teamInitializer.h"






void moverEntrenador(t_entrenador* entrenador, t_posicion_entrenador* posicionAMoverse);

void intentarAtraparPokemon(t_entrenador* entrenador, t_pokemon_entrenador* pokemon);

char puedeIntercambiarPokemon(t_entrenador* entrenador1, t_entrenador* entrenador2);

t_list* pokemonesInnecesarios(t_entrenador* entrenador);

t_list* pokemonesPendientes(t_entrenador* entrenador);

int calcularDistancia(t_posicion_entrenador* posicionActual, t_posicion_entrenador* posicionAMoverse);

void intercambiarPokemon(t_entrenador* entrenador1, t_entrenador* entrenador2);

void agregarPokemon(t_entrenador* entrenador, t_pokemon_entrenador* pokemon);

void quitarPokemon(t_entrenador* entrenador, t_pokemon_entrenador* pokemon);




#endif /* SRC_ENTRENADOR_H_ */
