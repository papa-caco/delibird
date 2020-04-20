/*
 *		utils.h
 *
 *  	Created on: 10 apr. 2020
 *
 *      Author:  Grupo "tp-2020-1C Los Que Aprueban"
 *
 *
 */

#ifndef UTILS_H_
#define UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<commons/config.h>
#include<commons/log.h>
#include<string.h>
#include<commons/collections/list.h>

/* ---  DEFINICION DE ESTRUCTURAS ---*/

typedef enum
{
	ID_MENSAJE = 10,
	MSG_CONFIRMED,
	APPEARED_BROKER,
	APPEARED_TEAM,
	CATCH_BROKER,
	CATCH_GAMECARD,
	CAUGHT_BROKER,
	GET_BROKER,
	GET_GAMECARD,
	NEW_BROKER,
	NEW_GAMECARD,
	LOCALIZED_BROKER,
} op_code;

typedef enum {
	UNKNOWN_PROC = 9, SUSCRIPTOR, GAMECARD, BROKER, TEAM, GAMEBOY,
} t_proceso;

typedef enum {
	UNKNOWN_QUEUE = 9,
	NEW_POKEMON,
	APPEARED_POKEMON,
	CATCH_POKEMON,
	CAUGHT_POKEMON,
	GET_POKEMON,
	LOCALIZED_POKEMON,
} t_tipo_mensaje;

typedef enum {
	FAIL, OK,
} t_result_caught;

typedef struct {
	t_proceso proceso;
	t_tipo_mensaje tipo_mensaje;
	t_list *argumentos;
} t_mensaje_gameboy;

typedef struct {
	char *ip_broker;
	char *ip_gamecard;
	char *ip_team;
	char *puerto_broker;
	char *puerto_gamecard;
	char *puerto_team;
	int coord_x_max;
	int coord_y_max;
	int cant_max_pokemon;
	int id_mensaje_unico;
	char *ruta_log;
} t_config_gameboy;

typedef struct {
	int size;
	void* data;
} t_stream;

typedef struct {
	op_code codigo_operacion;
	t_stream* buffer;
} t_paquete;

/* ---  DEFINICION DE VARIABLES GLOBALES ---*/

t_log *g_logger;
t_config *g_config;
t_config_gameboy *g_config_gameboy;

/* ---  DEFINICION DE FIRMA DE FUNCIONES ---*/

void construir_mensaje(t_mensaje_gameboy* argumentos, t_list *lista);

t_tipo_mensaje select_tipo_mensaje(char * valor);

void cargar_argumentos(t_mensaje_gameboy *argumentos_mensaje, t_list *lista);

bool validar_argumentos(t_mensaje_gameboy *argumentos_mensaje);

bool validar_tiempo(t_list *lista);

bool validar_coordXY(t_list *lista, int index1, int index2);

bool validar_id_mensaje(t_list *lista, int index);

bool validar_resultado_caught(t_list *lista, int index);

t_result_caught codificar_resultado_caught(char *valor);

int crear_conexion(t_mensaje_gameboy *msg_gameboy);

char* select_ip_proceso(t_mensaje_gameboy *msg_gameboy);

char* select_puerto_proceso(t_mensaje_gameboy *msg_gameboy);

char* obtengo_proceso(t_mensaje_gameboy *msg_gameboy);

char* obtengo_cola(t_mensaje_gameboy *msg_gameboy);

void enviar_mensaje(t_mensaje_gameboy *msg_gameboy, int socket_cliente);

void esperar_respuesta	(int socket_cliente);

void empaquetar_catch_broker	(t_mensaje_gameboy *msg_gameboy, t_paquete *paquete);

void empaquetar_caught_broker(t_mensaje_gameboy *msg_gameboy, t_paquete *paquete);

void empaquetar_catch_gamecard(t_mensaje_gameboy *msg_gameboy, t_paquete *paquete);

void empaquetar_new_broker(t_mensaje_gameboy *msg_gameboy, t_paquete *paquete);

void empaquetar_get_broker(t_mensaje_gameboy *msg_gameboy, t_paquete *paquete);

void empaquetar_appeared_broker(t_mensaje_gameboy *msg_gameboy, t_paquete *paquete);

void empaquetar_appeared_team(t_mensaje_gameboy *msg_gameboy, t_paquete *paquete);

void empaquetar_get_gamecard(t_mensaje_gameboy *msg_gameboy, t_paquete *paquete);

void* serializar_paquete(t_paquete* paquete, int *bytes);

t_list* recv_posiciones(int socket_cliente);

char* recibir_mensaje(int socket_cliente);

int recibir_op_code(int socket_cliente);

void* recibir_buffer(int* size, int socket_cliente);

void* recibir_stream(int* size, int socket_cliente);

void eliminar_paquete(t_paquete* paquete);

int tamano_paquete(t_paquete *paquete);

void borrar_comienzo(t_list* lista, int cant);

void iniciar_log(void);

void leer_config(char *path);

void terminar_programa(t_mensaje_gameboy *msg_gameboy,
		t_config_gameboy *config_gameboy, t_list *lista, t_log *log,
		t_config *config, int conexion);

void liberar_conexion(int socket_cliente);

#endif /* UTILS_H_ */
