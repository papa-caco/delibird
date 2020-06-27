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

int g_msg_deleted;

int g_page_fault_counter;

t_cache_particiones *g_cache_part;

t_cache_particiones *g_cache_swap;

t_broker_queue *g_queue_get_pokemon;

t_broker_queue *g_queue_new_pokemon;

t_broker_queue *g_queue_catch_pokemon;

t_broker_queue *g_queue_appeared_pokemon;

t_broker_queue *g_queue_localized_pokemon;

t_broker_queue *g_queue_caught_pokemon;

pthread_mutex_t g_mutex_msjs;

pthread_mutex_t g_mutex_cache_part;

pthread_mutex_t g_mutex_queue_get;

pthread_mutex_t g_mutex_queue_new;

pthread_mutex_t g_mutex_queue_catch;

pthread_mutex_t g_mutex_queue_localized;

pthread_mutex_t g_mutex_queue_appeared;

pthread_mutex_t g_mutex_queue_caught;

sem_t g_mutex_borrador;

t_tipo_mensaje g_mantenim_cola;

pthread_t tid_borrador;

t_list *g_team_suscriptos;

t_list *g_gamecards_suscriptos;

//---------------FIRMA DE FUNCIONES------------------//

void iniciar_log_broker(void);

void leer_config_broker(char *path);

void iniciar_estructuras_broker(void);

void inicio_server_broker(void);

void inicializar_cache_particiones_dinamicas(void);

void inicializar_memoria_swap(void);

void alta_suscriptor_cola(t_broker_queue *cola_broker, t_handsake_suscript *handshake);

void agregar_id_suscriptor_lista_suscriptores(t_tipo_mensaje id_cola, int id_suscriptor);

t_list *select_lista_suscriptores(t_tipo_mensaje id_cola);

void set_mensaje_enviado_a_suscriptor_cola(t_broker_queue *cola_broker, t_suscriptor_broker *suscriptor);

void deshabilitar_suscriptor_cola(t_handsake_suscript *suscrpitor);

bool es_suscriptor_habilitado(t_broker_queue *cola_broker, pthread_mutex_t semaforo_cola, int id_suscriptor);

bool msj_enviado_a_todo_suscriptor(t_queue_msg *mensaje_cola);

bool id_suscriptor_no_ingresado(t_list *lista_suscriptores, int *id_suscriptor);

t_suscriptor_broker *obtengo_suscriptor_cola(t_broker_queue *cola_broker, int id_suscriptor);

void eliminar_suscriptor_por_id(t_broker_queue *cola_broker, int id_suscriptor);

void quitar_suscriptor_cola(t_broker_queue *cola_broker, t_suscriptor_broker *suscriptor);

int eliminar_mensajes_cola(t_tipo_mensaje id_cola, int cant_mensajes, t_log *logger);

void eliminar_mensaje_por_id(t_broker_queue *cola_broker, int id_mensaje, t_log *logger);

void destruir_mensaje_cola(t_queue_msg *queue_msg);

void inicializar_colas_broker(void);

void setear_id_cola_broker(t_broker_queue *cola, t_tipo_mensaje tipo_cola);

void atender_publicacion(op_code cod_op, t_socket_cliente_broker *socket);

void atender_suscripcion(op_code cod_op, t_socket_cliente_broker *socket);

void lanzar_borrador_mensajes(t_log *logger);

void borrar_mensajes_habilitados(void);

void disparo_borrador_msjs(void);

bool es_cola_suscripcion_team(t_tipo_mensaje cola_suscripcion);

bool es_cola_suscripcion_gamecard(t_tipo_mensaje cola_suscripcion);

bool codigo_operacion_valido_broker(op_code cod_oper);

int compactar_particiones(t_cache_particiones *cache_part, t_log *logger);

t_list *ordenar_particiones_libres_first_fit(void);

t_list *ordenar_particiones_libres_best_fit(void);

t_list *obtengo_particiones_dinamicas_libres(void);

t_list *obtengo_particiones_swap_libres(void);

void reorganizar_tabla_particiones(t_cache_particiones *cache_part, t_log *logger);

bool ordenar_por_direccion_base(void *part1, void *part2);

bool ordenador_fifo(void *part1, void *part2);

bool ordenar_por_tamano(void *part1, void *part2);

bool es_particion_libre(void *part);

bool es_particion_ocupada(void *part);

bool espacio_suficiente_bloque_part_libres(t_cache_particiones *cache, int data_size);

int dir_heap_part_dinamica(t_queue_msg *msg_queue);

int tamano_particion_dinamica(t_particion_dinamica *particion);

t_particion_dinamica *duplico_particion_dinamica(t_particion_dinamica *part);

void generar_particion_dinamica_libre(int dir_base, int size);

int dir_base_ultimo_bloque_part_din(t_cache_particiones *cache_part);

int ultimo_bloque_cache(t_cache_particiones *cache_part);

bool sin_espacio_ult_bloque_cache(uint32_t data_size, t_cache_particiones *cache);

bool cache_swap_espacio_suficiente(int data_size);

bool cache_espacio_suficiente(uint32_t data_size, t_cache_particiones *cache_part);

void enviar_mensaje_cache_sin_espacio(int cliente_fd, t_log *logger);

int espacio_cache_msg_get(t_msg_get_broker *msg_get);

int espacio_cache_msg_new(t_msg_new_broker *msg_new);

int espacio_cache_msg_catch(t_msg_catch_broker *msg_catch);

int espacio_cache_msg_appeared(t_msg_appeared_broker *msg_appeared);

int espacio_cache_msg_caught(t_msg_caught_broker *msg_caught);

int espacio_cache_msg_localized(t_msg_localized_broker *msg_localized);

void* crear_archivo_swap(char *path, int size, t_log *logger);

t_algoritmo_memoria algoritmo_memoria(char *valor);

char *nombre_cache(t_algoritmo_memoria algoritmo);

t_algoritmo_part_libre algoritmo_part_libre(char *valor);

t_algoritmo_reemplazo algoritmo_reemplazo(char *valor);

#endif /* SRC_BROKER_UTILS_H_ */
