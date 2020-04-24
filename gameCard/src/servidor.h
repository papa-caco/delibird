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



/*****  Definicion de variables globales   *****/

pthread_t thread;

/*****  definicion de funciones   *****/
void* recibir_buffer(int*, int);
int crear_conexion(void);
void iniciar_servidor(int* servidor);
void esperar_cliente(int);
void* recibir_mensaje(int socket_cliente, int* size);
int recibir_operacion(int);
void process_request(int cod_op, int cliente_fd);
void serve_client(int *socket);
void suscribirse( int socket_gamecard);
void* serializar_paquete(t_paquete* paquete, int bytes);
void devolver_mensaje(void* payload, int size, int socket_cliente);

void enviar_mensaje(char* mensaje, int socket_cliente);
char* recibir_mensaje_broken(int socket_cliente);
void* serializar_paquete_cliente(t_paquete* paquete, int *bytes);

//void iniciar_servidor2(void);


#endif /* SRC_SERVIDOR_H_ */
