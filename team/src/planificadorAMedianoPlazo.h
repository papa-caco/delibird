/*
 * planificadorAMedianoPlazo.h
 *
 *  Created on: 7 jul. 2020
 *      Author: utnso
 */

#ifndef SRC_PLANIFICADORAMEDIANOPLAZO_H_
#define SRC_PLANIFICADORAMEDIANOPLAZO_H_

#include "utilsTeam.h"

t_entrenador* buscarPrimerEntrenadorEnDeadlock();
void planificadorMedianoPlazo();

void liberar_variables_globales();

#endif /* SRC_PLANIFICADORAMEDIANOPLAZO_H_ */
