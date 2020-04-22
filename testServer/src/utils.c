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
	case CATCH_BROKER:
		log_info(g_logger, "(RECEIVING: BROKER@CATCH_POKEMON | Socket#: %d)",
				cliente_fd);
		msg = rcv_catch_broker(cliente_fd, &size);
		devolver_id_mensaje_propio(cliente_fd);
		// Devuelve al GAMEBOY un mensaje op_code = ID_MENSAJE c/id_mensaje propio
		break;
	case CATCH_GAMECARD:
		log_info(g_logger, "(RECEIVING: GAMECARD@CATCH_POKEMON | Socket#: %d)",
				cliente_fd);
		msg = rcv_catch_gamecard(cliente_fd, &size);
		devolver_caught_broker(msg, cliente_fd);
		//El GameBoy tiene que recibir un mensaje op_code = CAUGHT_BROKER como respuesta
		break;
	case CAUGHT_BROKER:
		log_info(g_logger, "(RECEIVING: GAMECARD@CAUGHT_POKEMON | Socket#: %d)",
				cliente_fd);
		msg = rcv_caught_broker(cliente_fd, &size);
		devolver_recepcion_ok(cliente_fd);
		// El GameBoy no tiene que recibir ninguna repuesta en este tipo de mensaje.
		break;
	case GET_BROKER:
		log_info(g_logger, "(RECEIVING: BROKER@GET_POKEMON | SOCKET#: %d)",
				cliente_fd);
		msg = rcv_get_broker(cliente_fd, &size);
		//devolver_id_mensaje_propio(cliente_fd);
		// El GameBoy tiene que recibir un mensaje op_code = ID_MENSAJE con id_mensaje propio
		devolver_posiciones(cliente_fd, (char*) msg);
		break;
	case GET_GAMECARD:
		log_info(g_logger, "(RECEIVING: GAMECARD@GET_POKEMON | SOCKET#: %d)",
				cliente_fd);
		msg = rcv_get_gamecard(cliente_fd, &size);
		devolver_localized_broker(cliente_fd, size, msg);
		// El GameBoy tiene que recibir un mensaje op_code = LOCALIZED_BROKER como respuesta
		break;
	case NEW_BROKER:
		log_info(g_logger, "(RECEIVING: BROKER@NEW_POKEMON | SOCKET#: %d)",
				cliente_fd);
		msg = rcv_new_broker(cliente_fd, &size);
		devolver_recepcion_ok(cliente_fd);
		// El GameBoy no tiene que recibir ninguna repuesta en este tipo de mensaje.
		break;
	case NEW_GAMECARD:
		log_info(g_logger, "(RECEIVING: GAMECARD@NEW_POKEMON | SOCKET#: %d)",
				cliente_fd);
		msg = rcv_new_gamecard(cliente_fd, &size);
		devolver_appeared_broker(msg, size, cliente_fd);
		// El GameBoy tiene que recibir un mensaje op_code = APPEARED_BROKER
		break;
	case APPEARED_BROKER:
		log_info(g_logger, "(RECEIVING: BROKER@APPEARED_POKEMON | Socket#: %d)",
				cliente_fd);
		msg = rcv_appeared_broker(cliente_fd, &size);
		devolver_recepcion_ok(cliente_fd);
		// El GameBoy recibe RECIBIDO_OK para cerrar la comunicación
		break;
	case APPEARED_TEAM:
		log_info(g_logger,
				"(RECEIVING: TEAM@APPEARED_POKEMON | Socket_Cliente: %d)",
				cliente_fd);
		msg = rcv_appeared_team(cliente_fd, &size);
		devolver_recepcion_ok(cliente_fd);
		// El GameBoy no tiene que recibir ninguna repuesta en este tipo de mensaje.
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
	int tamano = tamano_recibido(*size);

	log_info(g_logger, "(MSG-BODY= %s | %d | %d -- SIZE = %d Bytes)", pokemon,
			*pos_x, *pos_y, tamano);

	return msg;
}

void *rcv_caught_broker(int socket_cliente, int *size) {

	void *msg;
	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	msg = malloc(*size);
	recv(socket_cliente, msg, *size, MSG_WAITALL);
	int offset = 0;
	int *id_mensaje = msg + offset;
	offset += sizeof(int);
	t_result_caught *resultado_caught = msg + offset;
	int tamano = tamano_recibido(*size);
	log_info(g_logger, "(MSG-BODY= %d | %d -- SIZE = %d Bytes)", *id_mensaje,
			*resultado_caught, tamano);
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
	int tamano = tamano_recibido(*size);

	log_info(g_logger, "(MSG-BODY= %s | %d | %d | %d -- SIZE = %d Bytes)",
			pokemon, *pos_x, *pos_y, *cantidad, tamano);

	return msg;
}

