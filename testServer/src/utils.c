/*
 * conexiones.c
 *
 *  Created on: 3 mar. 2019
 *      Author: utnso
 */

#include"utils.h"

void iniciar_servidor(void) {

	int socket_servidor;

	struct addrinfo hints, *servinfo, *p; 	//hints no es puntero

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;	// No importa si uso IPv4 o IPv6 - vale 0
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP
	hints.ai_flags = AI_PASSIVE;// Asigna el address del localhost: 127.0.0.1

	getaddrinfo(IP, PUERTO, &hints, &servinfo);

	log_info(g_logger, "Direccion: %s, Port: %s", IP, PUERTO);


	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((socket_servidor = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1)
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
		esperar_cliente(socket_servidor);
}

void esperar_cliente(int socket_servidor) {
	struct sockaddr_in dir_cliente;

	socklen_t tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente,
			&tam_direccion);

	pthread_create(&thread, NULL, (void*) serve_client, &socket_cliente);
	pthread_detach(thread);

}

void serve_client(int* socket) {
	int cod_op;
	if (recv(*socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		cod_op = -1;
	process_request(cod_op, *socket);
}

void process_request(int cod_op, int cliente_fd) {
	int size;
	void* msg;
	switch (cod_op) {
	case ID_MENSAJE:
		break;
	case CATCH_BROKER:		 // Tiene que devolver un ID_MENSAJE propio
		log_info(g_logger, "(NEW-MESSAGE: BROKER@CATCH_POKEMON | Socket#: %d", cliente_fd);
		msg = rcv_catch_broker(cliente_fd, &size);
		arrojar_id_mensaje(cliente_fd);
		break;
	case CATCH_GAMECARD:
		log_info(g_logger,
				"(NEW-MESSAGE @GAMECARD | CATCH_POKEMON | Socket_Cliente: %d",
				cliente_fd);
		//TODO
		break;
	case CAUGHT_BROKER:
		log_info(g_logger, "(NEW-MESSAGE @");
		//TODO
		break;
	case GET_BROKER:
		log_info(g_logger, "(NEW-MESSAGE @: BROKER@GET_POKEMON | SOCKET#: %d",
				cliente_fd);
		msg = rcv_get_broker(cliente_fd, &size);
		break;
	case GET_GAMECARD:
		log_info(g_logger, "(NEW-MESSAGE @");
		//TODO
		break;
	case NEW_BROKER:
		log_info(g_logger,
				"(NEW-MESSAGE @BROKER | NEW_POKEMON | Socket_Cliente: %d",
				cliente_fd);
		msg = rcv_new_broker(cliente_fd, &size);
		arrojar_id_mensaje(cliente_fd); // Tiene que devolver todas las posiciones del pokemon
		break;
	case NEW_GAMECARD:
		log_info(g_logger, "(NEW-MESSAGE @");
		//TODO
		break;
	case APPEARED_BROKER:
		log_info(g_logger, "(NEW-MESSAGE @");
		//TODO
		break;
	case APPEARED_TEAM:
		log_info(g_logger, "(NEW-MESSAGE @");
		//TODO
		break;
	case 0:
		pthread_exit(NULL);
	case -1:
		pthread_exit(NULL);
	}
	free(msg);
}

void* recibir_mensaje(int socket_cliente, int* size) {
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);
	log_info(g_logger, "Recibi del cliente Socket: %d el mensaje: %s",
			socket_cliente, buffer);
	return buffer;
}

void* rcv_catch_broker(int socket_cliente, int *size) {
	void *msg;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	msg = malloc(*size);
	recv(socket_cliente, msg, *size, MSG_WAITALL);

	int offset = 0;
	int *pos_x = msg + offset;
	offset += sizeof(int);
	int *pos_y = msg + offset;
	offset += sizeof(int);
	char *pokemon = msg + offset;

	log_info(g_logger, "(MSG-BODY= %s | %d | %d -- SIZE = %d Bytes)", pokemon,
			*pos_x, *pos_y, *size);

	return msg;
}

void* rcv_new_broker(int socket_cliente, int *size) {

	void *msg;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	msg = malloc(*size);
	recv(socket_cliente, msg, *size, MSG_WAITALL);

	int offset = 0;
	int *pos_x = msg + offset;
	offset += sizeof(int);
	int *pos_y = msg + offset;
	offset += sizeof(int);
	int *cantidad = msg + offset;
	offset += sizeof(int);
	char*pokemon = msg + offset;

	log_info(g_logger, "(MSG-BODY= %s | %d | %d | %d -- SIZE = %d Bytes)",
			pokemon, *pos_x, *pos_y, *cantidad, *size);

	return msg;

}

void* rcv_get_broker(int socket_cliente, int *size) {

	void *msg;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	msg = malloc(*size);
	recv(socket_cliente, msg, *size, MSG_WAITALL);

	char*pokemon = msg;

	log_info(g_logger, "(MSG-BODY= %s -- SIZE = %d Bytes)", pokemon, *size);

	return msg;
}

void* serializar_paquete(t_paquete* paquete, int bytes) {
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream,
			paquete->buffer->size);
	desplazamiento += paquete->buffer->size;

	return magic;
}

void arrojar_id_mensaje(int socket_cliente) {
	t_paquete* paquete = malloc(sizeof(t_paquete));
	int id_mensaje = ID_MSG_RTA;
	log_info(g_logger,"id_rta %d", id_mensaje);

	paquete->codigo_operacion = ID_MENSAJE;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = sizeof(int);
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, &(id_mensaje), paquete->buffer->size);

	int bytes = paquete->buffer->size + 2 * sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

void eliminar_paquete(t_paquete *paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void iniciar_logger(void) {
	g_logger = log_create("/home/utnso/logs/server.log","SERVER", 1, LOG_LEVEL_INFO);
}
