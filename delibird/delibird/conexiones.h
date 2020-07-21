/*
 * coexiones.h
 *
 *  Created on: 2 may. 2020
 *      Author: Los-Que-Aprueban
 *
 */

#ifndef SRC_CONEXIONES_H_
#define SRC_CONEXIONES_H_
#include "estructuras.h"


//-------------------Firma de funciones - Servidores ---------------------------//

void iniciar_server_broker(char *ip, char *puerto, t_log* logger);

void atender_cliente_broker(t_socket_cliente_broker *socket);

void iniciar_servidor(char *ip, char *puerto, t_log* logger);

void iniciar_servidor_gc(char *ip, char *puerto, pthread_mutex_t mutex_clientes, t_log* logger);

void esperar_cliente(int socket_servidor, pthread_t thread);

void serve_client(int *cliente_fd);

void atender_gameboy(int *cliente_fd);

void atender_gameboy_gc(int *cliente_fd);

//-------------------Firma de funciones - Clientes ---------------------------//

int crear_conexion(char *ip, char *puerto, t_log *logger, char *proceso, char *cola);

int *crear_conexion_broker(char *ip, char *puerto, t_log *logger, char *proceso, char *cola);


#endif /* SRC_CONEXIONES_H_ */
