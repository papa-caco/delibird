/*
 * suscripcion.h
 *
 *  Created on: 3 jun. 2020
 *      Author: utnso
 */

#ifndef SRC_SUSCRIPCION_H_
#define SRC_SUSCRIPCION_H_
#include "utils_gc.h"


void iniciar_suscripcion(void);

int conexion_broker(t_tipo_mensaje cola, t_log *logger);

void suscripcion(t_tipo_mensaje cola, t_log *logger);

void *threadfunction(void *parametro);

#endif /* SRC_SUSCRIPCION_H_ */
