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



typedef struct Pokemon_Entrenador{
	int cantidad;
	char* pokemon;
} t_pokemon_entrenador;

/*t_queue* new;
//t_list* objetivoGlobal;*/

typedef struct Posicion_Entrenador{
	int pos_x;
	int pos_y;
} t_posicion_entrenador;

typedef enum Estado_Entrenador{
	NEW,
	READY,
	BLOCKED,
	EXEC,
	EXIT,
} t_estado;

typedef struct Entrenador{
	t_posicion_entrenador* posicion;
	t_list* objetivoEntrenador;
	t_list* pokemonesObtenidos;
	//Tal vez como opción podrías agregar una variable hilo acá e ir inicializándoselo a cada uno.
	//La otra es crear los hilos por otro lado e ir manejándolos a tu criterio.
} t_entrenador;


void moverEntrenador(t_entrenador* entrenador, t_posicion_entrenador* posicionAMoverse);

void intentarAtraparPokemon(t_entrenador* entrenador, t_pokemon_entrenador* pokemon);

char puedeIntercambiarPokemon(t_entrenador* entrenador1, t_entrenador* entrenador2);

t_list* pokemonesInnecesarios(t_entrenador* entrenador);

t_list* objetivosPendientes(t_entrenador* entrenador);

int calcularDistancia(t_posicion_entrenador* posicionActual, t_posicion_entrenador* posicionAMoverse);

void intercambiarPokemon(t_entrenador* entrenador1, t_entrenador* entrenador2);

void agregarPokemon(t_entrenador* entrenador, t_pokemon_entrenador* pokemon);

void quitarPokemon(t_entrenador* entrenador, t_pokemon_entrenador* pokemon);




#endif /* SRC_ENTRENADOR_H_ */
