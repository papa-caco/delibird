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

#define IP "127.0.0.1"
#define PUERTO "5001"
#define ID_MSG_RTA 65535

typedef enum {
	ID_MENSAJE = 10,
	APPEARED_BROKER,
	APPEARED_TEAM,
	CATCH_BROKER,
	CATCH_GAMECARD,
	CAUGHT_BROKER,
	GET_BROKER,
	GET_GAMECARD,
	NEW_BROKER,
	NEW_GAMECARD,
} op_code;

typedef struct {
	char* nombrePokemon;
	int posicionX;
	int posicionY;
	int cantidad;

} t_posicion_pokemon;

typedef struct {
	int size;
	void* stream;
} t_stream;

typedef struct {
	op_code codigo_operacion;
	t_stream* buffer;
} t_paquete;


//-----------------Variables Globales----------------------------

pthread_t thread;


t_log *g_logger;

//-----------------Firma de Funciones----------------------------

void* recibir_buffer(int*, int);

int recibir_operacion(int socket);

void iniciar_servidor(void);

void serve_client(int *socket);

void process_request(int cod_op, int cliente_fd);

void* recibir_mensaje(int socket_cliente, int* size);

void* rcv_catch_broker(int socket_cliente, int* size);

void* rcv_new_broker(int socket_cliente, int* size);

void* rcv_get_broker(int socket_cliente, int* size);

void* rcv_catch_gamecard(int socket_cliente, int* size);

void* serializar_paquete(t_paquete* paquete, int bytes);

void devolver_id_mensaje_propio(int socket_cliente);

void send_posiciones(int socket_cliente, char* pokemon); //Hace un send de la lista de posiciones y cantidad de un pokemon

void esperar_cliente(int socket);

void eliminar_paquete (t_paquete *paquete);

void iniciar_logger(void);



#endif /* CONEXIONES_H_ */
