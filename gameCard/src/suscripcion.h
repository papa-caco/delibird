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

void funciones_reconexion_gc(int valor);

int enviar_appeared_pokemon_broker(t_msg_new_gamecard *msg_new, t_log *logger);

int connect_broker_y_enviar_mensaje_appeared(t_msg_appeared_broker *msg_appeared);

int enviar_localized_pokemon_broker(uint32_t id_correlativo, char *pokemon, t_list *posiciones, t_log *logger);

int connect_broker_y_enviar_mensaje_localized(t_msg_localized_broker *msg_localized);

int enviar_caught_pokemon_broker(uint32_t id_correlativo, t_result_caught resultado, t_log *logger);

int connect_broker_y_enviar_mensaje_caught(t_msg_caught_broker *msg_caught);

int enviar_end_suscripcion_broker(t_tipo_mensaje cola_id, int contador_msgs, t_log *logger);

void connect_broker_y_enviar_end_suscript(t_handsake_suscript *handshake);

int conexion_broker_gc(t_tipo_mensaje cola, t_log *logger);

void manejo_senial_externa_gc(void);

void funcion_captura_senial_gc(int senial);

void finalizar_suscripciones(int senial);

char *senial_recibida_gc(int senial);

int contador_msjs_colas_gc(t_tipo_mensaje cola_suscripcion);

void iniciar_cnt_msjs_gc(void);

#endif /* SRC_SUSCRIPCION_H_ */
