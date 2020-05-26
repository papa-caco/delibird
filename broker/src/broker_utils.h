/*
 ============================================================================
 Name        : broker_utils.h

 Author      : Los Que Aprueban.

 Created on	 : 9 may. 2020

 Version     :

 Description :
 ============================================================================
 */
#ifndef SRC_BROKER_UTILS_H_
#define SRC_BROKER_UTILS_H_
#define RUTA_LOG_DEBUG "logs/logdebug.log"
#define RUTA_CONFIG "config/broker.config"
#include "broker_estructuras.h"
#include "broker_oper.h"
#include "broker_serializ.h"

//-------------VARIABLES GLOBALES-------------------//

t_log *g_logger;

t_log *g_logdebug;

t_config *g_config;

t_config_broker *g_config_broker;

int g_msg_counter;

int g_cache_segment_id;

int g_cache_space_used;

t_broker_queue *g_queue_get_pokemon;

t_broker_queue *g_queue_new_pokemon;

t_broker_queue *g_queue_catch_pokemon;

t_broker_queue *g_queue_appeared_pokemon;

t_broker_queue *g_queue_localized_pokemon;

t_broker_queue *g_queue_caught_pokemon;

pthread_t g_thread_server;

sem_t g_mutex_sent_msg;

sem_t g_mutex_rcvd_msg;

sem_t g_mutex_queue_get;

sem_t g_mutex_queue_new;

sem_t g_mutex_queue_catch;

sem_t g_mutex_queue_localized;

sem_t g_mutex_queue_appeared;

sem_t g_mutex_queue_caught;



//---------------FIRMA DE FUNCIONES------------------//

void iniciar_log_broker(void);

void leer_config_broker(char *path);

void iniciar_estructuras_broker(void);

void inicio_server_broker(void);

void alta_suscriptor_cola(t_broker_queue *cola_broker, t_handsake_suscript *handshake);

void set_mensaje_enviado_a_suscriptor_cola(t_broker_queue *cola_broker, t_suscriptor_broker *suscriptor);

void deshabilitar_suscriptor_cola(t_handsake_suscript *suscrpitor);

bool es_suscriptor_habilitado(t_broker_queue *cola_broker, sem_t semaforo_cola, int id_suscriptor);

t_suscriptor_broker *obtengo_suscriptor_cola(t_broker_queue *cola_broker, int id_suscriptor);

void eliminar_suscriptor_por_id(t_broker_queue *cola_broker, int id_suscriptor);

void quitar_suscriptor_cola(t_broker_queue *cola_broker, void *suscriptor);

void eliminar_mensaje_cola(t_queue_msg *mensaje_cola);

void inicializar_cola_get_pokemon(void);

void inicializar_cola_new_pokemon(void);

void inicializar_cola_catch_pokemon(void);

void inicializar_cola_appeared_pokemon(void);

void inicializar_cola_localized_pokemon(void);

void inicializar_cola_caught_pokemon(void);

void atender_publicacion(op_code cod_op, t_socket_cliente_broker *socket);

void atender_suscripcion(op_code cod_op, t_socket_cliente_broker *socket);

t_algoritmo_memoria algoritmo_memoria(char *valor);

t_algoritmo_part_libre algoritmo_part_libre(char *valor);

t_algoritmo_reemplazo algoritmo_reemplazo(char *valor);

#endif /* SRC_BROKER_UTILS_H_ */
