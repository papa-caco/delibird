/*
 ============================================================================
 Name        : broker_oper.h

 Author      : Los Que Aprueban.

 Created on	 : 16 may. 2020

 Version     :

 Description :
 ============================================================================
 */

#ifndef SRC_BROKER_OPER_H_
#define SRC_BROKER_OPER_H_
#include "broker_estructuras.h"
#include "broker_serializ.h"
#include "broker_utils.h"

void enqueue_msg_get(t_msg_get_broker *msg_get_broker, t_log *logger, int cliente_fd);

void enqueue_msg_new(t_msg_new_broker *msg_new_broker, t_log *logger, int cliente_fd);

void enqueue_msg_catch(t_msg_catch_broker *msg_catch_broker, t_log *logger, int cliente_fd);

void enqueue_msg_appeared(t_msg_appeared_broker *msg_appeared_broker, t_log *logger, int cliente_fd);

void enqueue_msg_caught(t_msg_caught_broker *msg_caught_broker, t_log *logger, int cliente_fd);

void enqueue_msg_localized(t_msg_localized_broker *msg_localized_broker, t_log *logger, int cliente_fd);

void poner_msj_en_cola(t_queue_msg *mensaje_cola, t_broker_queue *cola_broker);

void incremento_sem_cont_suscriptor(t_suscriptor_broker *suscriptor);

void despachar_mensaje_a_suscriptor(t_socket_cliente_broker *socket, t_tipo_mensaje id_cola, t_suscriptor_broker *suscriptor, t_log *logger);

void despachar_msjs_get(t_socket_cliente_broker *socket, t_suscriptor_broker *suscriptor, t_log *logger);

void despachar_msjs_new(t_socket_cliente_broker *socket, t_suscriptor_broker *suscriptor, t_log * logger);

void despachar_msjs_catch(t_socket_cliente_broker *socket, t_suscriptor_broker *suscriptor, t_log * logger);

void despachar_msjs_localized(t_socket_cliente_broker *socket, t_suscriptor_broker *suscriptor, t_log * logger);

void despachar_msjs_appeared(t_socket_cliente_broker *socket, t_suscriptor_broker *suscriptor, t_log * logger);

void despachar_msjs_caught(t_socket_cliente_broker *socket, t_suscriptor_broker *suscriptor, t_log * logger);

t_queue_msg *get_msg_sin_enviar(t_broker_queue *cola_broker, int id_suscriptor);

int cant_msjs_sin_enviar(t_broker_queue *cola_broker, int id_suscriptor);

void eliminar_msj_cola_broker(void *mensaje);

bool es_msj_sin_enviar(t_queue_msg *queue_msg, int id_suscriptor);

bool cache_espacio_suficiente(int data_size);

void guardar_datos_en_segmento(t_queue_msg *msg_queue, t_log *logger);

t_queue_msg *new_queue_msg(void);

int espacio_cache_msg_get(t_msg_get_broker *msg_get);

int espacio_cache_msg_new(t_msg_new_broker *msg_new);

int espacio_cache_msg_catch(t_msg_catch_broker *msg_catch);

int espacio_cache_msg_appeared(t_msg_appeared_broker *msg_appeared);

int espacio_cache_msg_caught(t_msg_caught_broker *msg_caught);

int espacio_cache_msg_localized(t_msg_localized_broker *msg_localized);

t_broker_queue *cola_broker_suscripcion(t_tipo_mensaje tipo_mensaje);

sem_t semaforo_cola(t_tipo_mensaje tipo_mensaje);

int espacio_disponible_en_cache(void);

void incremento_espacio_cache_usado(int tamano);

void incremento_cnt_id_mensajes(void);

void incremento_id_partition(void);

#endif /* SRC_BROKER_OPER_H_ */
