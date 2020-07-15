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
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<commons/collections/list.h>
#include<commons/txt.h>
#include<readline/readline.h>

#include<unistd.h>

#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<pthread.h>


#include <delibird/estructuras.h>
#include <delibird/conexiones.h>
#include <delibird/mensajeria.h>
#include <delibird/serializaciones.h>


// #define RESPUESTA_OK "RECIBIDO_OK"
#include <sys/stat.h>
//#include <unistd.h>

#define PATH_CONFIG "config/gameCard.config"
#define PATH_LOG "config/gameCard.log"


/*** ESTRUCTURAS *
typedef struct Configuracion_GameCard
{
	char *ip_broker;
	char *puerto_broker;
	char *ip_game_card;
	char *puerto_game_card;
/*	int tiempo_reconexion;
	int retardo_ciclo_cpu;
	char *algoritmo_planificion;
	int quantum;
	int estimacion_inicial;
	char *ruta_log;
	int id_suscriptor;
} t_config_game_card;


/*
typedef enum Codigo_Operacion{
	ID_MENSAJE = 10,
	MSG_CONFIRMED,
	MSG_ERROR,
	FIN_SUSCRIPCION,
	NEW_BROKER = 20,
	APPEARED_BROKER,
	CAUGHT_BROKER,
	GET_BROKER = 24,
	LOCALIZED_BROKER,
	NEW_POKEMON=40,   //La corresponde con NEW_GAMECARD , del testServer
	CATCH_POKEMON=41, // Se corresponde con CATCH_GAMECARD, del testServer
	GET_POKEMON = 42, // Se corresponde con GET_GAMECARD, del testServer
	APPEARED_TEAM = 80,
	CAUGHT_TEAM,
	LOCALIZED_TEAM,
	SUSCRIP_NEW = 120,
	SUSCRIP_APPEARED,
	SUSCRIP_CATCH,
	SUSCRIP_CAUGHT,
	SUSCRIP_GET,
	SUSCRIP_LOCALIZED,
	SUSCRIP_END,
	COLA_VACIA,
} op_code;  //MODO_SERVIDOR
*/
/*ID_MENSAJE = 10,
	MSG_CONFIRMED,
	MSG_ERROR,
	APPEARED_BROKER=21,//TERMPORAL MIENTRAS USARMOS TEST SERVER
	CAUGHT_BROKER=23,
	NEW_POKEMON=40,   //NEW_GAMECARD = 40,
	APPEARED_POKEMON,
	CATCH_POKEMON,    //CATCH_GAMECARD,
	CAUGHT_POKEMON,
	GET_POKEMON,
	LOCALIZED_POKEMON,*/
/*
typedef enum Tipo_Mensaje{
	UNKNOWN_QUEUE = 9,
	//NEW_POKEMON,
	APPEARED_POKEMON = 11,
	//CATCH_POKEMON,
	CAUGHT_POKEMON = 13,
	//GET_POKEMON,
	LOCALIZED_POKEMON = 15,
} t_tipo_mensaje;


typedef struct socket_cliente{
	int cliente_fd;
	int cant_msg_enviados;
} t_socket_cliente; //MODO_SERVIDOR

/*
typedef struct Stream{
	int size;
	void* data;
} t_stream; //MODO_SERVIDOR
*/

/*
typedef struct
{
	op_code codigo_operacion;
	t_stream* buffer;
} t_paquete;//MODO_SERVIDOR
*/
/*
typedef struct Posicion_Pokemon{
	int pos_x;
	int pos_y;
	int cantidad;
} t_posicion_pokemon;//MODO_SERVIDOR
*/
/*
typedef enum Resultado_Caught{
	FAIL, OK,
} t_result_caught;
*/
//



/* ---  DEFINICION DE VARIABLES GLOBALES ---
pthread_t thread;
t_log *g_logger;



/* ---  DEFINICION DE LAS INTERFACES ---
t_config* leer_config(void);
void iniciar_log(void);
void finalizar_log(void);
void destruir_config(void);

//void suscribirse(int conexion);
//t_config* config = leer_config();
//

/*** DEFINICION DE FUNCIONES *


void inicio_server_game_card();
void iniciar_log_game_card();

//// MODO SERVIDOR
//void esperar_cliente(int);

//void serve_client(t_socket_cliente *socket);

void process_request(op_code cod_op, t_socket_cliente *socket);

void* serializar_paquete(t_paquete* paquete, int bytes);

//void *rcv_new_pokemon(int socket_cliente, int *size);
void devolver_appeared_pokemon(void *msg, int size, int socket_cliente);

void* rcv_catch_pokemon(int socket_cliente, int *size);
void devolver_caught_pokemon(void *msg, int socket_cliente);

void *rcv_get_pokemon(int socket_cliente, int *size);
void devolver_posiciones(int socket_cliente, char* pokemon,	int* encontroPokemon);

void devolver_recepcion_ok(int socket_cliente);
void devolver_recepcion_fail(int socket_cliente, char* mensajeError) ;

void enviar_mensaje_a_broker(t_paquete* paquete, int bytes);

void liberar_listas(char** lista);

void liberar_lista_posiciones(t_list* list);


void eliminar_paquete(t_paquete* paquete) ;

int tamano_recibido(int bytes);

void verificarPokemon(char* pathPokemon,t_posicion_pokemon* posicion);

char *obtengo_cola_mensaje(int cod_op);

//// FIN MODO SERVIDOR
#endif /* UTILS_H_ */
