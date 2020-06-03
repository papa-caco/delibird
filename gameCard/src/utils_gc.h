/*
 * utils.h
 *
 *  Created on: 20 abr. 2020
 *      Author: utnso
 */
#ifndef UTILS_H_
#define UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<string.h>

#include<readline/readline.h>

#include<unistd.h>
/*
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<pthread.h>

*/
#include<commons/txt.h>

#include <delibird/estructuras.h>
#include <delibird/conexiones.h>
#include <delibird/mensajeria.h>
#include <delibird/serializaciones.h>


#include <sys/stat.h>


#define PATH_CONFIG "config/gameCard.config"
#define PATH_LOG "log/gameCard.log"


/*** ESTRUCTURAS **/
typedef struct Configuracion_GameCard
{
	char *ip_broker;
	char *puerto_broker;
	char *ip_gamecard;
	char *puerto_gamecard;
	char *path_pokemon; //PUNTO_MONTAJE_TALLGRASS
/*	int tiempo_reconexion;
	int retardo_ciclo_cpu;
	char *algoritmo_planificion;
	int quantum;
	int estimacion_inicial;
	char *ruta_log;
	int id_suscriptor;*/
} t_config_game_card;


typedef struct socket_cliente{
	int cliente_fd;
	int cant_msg_enviados;
} t_socket_cliente; //MODO_SERVIDOR



/* --- VARIABLES GLOBALES ---*/
// pthread_t thread;
t_config_game_card  *g_config_game_card;
t_log *g_logger;


/*** DEFINICION INTERFACES **/

void iniciar_game_card(void);
void inicio_server_game_card();
void iniciar_log_game_card();
void leer_config(void);

void process_request(op_code cod_op, int cliente_fd);
void *rcv_new_pokemon(int socket_cliente, int *size);
void devolver_appeared_pokemon(void *msg, int size, int socket_cliente);
void devolver_posiciones(int socket_cliente, char* pokemon,	int* encontroPokemon);
void* rcv_catch_pokemon(int socket_cliente, int *size);
void devolver_caught_pokemon(void *msg, int socket_cliente);
void *rcv_get_pokemon(int socket_cliente, int *size);
void devolver_recepcion_ok(int socket_cliente);
void devolver_recepcion_fail(int socket_cliente, char* mensajeError) ;
void liberar_lista_posiciones(t_list* lista);
void liberar_listas(char** lista);
void devolver_posiciones(int socket_cliente, char* pokemon,	int* encontroPokemon);
int tamano_recibido(int bytes);
void verificarPokemon(char* pathPokemon,t_posicion_pokemon* posicion);
char* obtengo_cola_mensaje(int codigo_operacion);
void enviar_mensaje_a_broker(t_paquete* paquete,int bytes);
#endif
