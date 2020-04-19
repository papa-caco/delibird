/*
 * conexiones.c
 *
 *  Created on: 3 mar. 2019
 *      Author: utnso
 */

#include"utils.h"

void iniciar_servidor(void) {
	int socket_servidor;

	iniciar_logger();

	struct addrinfo hints, *servinfo, *p; 	//hints no es puntero

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;	// No importa si uso IPv4 o IPv6 - vale 0
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP
	hints.ai_flags = AI_PASSIVE;// Asigna el address del localhost: 127.0.0.1

	getaddrinfo(IP, PUERTO, &hints, &servinfo);

	log_info(g_logger, "Direccion: %s, Port: %s", IP, PUERTO);
	log_destroy(g_logger);

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
	iniciar_logger();
	switch (cod_op) {
	case MENSAJE:
		msg = recibir_mensaje(cliente_fd, &size);
		devolver_mensaje(msg, size, cliente_fd);
		free(msg);
		break;
	case CATCH_BROKER:
		log_info(g_logger, "(NEW-MESSAGE: BROKER@CATCH_POKEMON | Socket#: %d",
				cliente_fd);
		msg = rcv_catch_broker(cliente_fd, &size);
		devolver_mensaje(msg, size, cliente_fd); // Tiene que devolver un Id Mensaje (un int)
		free(msg);
		break;
	case CATCH_GAMECARD:
		log_info(g_logger,
				"(NEW-MESSAGE @GAMECARD | CATCH_POKEMON | Socket_Cliente: %d",
				cliente_fd);
		rcv_catch_gamecard(cliente_fd, &size);
		break;
	case CAUGHT_BROKER:
		log_info(g_logger, "(NEW-MESSAGE @");
		//TODO
		break;
	case GET_BROKER:
		log_info(g_logger, "(NEW-MESSAGE @: BROKER@GET_POKEMON | SOCKET#: %d",
				cliente_fd);
		msg = rcv_get_broker(cliente_fd, &size);
		send_posiciones(cliente_fd, (char*) msg);
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
		devolver_mensaje(msg, size, cliente_fd); // Tiene que devolver todas las posiciones del pokemon
		break;
	case NEW_GAMECARD:
		log_info(g_logger, "(NEW-MESSAGE @");
		//TODO
		break;
	case APPEARED_BROKER:
		log_info(g_logger,"ENTRO A APPEARED" );
		log_info(g_logger,
				"(NEW-MESSAGE @BROKER | APPEARED_POKEMON | Socket_Cliente: %d",
				cliente_fd);
		msg = rcv_appeared_broker(cliente_fd, &size);
		devolver_mensaje(msg, size, cliente_fd); //PRUEBA
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
	log_destroy(g_logger);
}

void* recibir_mensaje(int socket_cliente, int* size) {
	iniciar_logger();
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);
	log_info(g_logger, "Recibi del cliente Socket: %d el mensaje: %s",
			socket_cliente, buffer);
	log_destroy(g_logger);
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

void* rcv_catch_gamecard(int socket_cliente, int *size) {

	void *msg;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	msg = malloc(*size);
	recv(socket_cliente, msg, *size, MSG_WAITALL);

	int offset = 0;
	int *idUnico = msg + offset;
	offset += sizeof(int);

	int *pos_x = msg + offset;
	offset += sizeof(int);

	int *pos_y = msg + offset;
	offset += sizeof(int);

	char *pokemon = msg + offset;

	log_info(g_logger, "(MSG-BODY= %s | %d | %d | %d -- SIZE = %d Bytes)",
			pokemon, *idUnico, *pos_x, *pos_y, *size);

	return msg;

}

void* rcv_appeared_broker(int socket_cliente, int *size) {

	void *msg;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	msg = malloc(*size);
	recv(socket_cliente, msg, *size, MSG_WAITALL);

	int offset = 0;

	int *id_mensaje = msg + offset;
	offset += sizeof(int);
	int *pos_x = msg + offset;
	offset += sizeof(int);
	int *pos_y = msg + offset;
	offset += sizeof(int);
	char*pokemon = msg + offset;

	log_info(g_logger, "(MSG-BODY= %d | %s | %d | %d -- SIZE = %d Bytes)",
			*id_mensaje, pokemon, *pos_x, *pos_y, *size);

	return msg;
}

void send_posiciones(int socket_cliente, char* pokemon) {

	printf("El socket es : %d \n", socket_cliente);

	FILE* posiciones = fopen("/home/utnso/config/Pokemon", "r");

	char* line = NULL;
	size_t len = 0;
	ssize_t read;

	while ((read = getline(&line, &len, posiciones)) != -1) {
		char** keyValue = malloc(sizeof(char*));
		keyValue = string_split(line, "=");

		char* key = keyValue[0];

		int cantidad = atoi(keyValue[1]);

		char** posiciones = malloc(sizeof(char*));
		posiciones = string_split(key, "-");

		int posicionX = atoi(posiciones[0]);
		int posicionY = atoi(posiciones[1]);

		t_posicion_pokemon *posicion = malloc(sizeof(t_posicion_pokemon));
		posicion->cantidad = cantidad;
		posicion->nombrePokemon = pokemon;
		posicion->posicionX = posicionX;
		posicion->posicionY = posicionY;

		printf("Pokemon %s : \n", posicion->nombrePokemon);
		printf("Posicion X: %d \n", posicion->posicionX);
		printf("Posicion Y: %d \n", posicion->posicionY);
		printf("Cantidad: %d \n", posicion->cantidad);
		printf("------------------------------ \n");

		free(keyValue);
		free(posiciones);
		free(posicion);

	}

	txt_close_file(posiciones);

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

void devolver_mensaje(void* payload, int size, int socket_cliente) {
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = size;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, payload, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2 * sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void iniciar_logger(void) {
	g_logger = log_create("/home/utnso/logs/server.log", "SERVER", 1,
			LOG_LEVEL_INFO);
}