void *rcv_new_gamecard(int socket_cliente, int *size) {
	void *msg;
	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	msg = malloc(*size);
	recv(socket_cliente, msg, *size, MSG_WAITALL);
	t_posicion_pokemon *posicion = malloc(sizeof(t_posicion_pokemon));

	int offset = 0;
	int *pos_x = msg + offset;
	offset += sizeof(int);
	int *pos_y = msg + offset;
	offset += sizeof(int);
	int *cantidad = msg + offset;
	offset += sizeof(int);
	char *nombrePokemon = msg + offset;
	int tamano = tamano_recibido(*size);

	posicion->pos_x = *pos_x;
	posicion->pos_y = *pos_y;
	posicion->cantidad = *cantidad;

	log_info(g_logger, "(MSG-BODY= %d | %d | %d | %d -- SIZE = %d Bytes)",
			 posicion->pos_x, posicion->pos_y,
			posicion->cantidad, tamano);
	free(posicion);
	return msg;
}

void* rcv_get_broker(int socket_cliente, int *size) {

	void *msg;
	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	msg = malloc(*size);
	recv(socket_cliente, msg, *size, MSG_WAITALL);

	char *pokemon = msg;
	int tamano = tamano_recibido(*size);

	log_info(g_logger, "(MSG-BODY= %s -- SIZE = %d Bytes)", pokemon, tamano);

	return msg;
}

void *rcv_get_gamecard(int socket_cliente, int *size) {
	void *msg;
	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	msg = malloc(*size);
	recv(socket_cliente, msg, *size, MSG_WAITALL);

	int offset = 0;
	int *idUnico = msg + offset;
	offset += sizeof(int);
	char *pokemon = msg + offset;

	int tamano = tamano_recibido(*size);
	log_info(g_logger, "(MSG-BODY= %d | %s -- SIZE = %d Bytes)", *idUnico,
			pokemon, tamano);

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
	int tamano = tamano_recibido(*size);

	log_info(g_logger, "(MSG-BODY= %d | %s | %d | %d -- SIZE = %d Bytes)",
			*idUnico, pokemon, *pos_x, *pos_y, tamano);

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
	int tamano = tamano_recibido(*size);

	log_info(g_logger, "(MSG-BODY= %d | %s | %d | %d -- SIZE = %d Bytes)",
			*id_mensaje, pokemon, *pos_x, *pos_y, tamano);

	return msg;
}

void* rcv_appeared_team(int socket_cliente, int *size) {
	void* msg;
	int* posX;
	int* posY;
	char* pokemonName;
	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	msg = malloc(*size);
	recv(socket_cliente, msg, *size, MSG_WAITALL);
	int offset = 0;
	posX = msg + offset;
	offset += sizeof(int);
	posY = msg + offset;
	offset += sizeof(int);
	pokemonName = msg + offset;

	log_info(g_logger, "(MSG-BODY= %d | %d | %s -- SIZE = %d Bytes)", *posX,
			*posY, pokemonName, *size);

	return msg;
}

void devolver_posiciones(int socket_cliente, char* pokemon) {
	printf("El socket es : %d \n", socket_cliente);
	FILE* posiciones = fopen("/home/utnso/config/Pokemon", "r");
	char* line = NULL;
	size_t len = 0;
	ssize_t read;

	t_config* config = config_create("/home/utnso/config/gameboy.config");

	int idMensajeUnico = config_get_int_value(config, "ID_MENSAJE_UNICO");

	t_list* listaPosiciones = list_create();

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
		posicion->pos_x = posicionX;
		posicion->pos_y = posicionY;

		printf("Pokemon %s : \n", pokemon);
		printf("Posicion X: %d \n", posicion->pos_x);
		printf("Posicion Y: %d \n", posicion->pos_y);
		printf("Cantidad: %d \n", posicion->cantidad);
		printf("------------------------------ \n");

		list_add(listaPosiciones, posicion);

		free(keyValue);
		free(posiciones);

	}

	int cantidadDePosiciones = list_size(listaPosiciones);

	printf("La cantidad de posiciones en la lista es %d: \n",
			cantidadDePosiciones);

	int totalBytes = sizeof(int) + cantidadDePosiciones * 3 * sizeof(int);

	printf("La cantidad de bytes son :%d \n", totalBytes);

	void* stream = malloc(totalBytes);

	int offset = 0;

	memcpy(stream+offset, &(idMensajeUnico), sizeof(int));
	offset += sizeof(int);

	for (int procesados = 0; procesados < cantidadDePosiciones; procesados++) {

		t_posicion_pokemon* posicionActual = list_get(listaPosiciones,
				procesados);

		memcpy(stream + offset, &(posicionActual->cantidad), sizeof(int));
		offset += sizeof(int);

		memcpy(stream + offset, &(posicionActual->pos_x), sizeof(int));
		offset += sizeof(int);

		printf("la posicion x es : %d \n", posicionActual->pos_x);

		memcpy(stream + offset, &(posicionActual->pos_y), sizeof(int));
		offset += sizeof(int);

		printf("la posicion y es : %d \n", posicionActual->pos_y);

		printf("El offset es: %d \n", offset);

	}

	printf("Termine de armar el stream \n");

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = LOCALIZED_BROKER;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = totalBytes;
	paquete->buffer->stream = malloc(sizeof(paquete->buffer->size));
	paquete->buffer->stream = stream;

	printf("Termine de armar el paquete \n");

	int totalBuffer = paquete->buffer->size + (2 * (sizeof(int)));

	printf("El total del tamanio de a_enviar es: %d \n", totalBuffer);

	void* a_enviar = serializar_paquete(paquete, totalBuffer);

	printf("Termine de serializar paquete \n");

	send(socket_cliente, a_enviar, totalBuffer, MSG_WAITALL);

	printf("Enviado el paquete \n");

	free(a_enviar);

	printf("Liberado a_enviar \n");

	eliminar_paquete(paquete);

	printf("Liberado el paquete \n");

	list_destroy(listaPosiciones);

	printf("Destruida la lista \n");

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

