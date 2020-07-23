/*
 * utils.h
 *
 *  Created on: 20 abr. 2020
 *      Author: utnso
 */
#ifndef UTILS_H_
#define UTILS_H_

#include <delibird/estructuras.h>
#include <delibird/conexiones.h>
#include <delibird/mensajeria.h>
#include <delibird/serializaciones.h>
#include <signal.h>
#include <time.h>
#include <math.h>
#include <commons/process.h>
#include <commons/txt.h>
#include <sys/stat.h>
#include "tall_grass.h"
#include "suscripcion.h"

#define PATH_CONFIG "config/gameCard.config"
#define PATH_LOG "log/gameCard_dbg.log"

/*** ESTRUCTURAS **/
typedef struct Configuracion_GameCard
{
	char *ip_broker;
	char *puerto_broker;
	char *ip_gamecard;
	char *puerto_gamecard;
	char *path_tall_grass; //PUNTO_MONTAJE_TALLGRASS
	char *dirname_blocks;
	char *dirname_files;
	int id_suscriptor;
	int tiempo_reconexion;
	int tmp_reintento_oper;
	char *ruta_log;
	bool show_logs_on_screen;
	char *ruta_bitmap;
	char *tg_block_size;
	char *tg_blocks;
	char *magic_number;
} t_config_gamecard;

typedef struct socket_cliente{
	int cliente_fd;
	int cant_msg_enviados;
} t_socket_cliente; //MODO_SERVIDOR

typedef struct pokemon_semaforo{
	char* pokemon;
	sem_t semaforo;
} t_pokemon_semaforo; //Para verificar si un archivo de pokemon está siendo utilizado

typedef struct operaciones_tallgrass
{
	uint32_t id_correlativo;
	char *pokemon;
	t_posicion_pokemon *posicion;
	t_tipo_mensaje id_cola;
}t_operacion_tallgrass;

/* --- VARIABLES GLOBALES ---*/

t_config_gamecard  *g_config_gc;

t_log *g_logger;

t_log *g_logdebug;

t_config* gc_config;

sem_t sem_mutex_suscripcion;

sem_t sem_mutex_semaforos;

sem_t mutex_msjs_gc;

pthread_t tid_send_appeared;

pthread_t tid_send_caught;

pthread_t tid_send_localized;

pthread_mutex_t g_mutex_envio;

pthread_mutex_t g_mutex_recepcion;

bool status_conexion_broker;

pthread_t tid_reconexion_gc;

pthread_mutex_t mutex_reconexion;

t_list *semaforos_pokemon; // Lista global de semaforos

int g_cnt_msjs_get, g_cnt_msjs_catch, g_cnt_msjs_new;

t_queue *g_reintentos;

/*** DEFINICION INTERFACES **/

void iniciar_gamecard(void);

void inicio_server_gamecard(void);

void iniciar_log_gamecard(void);

void iniciar_estructuras_gamecard(void);

void leer_config(void);

void atender_gameboy_gc(int *cliente_fd);

void recibir_msjs_gameboy(op_code cod_op, int *cliente_fd, t_log *logger);

int recibir_msg_get_pokemon(t_msg_get_gamecard *msg_get, t_log *logger);

void procesar_msg_get_pokemon(t_msg_get_gamecard *msg_get);

int recibir_msg_new_pokemon(t_msg_new_gamecard *msg_new, t_log *logger);

void procesar_msg_new_pokemon(t_msg_new_gamecard *msg_new);

int recibir_msg_catch_pokemon(t_msg_catch_gamecard *msg_catch, t_log *logger);

void procesar_msg_catch_pokemon(t_msg_catch_gamecard *msg_catch);

void encolar_operacion_tallgrass(int32_t id_correlativo, char *pokemon, t_posicion_pokemon *posicion, t_tipo_mensaje cola, t_log *logger);

void devolver_caught_pokemon(t_msg_catch_gamecard *msg, int socket_cliente);

int dispatcher_operaciones_pendientes(t_log *logger);

void inicio_dispatcher_operaciones_pendientes(void);

void completar_operaciones_pendientes(t_log *logger);

void eliminar_operacion_tallgrass(t_operacion_tallgrass *operacion);

void liberar_lista_posiciones(t_list* lista);

void liberar_listas(char** lista);

// Funciones para la lista de semaforos global

t_pokemon_semaforo *obtener_semaforo_pokemon(char* pokemon);

void eliminar_semaforo_pokemon(char* pokemon);

void crear_semaforo_pokemon(char* pokemon);

char *concatenar_posiciones_pokemon(t_list *posiciones);

bool es_cod_oper_mensaje_gamecard(op_code codigo_operacion);

bool nro_par(int numero);

#endif
// export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/utnso/tp-2020-1c-Los-Que-Aprueban/delibird/build
