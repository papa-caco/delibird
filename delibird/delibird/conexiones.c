/*
 * conexiones.c
 *
 *  Created on: 2 may. 2020
 *      Author: Los-Que-Aprueban

 */
#include "conexiones.h"

// -------------Funciones Para Servidores----------------------------------------//

void iniciar_server_broker(char *ip, char *puerto, t_log* logger)
{
	int socket_servidor, status;
	pthread_mutex_t mutex_clientes;
	pthread_mutex_init(&mutex_clientes, NULL);
	struct sockaddr_storage dir_cliente;
	socklen_t tam_direccion;
	struct addrinfo hints, *servinfo; 	//hints no es puntero
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;			// No importa si uso IPv4 o IPv6 - vale 0
	hints.ai_socktype = SOCK_STREAM;		// Indica que usaremos el protocolo TCP
	hints.ai_flags = AI_PASSIVE;			// Asigna el address del localhost: 127.0.0.1

	getaddrinfo(ip, puerto, &hints, &servinfo);

	if ((status = getaddrinfo(NULL, puerto, &hints, &servinfo)) != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
			exit(EXIT_FAILURE);
	}
	log_trace(logger, "(Esperando conexiones en Direccion: %s, Puerto: %s)", ip, puerto);
	socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	int yes = 1;
	setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);

	if (listen(socket_servidor, 10) == -1) {
		perror("listen");
	}

	freeaddrinfo(servinfo);

	while (1){
		tam_direccion = sizeof(dir_cliente);
		pthread_mutex_lock(&mutex_clientes);
		int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);
		pthread_t pid;
		log_info(logger,"(NEW CLIENT CONNECTED | SOCKET#:%d)", socket_cliente);
		t_socket_cliente_broker *socket = malloc(sizeof(t_socket_cliente_broker));
		socket->cliente_fd = socket_cliente;
		// inicializa contador de mensajes enviados al cliente que se conectó.
		socket->cant_msg_enviados = 0;
		int thread_status = pthread_create(&pid, NULL, (void*) atender_cliente_broker,(void*) socket);
		pthread_mutex_unlock(&mutex_clientes);
		if( thread_status != 0 ){
			log_error(logger, "Thread create returno %d", thread_status );
			log_error(logger, "Thread create returno %s", strerror( thread_status ) );
		} else {
			pthread_detach( pid );
		}
	}
}

void esperar_cliente_broker(int socket_servidor,t_log *logger, pthread_t thread)
{
	struct sockaddr_in dir_cliente;
	socklen_t tam_direccion = sizeof(struct sockaddr_in);
	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);
	log_info(logger,"(NEW CLIENT CONNECTED | SOCKET#:%d)", socket_cliente);
	t_socket_cliente_broker *socket = malloc(sizeof(t_socket_cliente_broker));
	socket->cliente_fd = socket_cliente;
	// inicializa contador de mensajes enviados al cliente que se conectó.
	socket->cant_msg_enviados = 0;
	pthread_create(&thread, NULL, (void*) atender_cliente_broker,(void*) socket);
	pthread_detach(thread);
}

void iniciar_servidor(char *ip, char *puerto, t_log* logger)
{
	int socket_servidor = 0, status = 0;
	pthread_mutex_t mutex_clientes;
	pthread_mutex_init(&mutex_clientes, NULL);
	struct sockaddr_storage dir_cliente;
	socklen_t tam_direccion;
	struct addrinfo hints, *servinfo; 	//hints no es puntero
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;			// No importa si uso IPv4 o IPv6 - vale 0
	hints.ai_socktype = SOCK_STREAM;		// Indica que usaremos el protocolo TCP
	hints.ai_flags = AI_PASSIVE;			// Asigna el address del localhost: 127.0.0.1

	getaddrinfo(ip, puerto, &hints, &servinfo);

	if ((status = getaddrinfo(NULL, puerto, &hints, &servinfo)) != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
			exit(EXIT_FAILURE);
	}
	log_debug(logger, "Esperando conexiones en Direccion: %s, Puerto: %s", ip, puerto);
	socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	int yes = 1;
	setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);

	if (listen(socket_servidor, 10) == -1) {
		perror("listen");
	}
	freeaddrinfo(servinfo);
	while (1){
		tam_direccion = sizeof(dir_cliente);
		pthread_mutex_lock(&mutex_clientes);
		int *socket_cliente = malloc(sizeof(int));
		*socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);
		pthread_t pid;
		log_info(logger,"(NEW CLIENT CONNECTED | SOCKET#:%d)", *socket_cliente);
		int thread_status = pthread_create(&pid,NULL,(void*) atender_gameboy_gc,(int*) socket_cliente);
		pthread_mutex_unlock(&mutex_clientes);
		if( thread_status != 0 ){
			log_error(logger, "Thread create returno %d", thread_status );
			log_error(logger, "Thread create returno %s", strerror( thread_status ) );
		} else {
			pthread_detach(pid);
		}
	}
}

