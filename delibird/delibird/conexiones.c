/*
 * conexiones.c
 *
 *  Created on: 2 may. 2020
 *      Author: Los-Que-Aprueban

 */
#include "conexiones.h"

// -------------Funciones Para Servidores----------------------------------------//

void iniciar_server_broker(char *ip, char *puerto, t_log* logger, pthread_t thread)
{

	int socket_servidor;
	struct addrinfo hints, *servinfo, *p; 	//hints no es puntero
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;			// No importa si uso IPv4 o IPv6 - vale 0
	hints.ai_socktype = SOCK_STREAM;		// Indica que usaremos el protocolo TCP
	hints.ai_flags = AI_PASSIVE;			// Asigna el address del localhost: 127.0.0.1

	getaddrinfo(ip, puerto, &hints, &servinfo);
	log_info(logger, "Esperando conexiones en Direccion: %s, Puerto: %s", ip, puerto);

	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
			continue;
		if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
			close(socket_servidor);
			continue;
		}
		break;
	}
	listen(socket_servidor, SOMAXCONN);	// Maximum queue length specifiable by listen = 128 (default)
	freeaddrinfo(servinfo);
	while (1)
		esperar_cliente_broker(socket_servidor, thread);
}

void esperar_cliente_broker(int socket_servidor, pthread_t thread)
{
	struct sockaddr_in dir_cliente;
	socklen_t tam_direccion = sizeof(struct sockaddr_in);
	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);
	t_socket_cliente_broker *socket = malloc(sizeof(t_socket_cliente_broker));
	socket->cliente_fd = socket_cliente;
	// inicializa contador de mensajes enviados al cliente que se conectó.
	socket->cant_msg_enviados = 0;
	pthread_create(&thread, NULL, (void*) atender_cliente_broker,(void*) socket);
	pthread_detach(thread);
}


void iniciar_servidor(char *ip, char *puerto, t_log* logger, pthread_t thread)
{

	int socket_servidor;
	struct addrinfo hints, *servinfo, *p; 	//hints no es puntero
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;			// No importa si uso IPv4 o IPv6 - vale 0
	hints.ai_socktype = SOCK_STREAM;		// Indica que usaremos el protocolo TCP
	hints.ai_flags = AI_PASSIVE;			// Asigna el address del localhost: 127.0.0.1

	getaddrinfo(ip, puerto, &hints, &servinfo);
	log_info(logger, "Esperando conexiones en Direccion: %s, Puerto: %s", ip, puerto);

	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
			continue;
		if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
			close(socket_servidor);
			continue;
		}
		break;
	}
	listen(socket_servidor, SOMAXCONN);	// Maximum queue length specifiable by listen = 128 (default)
	freeaddrinfo(servinfo);
	while (1)
		esperar_cliente(socket_servidor, thread);
}

void esperar_cliente(int socket_servidor, pthread_t thread)
{
	struct sockaddr_in dir_cliente;
	socklen_t tam_direccion = sizeof(struct sockaddr_in);
	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);
	pthread_create(&thread, NULL, (void*) serve_client, &socket_cliente);
	pthread_detach(thread);
}

int crear_conexion(char *ip, char *puerto, t_log *logger, char *proceso, char *cola)
{
	struct addrinfo hints;
	struct addrinfo *server_info;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);
	int socket_cliente = socket(server_info->ai_family,
			server_info->ai_socktype, server_info->ai_protocol);
	int conexion = connect(socket_cliente, server_info->ai_addr,
			server_info->ai_addrlen);
	if (conexion < 0) {
		log_error(logger,"(CONN_FAILED:PROCESS: %s | QUEUE: %s | IP: %s | PORT: %s)", proceso, cola, ip, puerto);
	}
	else {
		log_info(logger,"(SUCCESS_CONN - PROCESS: %s | QUEUE: %s | IP: %s | PORT: %s)", proceso, cola, ip, puerto);
	}
	freeaddrinfo(server_info);
	return socket_cliente;
}

void atender_cliente_broker(t_socket_cliente_broker *socket)
{

}

void serve_client(int *cliente_fd)
{

}
