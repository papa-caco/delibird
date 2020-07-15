/*
 * suscripcion.h
 *
 *  Created on: 3 jun. 2020
 *      Author: utnso
 */

#ifndef SRC_SUSCRIPCION_H_
#define SRC_SUSCRIPCION_H_
#include "utils_gc.h"

void iniciar_suscripciones_broker_gc(t_log *logger);

int iniciar_suscripcion_cola_gc(t_tipo_mensaje cola_suscripta, t_log *logger);

void suscripcion_gc(t_tipo_mensaje *cola);

uint32_t rcv_msjs_broker_gc(op_code codigo_operacion, int socket_cliente, t_log *logger);

void lanzar_reconexion_broker_gc(t_log *logger);

void funciones_reconexion_gc(void);

int conexion_broker_gc(t_tipo_mensaje cola, t_log *logger);

int contador_msjs_colas_gc(t_tipo_mensaje cola_suscripcion);

void iniciar_cnt_msjs_gc(void);

#endif /* SRC_SUSCRIPCION_H_ */
