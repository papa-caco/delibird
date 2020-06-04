/*
 * utils.h
 *
 *  Created on: 20 abr. 2020
 *      Author: utnso
 */
#ifndef UTILS_H_
#define UTILS_H_
/*
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<string.h>

#include<readline/readline.h>

#include<unistd.h>

#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<pthread.h>

*/

#include <delibird/estructuras.h>
#include <delibird/conexiones.h>
#include <delibird/mensajeria.h>
#include <delibird/serializaciones.h>

#include<commons/txt.h>
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
	int id_suscriptor;
	int tiempo_reconexion;
} t_config_gamecard;


typedef struct socket_cliente{
	int cliente_fd;
	int cant_msg_enviados;
} t_socket_cliente; //MODO_SERVIDOR



/* --- VARIABLES GLOBALES ---*/
// pthread_t thread;

t_config_gamecard  *g_config_gc;
t_log *g_logger;
t_config* g_config;
sem_t sem_mutex_msjs;
bool status_conn_broker;
pthread_t tid;

/*** DEFINICION INTERFACES **/

void iniciar_gamecard(void);
void inicio_server_gamecard();
void iniciar_log_gamecard();
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
