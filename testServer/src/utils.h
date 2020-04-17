#ifndef CONEXIONES_H_
#define CONEXIONES_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<string.h>
#include<pthread.h>

#define IP "127.0.0.1"
#define PUERTO "5001"

typedef enum
{
	MENSAJE = 10,
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

typedef struct
{
	int size;
	void* stream;
} t_stream;

typedef struct
{
	op_code codigo_operacion;
	t_stream* buffer;
} t_paquete;

// Variable Global
pthread_t thread;

//-----------------Variables Globales----------------------------

t_log *g_logger;

//-----------------Firma de Funciones----------------------------

void*	recibir_buffer		(int*, int);

int 	recibir_operacion	(int socket);

void 	iniciar_servidor	(void);

void 	serve_client		(int *socket);

void 	process_request		(int cod_op, int cliente_fd);

void* 	recibir_mensaje		(int socket_cliente, int* size);

void* 	rcv_catch_broker	(int socket_cliente, int* size);

void* 	serializar_paquete	(t_paquete* paquete, int bytes);

void 	devolver_mensaje	(void* payload, int size, int socket_cliente);	// hace un send

void 	esperar_cliente		(int socket);

void 	iniciar_logger		(void);

#endif /* CONEXIONES_H_ */
