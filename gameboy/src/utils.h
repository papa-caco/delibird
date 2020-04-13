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
	MENSAJE = 1,
} op_code;

typedef enum
{
	UNKNOWN_PROC = 9,
	SUSCRIPTOR,
	GAMECARD,
	BROKER,
	TEAM,
	GAMEBOY,
} t_proceso;

typedef enum
{
	UNKNOWN_QUEUE = 9,
	NEW_POKEMON,
	APPEARED_POKEMON,
	CATCH_POKEMON,
	CAUGHT_POKEMON,
	GET_POKEMON,
	LOCALIZED_POKEMON,
} t_tipo_mensaje;

typedef enum
{
	FAIL,
	OK,
} t_result_caught;

typedef struct
{
	t_proceso proceso;
	t_tipo_mensaje tipo_mensaje;
	t_list *argumentos;
} t_mensaje_gameboy;

typedef struct
{
char *ip_broker;
char *ip_gamecard;
char *ip_team;
char *puerto_broker;
char *puerto_gamecard;
char *puerto_team;
int coord_x_max;
int coord_y_max;
int cant_max_pokemon;
} t_config_gameboy;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

/* ---  DEFINICION DE VARIABLES GLOBALES ---*/

t_log 				*g_logger;
t_config			*g_config;
t_config_gameboy	*g_config_gameboy;


/* ---  DEFINICION DE FIRMA DE FUNCIONES ---*/

void	construir_mensaje	(t_mensaje_gameboy* argumentos, t_list *lista);

t_tipo_mensaje 	select_tipo_mensaje	(char * valor);

void 	cargar_argumentos	(t_mensaje_gameboy *argumentos_mensaje, t_list *lista);

bool 	validar_argumentos	(t_mensaje_gameboy *argumentos_mensaje);

bool 	validar_tiempo		(t_list *lista);

bool	validar_coordXY		(t_list *lista,int index1, int index2);

bool 	validar_id_mensaje	(t_list *lista,int index);

bool 	validar_resultado_caught	(t_list *lista,int index);

int 	crear_conexion		(t_mensaje_gameboy *msg_gameboy);

char*	select_ip_proceso	(t_mensaje_gameboy *msg_gameboy);

char*	select_puerto_proceso	(t_mensaje_gameboy *msg_gameboy);

char*	obtengo_proceso		(t_mensaje_gameboy *msg_gameboy);

char*	obtengo_cola		(t_mensaje_gameboy *msg_gameboy);

void 	enviar_mensaje		(char* mensaje, int socket_cliente);

void* 	serializar_paquete	(t_paquete* paquete, int *bytes);

char* 	recibir_mensaje		(int socket_cliente);

int 	recibir_op_code		(int socket_cliente);

void* 	recibir_buffer		(int* size, int socket_cliente);

void* 	recibir_stream		(int* size, int socket_cliente);

void 	eliminar_paquete	(t_paquete* paquete);

void 	borrar_comienzo		(t_list* lista, int cant);

void 	iniciar_log			(void);

void 	leer_config			(char *path);

void 	liberar_conexion	(int socket_cliente);

#endif /* UTILS_H_ */
