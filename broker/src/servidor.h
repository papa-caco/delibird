/*
 * servidor.h
 *
 *  Created on: 21 abr. 2020
 *      Author: utnso
 */

#ifndef SRC_SERVIDOR_H_
#define SRC_SERVIDOR_H_
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<string.h>
#include<pthread.h>



typedef enum
{
	SUSCRIBE,
	MENSAJE,
	ID_MENSAJE,
} op_code; //codigo de operacion


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

/*****  Definicion de variables globales   *****/

pthread_t thread;

/*****  definicion de funciones   *****/
void* recibir_buffer(int*, int);

void iniciar_servidor(void);
void esperar_cliente(int);
void* recibir_mensaje(int socket_cliente, int* size);
int recibir_operacion(int);
void process_request(int cod_op, int cliente_fd);
void serve_client(int *socket);
void* serializar_paquete(t_paquete* paquete, int bytes);
void devolver_mensaje(void* payload, int size, int socket_cliente);
void suscribir_proceso(int cliente);

void* procesar_mensaje(int socket_cliente ,int* idMensaje, int* sizeMensaje);
void devolver_id_mensaje(int idMensaje,  int socket_cliente);
void enviar_mensaje_a_suscriptores(void* payload, int size);
//void iniciar_servidor2(void);


#endif /* SRC_SERVIDOR_H_ */