void iniciar_servidor_gc(char *ip, char *puerto, pthread_mutex_t mutex_clientes, t_log* logger)
{
	int socket_servidor = 0, status = 0;
	struct sockaddr_storage dir_cliente;
	socklen_t tam_direccion;
	struct addrinfo hints, *servinfo; 	//hints no es puntero
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;			// No importa si uso IPv4 o IPv6 - vale 0
	hints.ai_socktype = SOCK_STREAM;		// Indica que usaremos el protocolo TCP
	hints.ai_flags = AI_PASSIVE;			// Asigna el address del localhost: 127.0.0.1

	getaddrinfo(ip, puerto, &hints, &servinfo);

	if ((status = getaddrinfo(NULL, puerto, &hints, &servinfo)) != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
			exit(EXIT_FAILURE);
	}
	log_debug(logger, "Esperando conexiones en Direccion: %s, Puerto: %s", ip, puerto);
	socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	int yes = 1;
	setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);

	if (listen(socket_servidor, 10) == -1) {
		perror("listen");
	}
	freeaddrinfo(servinfo);
	while (1){
		tam_direccion = sizeof(dir_cliente);
		int *socket_cliente = malloc(sizeof(int));
		*socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);
		pthread_mutex_lock(&mutex_clientes);
		pthread_t pid;
		//log_info(logger,"(NEW CLIENT CONNECTED | SOCKET#:%d)", *socket_cliente); // -->> es buche <<--
		int thread_status = pthread_create(&pid,NULL,(void*) atender_gameboy_gc,(int*) socket_cliente);
		pthread_mutex_unlock(&mutex_clientes);
		if( thread_status != 0 ){
			log_error(logger, "Thread create returno %d", thread_status );
			log_error(logger, "Thread create returno %s", strerror( thread_status ) );
		} else {
			pthread_detach(pid);
		}
	}
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
	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
	int conexion = connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);
	if (conexion < 0) {
		log_error(logger,"(CONN_FAILED:PROCESS: %s | QUEUE: %s | IP: %s | PORT: %s)", proceso, cola, ip, puerto);
		socket_cliente = conexion;
	}
	else {
		log_info(logger,"(SUCCESS_CONN - PROCESS: %s | SOCKET#: %d| QUEUE: %s | IP: %s | PORT: %s)", proceso, socket_cliente, cola, ip, puerto);
	}
	freeaddrinfo(server_info);
	return socket_cliente;
}

int *crear_conexion_broker(char *ip, char *puerto, t_log *logger, char *proceso, char *cola)
{
	struct addrinfo hints;
	struct addrinfo *server_info;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	int *socket_cliente = malloc(sizeof(int));

	getaddrinfo(ip, puerto, &hints, &server_info);
	(*socket_cliente) = socket(AF_INET, SOCK_STREAM, 0);
	int conexion = connect(*socket_cliente, server_info->ai_addr, server_info->ai_addrlen);
	if (conexion < 0) {
		log_error(logger,"(CONN_FAILED:PROCESS: %s | QUEUE: %s | IP: %s | PORT: %s)", proceso, cola, ip, puerto);
		(*socket_cliente) = conexion;
	}
	else {
		log_info(logger,"(SUCCESS_CONN - PROCESS: %s | SOCKET#: %d| QUEUE: %s | IP: %s | PORT: %s)",
				proceso, *socket_cliente, cola, ip, puerto);
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

void atender_gameboy(int *cliente_fd)
{

}

void atender_gameboy_gc(int *cliente_fd)
{

}