void devolver_id_mensaje_propio(int socket_cliente) {
	t_paquete* paquete = malloc(sizeof(t_paquete));
	int id_mensaje = ID_MSG_RTA;
	log_info(g_logger, "(RESPUESTA: ID_MENSAJE= %d)", id_mensaje);

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

void devolver_recepcion_ok(int socket_cliente) {
	t_paquete* paquete = malloc(sizeof(t_paquete));
	char *respuesta = RESPUESTA_OK;
	log_info(g_logger, "(RESPUESTA = %s)", respuesta);

	paquete->codigo_operacion = MSG_CONFIRMED;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = strlen(respuesta) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, respuesta, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2 * sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);
	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

void devolver_caught_broker(void *msg, int socket_cliente) {
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = CAUGHT_BROKER;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = 2 * sizeof(int);
	void *stream = malloc(paquete->buffer->size);

	int offset = 0;
	memcpy(stream + offset, msg, sizeof(int)); // Tomamos el id_mensaje Recibido en CATCH_GAMECARD
	t_result_caught resultado = OK;
	offset += sizeof(t_result_caught);
	memcpy(stream + offset, &(resultado), sizeof(t_result_caught));
	paquete->buffer->stream = stream;

	int bytes = paquete->buffer->size + 2 * sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);
	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

void devolver_appeared_broker(void *msg, int size, int socket_cliente) {
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = APPEARED_BROKER;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = size - sizeof(int);
	int long_pokemon = paquete->buffer->size - 3 * sizeof(int);
	void *stream = malloc(paquete->buffer->size);

	int offset = 0;
	memcpy(stream + offset, msg, 3 * sizeof(int)); //Tomamos el id_mensaje, pos_x y pos_y Recibido en new_GAMECARD
	offset += 3 * sizeof(int);
	memcpy(stream + offset, msg + offset + sizeof(int), long_pokemon);
	paquete->buffer->stream = stream;

	int bytes = paquete->buffer->size + 2 * sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);
	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

void devolver_localized_broker(int socket_cliente, int size, void *msg) {
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = LOCALIZED_BROKER;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = size;
	int long_pokemon = paquete->buffer->size - sizeof(int);
	void *stream = malloc(paquete->buffer->size);
	//TODO Buscarle la vuelta para enviar la lista de posiciones con la cantidad correspondiente
	int offset = 0;
	memcpy(stream + offset, msg, sizeof(int)); //Tomamos el id_mensaje, pos_x y pos_y Recibido en new_GAMECARD
	offset += sizeof(int);
	memcpy(stream + offset, msg + offset, long_pokemon);
	paquete->buffer->stream = stream;

	int bytes = paquete->buffer->size + 2 * sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);
	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);

}

void devolver_id_mensaje(void *msg, int socket_cliente) {
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = ID_MENSAJE;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = sizeof(int);
	void *stream = malloc(paquete->buffer->size);

	memcpy(stream, msg, sizeof(int)); // Tomamos el id_mensaje Recibido en CATCH_GAMECARD
	int bytes = paquete->buffer->size + 2 * sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);
	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

int tamano_recibido(int bytes) {
	return bytes + 2 * sizeof(int);
}

void eliminar_paquete(t_paquete *paquete) {
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void iniciar_logger(void) {
	g_logger = log_create("/home/utnso/logs/server.log", "SERVER", 1,
			LOG_LEVEL_INFO);
}
