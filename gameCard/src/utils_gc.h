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
	char *file_metadata;
	int id_suscriptor;
	int tiempo_reconexion;
	int tmp_reintento_oper;
	char *ruta_log;
	bool show_logs_on_screen;
	char *ruta_bitmap;
} t_config_gamecard;

typedef struct socket_cliente{
	int cliente_fd;
	int cant_msg_enviados;
} t_socket_cliente; //MODO_SERVIDOR

typedef struct pokemon_semaforo{
	char* pokemon;
	sem_t semaforo;
} t_pokemon_semaforo; //Para verificar si un archivo de pokemon está siendo utilizado

/* --- VARIABLES GLOBALES ---*/

t_config_gamecard  *g_config_gc;

t_log *g_logger;

t_log *g_logdebug;

t_config* gc_config;

sem_t sem_mutex_suscripcion;

sem_t sem_mutex_semaforos;

sem_t mutex_msjs_gc;

pthread_mutex_t g_mutex_cnt_blocks;

bool status_conexion_broker;

pthread_t tid_reconexion_gc;

pthread_mutex_t mutex_reconexion;

t_list *semaforos_pokemon; // Lista global de semaforos

int g_cnt_msjs_get, g_cnt_msjs_catch, g_cnt_msjs_new;

/*** DEFINICION INTERFACES **/

void iniciar_gamecard(void);

void inicio_server_gamecard(void);

void iniciar_log_gamecard(void);

void iniciar_estructuras_gamecard(void);

void leer_config(void);

void procesar_msjs_gameboy(op_code cod_op, int cliente_fd, t_log *logger);

void rcv_new_pokemon(t_msg_new_gamecard *msg);

void devolver_appeared_pokemon(t_msg_new_gamecard *msg, int socket_cliente);

void devolver_posiciones(int socket_cliente, char* pokemon,	int* encontroPokemon);

//void devolver_posiciones(int socket_cliente, char* pokemon,	int* encontroPokemon);

void devolver_caught_pokemon(t_msg_catch_gamecard *msg, int socket_cliente);

void rcv_get_pokemon(t_msg_get_gamecard *msg, int socket_cliente);

void liberar_lista_posiciones(t_list* lista);

void liberar_listas(char** lista);



int tamano_recibido(int bytes);

//void verificarPokemon(char* pathPokemon,t_posicion_pokemon* posicion);

// Funciones para la lista de semaforos global

t_pokemon_semaforo *obtener_semaforo_pokemon(char* pokemon);

void eliminar_semaforo_pokemon(char* pokemon);

void crear_semaforos_pokemon();

void crear_semaforo_pokemon(char* pokemon);

void prueba_semaforo(void);

char *concatenar_posiciones_pokemon(t_list *posiciones);

bool es_cod_oper_mensaje_gamecard(op_code codigo_operacion);

t_list* obtener_posiciones_pokemon(char* pokemon, t_posicion_pokemon *posicion, int incrementar_posicion);

#endif
// export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/utnso/tp-2020-1c-Los-Que-Aprueban/delibird/build
