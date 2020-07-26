/*
 * planificadorAMedianoPlazo.h
 *
 *  Created on: 7 jul. 2020
 *      Author: utnso
 */

#ifndef SRC_PLANIFICADORAMEDIANOPLAZO_H_
#define SRC_PLANIFICADORAMEDIANOPLAZO_H_

#include "utilsTeam.h"
#include "entrenador.h"

t_entrenador* buscarPrimerEntrenadorEnDeadlock();

void planificadorMedianoPlazo();

void liberar_variables_globales();

void logearResultadoTeam();

char todosQuierenMoverseAPokemon(t_queue* colaDeEntrenadores);

t_entrenador* buscarEntrenadorMasConvenienteEnCola(t_queue* colaEntrenadores);

t_entrenador* buscarEntrenadorMasConvenienteRR(t_queue* colaEntrenadores);

#endif /* SRC_PLANIFICADORAMEDIANOPLAZO_H_ */
