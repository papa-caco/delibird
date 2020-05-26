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

#include <delibird/estructuras.h>
#include <delibird/serializaciones.h>
#include <delibird/conexiones.h>
#include <delibird/mensajeria.h>
#define MENSAJE_0 0
#define RUTA_CONFIG "config/gameboy.config"

/* ---  DEFINICION DE ESTRUCTURAS PROPIAS DEL GAMEBOY---*/

typedef struct Mensaje_Gameboy{
	t_proceso proceso;
	t_tipo_mensaje tipo_mensaje;
	t_list *argumentos;
} t_mensaje_gameboy;

typedef struct Configuracion_Gameboy{
	char *ip_broker;
	char *ip_gamecard;
	char *ip_team;
	char *puerto_broker;
	char *puerto_gamecard;
	char *puerto_team;
	int coord_x_max;
	int coord_y_max;
	int cant_max_pokemon;
	int time_suscrip_max;
	int id_mensaje_unico;
	int id_suscriptor;
	char *ruta_log;
} t_config_gameboy;

/* -----------DEFINICION DE VARIABLES GLOBALES --------*/

t_log 				*g_logger;

t_config 			*g_config;

t_config_gameboy 	*g_config_gameboy;

t_tipo_mensaje		g_suscript_queue;

int 				g_rcv_msg_qty;

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

int realizar_conexion(t_mensaje_gameboy *msg_gameboy);

int conexion_broker(void);

char* select_ip_proceso(t_mensaje_gameboy *msg_gameboy);

char* select_puerto_proceso(t_mensaje_gameboy *msg_gameboy);

char* obtengo_proceso(t_mensaje_gameboy *msg_gameboy);

char* obtengo_cola(t_mensaje_gameboy *msg_gameboy);

int get_time_suscripcion(t_mensaje_gameboy *msg_gameboy);

void enviar_mensaje_gameboy(t_mensaje_gameboy *msg_gameboy, int socket_cliente);

void send_msg_gameboy_suscriptor(t_mensaje_gameboy *msg_gameboy,int socket_cliente);

void enviar_msj_suscripcion_broker(t_tipo_mensaje tipo_mensaje, uint32_t id_recibido, int socket_cliente);

void esperar_rta_servidor(int socket_cliente);

void send_msg_catch_broker(t_mensaje_gameboy *msg_gameboy, int socket_cliente, t_log *logger);

void send_msg_caught_broker(t_mensaje_gameboy *msg_gameboy, int socket_cliente, t_log *logger);

void send_msg_catch_gamecard(t_mensaje_gameboy *msg_gameboy, int socket_cliente, t_log *logger);

void send_msg_new_broker(t_mensaje_gameboy *msg_gameboy, int socket_cliente, t_log *logger);

void send_msg_new_gamecard(t_mensaje_gameboy *msg_gameboy, int socket_cliente, t_log *logger);

void send_msg_get_broker(t_mensaje_gameboy *msg_gameboy, int socket_cliente, t_log *logger);

void send_msg_appeared_broker(t_mensaje_gameboy *msg_gameboy, int socket_cliente, t_log *logger);;

void send_msg_appeared_team(t_mensaje_gameboy *msg_gameboy, int socket_cliente, t_log *logger);;

void send_msg_get_gamecard(t_mensaje_gameboy *msg_gameboy, int socket_cliente, t_log *logger);

uint32_t rcv_mensaje_publisher(op_code codigo_operacion, int socket_cliente);

void borrar_comienzo_lista(t_list* lista, int cant);

void iniciar_log_gameboy(void);

void leer_config_gameboy(char *path);

void finalizar_gameboy(t_config_gameboy *config_gameboy, t_log *log, t_config *config);

#endif /* UTILS_H_ */
