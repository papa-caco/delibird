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

ssize_t despachar_mensaje_a_suscriptor(t_socket_cliente_broker *socket, t_tipo_mensaje id_cola, int ult_id_recibido, t_suscriptor_broker *suscriptor, t_log *logger);

ssize_t despachar_msjs_get(t_socket_cliente_broker *socket, int ult_id_recibido, t_suscriptor_broker *suscriptor, t_log *logger);

ssize_t despachar_msjs_new(t_socket_cliente_broker *socket, int ult_id_recibido, t_suscriptor_broker *suscriptor, t_log * logger);

ssize_t despachar_msjs_catch(t_socket_cliente_broker *socket, int ult_id_recibido, t_suscriptor_broker *suscriptor, t_log * logger);

ssize_t despachar_msjs_localized(t_socket_cliente_broker *socket, int ult_id_recibido, t_suscriptor_broker *suscriptor, t_log * logger);

ssize_t despachar_msjs_appeared(t_socket_cliente_broker *socket, int ult_id_recibido, t_suscriptor_broker *suscriptor, t_log * logger);

ssize_t despachar_msjs_caught(t_socket_cliente_broker *socket, int ult_id_recibido, t_suscriptor_broker *suscriptor, t_log * logger);

void set_msg_enviado_a_suscriptor(t_broker_queue *cola_broker, int id_suscriptor, int id_mensaje);

t_queue_msg *get_msg_sin_enviar(t_broker_queue *cola_broker, int id_suscriptor);

int cant_msjs_sin_enviar(t_broker_queue *cola_broker, int id_suscriptor);

void set_sin_recibir_por_todos(t_queue_msg *mensaje_cola);

bool es_msj_con_mismo_id(t_queue_msg *mensaje, int id_mensaje);

bool es_msj_sin_enviar(t_queue_msg *queue_msg, int id_suscriptor);

int poner_datos_msj_en_particion_cache(t_queue_msg *msg_queue, t_log *logger);

t_particion_dinamica *generar_particion_dinamica(t_queue_msg *msg_queue);

t_particion_dinamica *buscar_particion_dinamica_libre(t_queue_msg *msg_queue);

t_particion_dinamica *particion_proxima_victima(void);

t_particion_dinamica *reemplazar_particion_dinamica(t_queue_msg *msg, t_log *logger);

t_particion_dinamica *consolidar_particion_dinamica(t_particion_dinamica *particion, t_log *logger);

void mover_datos_particion_dinamica_a_swap(t_particion_dinamica *particion, t_log *logger);

void obtener_datos_msj_de_particion_cache(t_queue_msg *msg_queue, t_log *g_logger);

void eliminar_msg_data_particion_cache(int id_mensaje, t_log *logger);

t_particion_dinamica *generar_particion_swap(t_particion_dinamica *part_din);

t_particion_dinamica *buscar_particion_swap_libre(uint32_t data_size);

t_particion_dinamica *get_particion_cache_por_id_mensaje(t_cache_particiones *cache, int id_mensaje);

t_particion_dinamica *get_particion_cache_por_id_particion(t_cache_particiones *cache, int id_partcion);

t_queue_msg *generar_nuevo_mensaje_cola(void);

t_broker_queue *cola_broker_suscripcion(t_tipo_mensaje tipo_mensaje);

pthread_mutex_t *semaforo_cola(t_tipo_mensaje tipo_mensaje);

int espacio_disponible_en_cache(t_cache_particiones *cache);

int espacio_disponible_en_swap(void);

int tamano_particiones_libres(int index);

int tamano_particiones_proximo_reemplazo(int data_size);

void incremento_cnt_id_mensajes(void);

#endif /* SRC_BROKER_OPER_H_ */
