#ifndef CONEXIONES_H_
#define CONEXIONES_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<commons/txt.h>
#include<commons/collections/list.h>
#include<string.h>
#include<pthread.h>
#include<stdbool.h>

#define IP "127.0.0.1"
#define PUERTO "5001"
#define ID_MSG_RTA 65535
#define RESPUESTA_OK "RECIBIDO_OK"
#define HANDSHAKE_SUSCRIPTOR 255

typedef enum Resultado_Caught{
	FAIL, OK,
} t_result_caught;

typedef enum Codigo_Operacion{
	ID_MENSAJE = 10,
	MSG_CONFIRMED,
	MSG_ERROR,
	NEW_BROKER = 20,
	APPEARED_BROKER,
	CATCH_BROKER,
	CAUGHT_BROKER,
	GET_BROKER,
	LOCALIZED_BROKER,
	NEW_GAMECARD = 40,
	CATCH_GAMECARD,
	GET_GAMECARD,
	APPEARED_TEAM = 80,
	CAUGHT_TEAM,
	LOCALIZED_TEAM,
	SUSCRIP_NEW = 120,
	SUSCRIP_APPEARED,
	SUSCRIP_CATCH,
	SUSCRIP_CAUGHT,
	SUSCRIP_GET,
	SUSCRIP_LOCALIZED,
	COLA_VACIA,
	FIN_SUSCRIPCION,
} op_code;

typedef struct Posicion_Pokemon{
	int pos_x;
	int pos_y;
	int cantidad;
} t_posicion_pokemon;

typedef struct Stream{
	int size;
	void* stream;
} t_stream;

typedef struct Paquete{
	op_code codigo_operacion;
	t_stream* buffer;
} t_paquete;

typedef struct socket_cliente{
	int cliente_fd;
	int cant_msg_enviados;
} t_socket_cliente;


//-----------------Variables Globales----------------------------

pthread_t thread;


t_log *g_logger;

//-----------------Firma de Funciones----------------------------

void* recibir_buffer(int*, int);

int recibir_operacion(int socket);

void iniciar_servidor(void);

void serve_client(t_socket_cliente *socket);

void process_request(int cod_op, t_socket_cliente *socket);

void* recibir_mensaje(int socket_cliente, int* size);

void* rcv_catch_broker(int socket_cliente, int* size);

void *rcv_caught_broker(int socket_cliente,int *size);

void* rcv_new_broker(int socket_cliente, int* size);

void* rcv_new_gamecard(int socket_cliente, int* size);

void* rcv_get_broker(int socket_cliente, int* size);

void *rcv_get_gamecard(int socket_cliente, int *size);

void* rcv_catch_gamecard(int socket_cliente, int* size);

void* rcv_appeared_broker(int socket_cliente, int* size);

void* rcv_appeared_team(int socket_cliente, int* size);

void *rcv_handshake_suscripcion(t_socket_cliente *socket, int *size);

void *rcv_fin_suscripcion(t_socket_cliente *socket, int *size);

void *serializar_paquete(t_paquete* paquete, int bytes);

void devolver_id_mensaje_propio(int socket_cliente);

void devolver_recepcion_ok(int socket_cliente);

void devolver_recepcion_fail(int socket_cliente, char* mensajeError);

void devolver_caught_broker(void *msg, int socket_cliente);

void devolver_appeared_broker(void *msg, int size, int socket_cliente);

void devolver_id_mensaje(void *msg,int socket_cliente);

void devolver_posiciones(int socket_cliente, char* pokemon, int* encontroPokemon); //Hace un send de la lista de posiciones y cantidad de un pokemon

void esperar_cliente(int socket);

int tamano_recibido(int bytes);

void eliminar_paquete (t_paquete *paquete);

void iniciar_logger(void);



#endif /* CONEXIONES_H_ */
