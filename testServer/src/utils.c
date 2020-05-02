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
		esperar_cliente(socket_servidor);
}

void esperar_cliente(int socket_servidor) {
	struct sockaddr_in dir_cliente;

	socklen_t tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente,
			&tam_direccion);

	t_socket_cliente *socket = malloc(sizeof(t_socket_cliente));
	socket->cliente_fd = socket_cliente;
	// inicializa contador de mensajes de algun cliente suscriptor
	socket->cant_msg_enviados = 0;
	pthread_create(&thread, NULL, (void*) serve_client, socket);
	pthread_detach(thread);
}

void serve_client(t_socket_cliente *socket) {
	int cod_op;
	int cliente_fd = socket->cliente_fd;
	// OBTENGO el Codigo_Operacion de los  Mensajes que Recibo
	if (recv(cliente_fd, &cod_op, sizeof(int), MSG_WAITALL) == -1) {
		cod_op = -1;
	}
	if (cod_op < SUSCRIP_NEW) {
		process_request(cod_op, socket);
	} else {
		process_suscript(cod_op, socket);
	}
}
// RECIBE todos los TIPOS de MENSAJE QUE MANEJA el GAMEBOY y resuelve segun el CODIGO_OPERACION del MENSAJE
void process_request(op_code cod_op, t_socket_cliente *socket) {
	int size;
	int cliente_fd = socket->cliente_fd;
	t_handsake_suscript *handshake = malloc(sizeof(t_handsake_suscript));
	handshake->msjs_recibidos = socket->cant_msg_enviados;
	int existePokemon;
	char* nombrePokemon;
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
		devolver_id_mensaje_propio(cliente_fd);
		// El GameBoy tiene que recibir un mensaje op_code = ID_MENSAJE con id_mensaje propio
		break;
	case GET_GAMECARD:
		log_info(g_logger, "(RECEIVING: GAMECARD@GET_POKEMON | SOCKET#: %d)",
				cliente_fd);
		msg = rcv_get_gamecard(cliente_fd, &size);
		nombrePokemon = msg + sizeof(int);
		log_info(g_logger, "POKEMON: %s", nombrePokemon);

		devolver_posiciones(cliente_fd, nombrePokemon, &existePokemon);

		log_info(g_logger, "VALOR ENCONTRO POKEMON: %d", existePokemon);

		//Verifico si encontro o no el archivo
		if (existePokemon == 0) {
			devolver_recepcion_fail(cliente_fd,	"NO SE ENCONTRO EL ARCHIVO DEL POKEMON");
		}

		//devolver_posiciones(cliente_fd, "Pokemon");
		//devolver_localized_broker(cliente_fd, size, msg);
		// El GameBoy tiene que recibir un mensaje op_code = LOCALIZED_BROKER como respuesta
		break;
	case NEW_BROKER:
		log_info(g_logger, "(RECEIVING: BROKER@NEW_POKEMON | SOCKET#: %d)", cliente_fd);
		msg = rcv_new_broker(cliente_fd, &size);
		devolver_recepcion_ok(cliente_fd);
		// El GameBoy no tiene que recibir ninguna repuesta en este tipo de mensaje.
		break;
	case NEW_GAMECARD:
		log_info(g_logger, "(RECEIVING: GAMECARD@NEW_POKEMON | SOCKET#: %d)", cliente_fd);
		msg = rcv_new_gamecard(cliente_fd, &size);
		devolver_appeared_broker(msg, size, cliente_fd);
		// El GameBoy tiene que recibir un mensaje op_code = APPEARED_BROKER
		break;
	case APPEARED_BROKER:
		log_info(g_logger, "(RECEIVING: BROKER@APPEARED_POKEMON | Socket#: %d)", cliente_fd);
		msg = rcv_appeared_broker(cliente_fd, &size);
		devolver_recepcion_ok(cliente_fd);
		// El GameBoy recibe RECIBIDO_OK para cerrar la comunicación
		break;
	case APPEARED_TEAM:
		log_info(g_logger, "(RECEIVING: TEAM@APPEARED_POKEMON | Socket#: %d)", cliente_fd);
		msg = rcv_appeared_team(cliente_fd, &size);
		devolver_recepcion_ok(cliente_fd);
		// El GameBoy no tiene que recibir ninguna repuesta en este tipo de mensaje.
		break;
	case FIN_SUSCRIPCION:
		msg = malloc(1);
		handshake = rcv_fin_suscripcion(cliente_fd, &size);
		stop_suscription(g_suscriptores, handshake);
		devolver_recepcion_ok(cliente_fd);
		break;
	case 0:
		pthread_exit(NULL);
	case -1:
		pthread_exit(NULL);
	}
	free(msg);
	free(handshake);
	free(socket);
}

// MENSAJES QUE RECIBE en MODO SUSCRIPTOR
void process_suscript(op_code cod_op, t_socket_cliente *socket) {
	int size;
	int id_suscriptor;
	int cliente_fd = socket->cliente_fd;
	int cant_msg = socket->cant_msg_enviados;
	t_handsake_suscript *handshake;
	switch (cod_op) {
	case SUSCRIP_NEW:
		handshake = rcv_handshake_suscripcion(socket, &size);
		id_suscriptor = handshake->id_suscriptor;
		cant_msg = handshake->msjs_recibidos;
		if (cant_msg == 0) {
			log_info(g_logger,"(SUSCRIPT_RECEIVED: NEW_POKEMON | ID_SUSCRIPTOR = %d | Socket# = %d)", handshake->id_suscriptor, cliente_fd);
			start_suscription(g_suscriptores,handshake);
		}
		// Simulo Buscar mensajes en cola NEW_POKEMON no enviados al suscriptor
		while (is_active_suscriptor(g_suscriptores, id_suscriptor)) {
			socket->cant_msg_enviados = cant_msg;
			if (cant_msg >= 0 && cant_msg < 10000){
				enviar_msjs_new(socket);
				free(handshake);
				serve_client(socket);
				free(socket);
			}
			else {
				enviar_cola_vacia(socket, id_suscriptor);
				sleep(1);
				free(handshake);
				serve_client(socket);
				free(socket);
			}
		}
		socket->cant_msg_enviados = cant_msg;
		enviar_suscript_end(socket, id_suscriptor);
		remove_ended_suscriptor(g_suscriptores,id_suscriptor);
		free(handshake);
		free(socket);
		pthread_exit(EXIT_SUCCESS);
	case SUSCRIP_APPEARED:
		handshake = rcv_handshake_suscripcion(socket, &size);
		id_suscriptor = handshake->id_suscriptor;
		cant_msg = handshake->msjs_recibidos;
		if (cant_msg == 0) {
			log_info(g_logger,"(SUSCRIPT_RECEIVED: APPEARED_POKEMON | ID_SUSCRIPTOR = %d | Socket# = %d)", handshake->id_suscriptor, cliente_fd);
			start_suscription(g_suscriptores,handshake);
		}
		while (is_active_suscriptor(g_suscriptores, id_suscriptor)) {
			socket->cant_msg_enviados = cant_msg;
			if (cant_msg >= 0 && cant_msg < 10000){
				enviar_msjs_appeared(socket);
				free(handshake);
				serve_client(socket);
				free(socket);
			}
			else {
				enviar_cola_vacia(socket, id_suscriptor);
				sleep(15);
				free(handshake);
				serve_client(socket);
				free(socket);
			}
		}
		socket->cant_msg_enviados = cant_msg;
		enviar_suscript_end(socket, id_suscriptor);
		remove_ended_suscriptor(g_suscriptores,id_suscriptor);
		free(handshake);
		free(socket);
		pthread_exit(EXIT_SUCCESS);
	case SUSCRIP_CATCH:
		handshake = rcv_handshake_suscripcion(socket, &size);
		id_suscriptor = handshake->id_suscriptor;
		cant_msg = handshake->msjs_recibidos;
		if (cant_msg == 0) {
			log_info(g_logger,"(SUSCRIPT_RECEIVED: CATCH_POKEMON | ID_SUSCRIPTOR = %d | Socket# = %d)", handshake->id_suscriptor, cliente_fd);
			start_suscription(g_suscriptores,handshake);
		}
		while (is_active_suscriptor(g_suscriptores, id_suscriptor)) {
			socket->cant_msg_enviados = cant_msg;
			if (cant_msg >= 0 && cant_msg < 10000){
				enviar_msjs_catch(socket);
				free(handshake);
				serve_client(socket);
				free(socket);
			}
			else {
				enviar_cola_vacia(socket, id_suscriptor);
				sleep(15);
				free(handshake);
				serve_client(socket);
				free(socket);
			}
		}
		socket->cant_msg_enviados = cant_msg;
		enviar_suscript_end(socket, id_suscriptor);
		remove_ended_suscriptor(g_suscriptores,id_suscriptor);
		free(handshake);
		free(socket);
		pthread_exit(EXIT_SUCCESS);
	case SUSCRIP_CAUGHT:
		handshake = rcv_handshake_suscripcion(socket, &size);
		id_suscriptor = handshake->id_suscriptor;
		cant_msg = handshake->msjs_recibidos;
		if (cant_msg == 0) {
			log_info(g_logger,"(SUSCRIPT_RECEIVED: CATCH_POKEMON | ID_SUSCRIPTOR = %d | Socket# = %d)", handshake->id_suscriptor, cliente_fd);
			start_suscription(g_suscriptores,handshake);
		}
		while (is_active_suscriptor(g_suscriptores, id_suscriptor)) {
			socket->cant_msg_enviados = cant_msg;
			if (cant_msg >= 0 && cant_msg < 10000){
				enviar_msjs_caught(socket);
				free(handshake);
				serve_client(socket);
				free(socket);
			}
			else {
				enviar_cola_vacia(socket, id_suscriptor);
				sleep(15);
				free(handshake);
				serve_client(socket);
				free(socket);
			}
		}
		socket->cant_msg_enviados = cant_msg;
		enviar_suscript_end(socket, id_suscriptor);
		remove_ended_suscriptor(g_suscriptores,id_suscriptor);
		free(handshake);
		free(socket);
		pthread_exit(EXIT_SUCCESS);
	case SUSCRIP_GET:
		handshake = rcv_handshake_suscripcion(socket, &size);
		id_suscriptor = handshake->id_suscriptor;
		cant_msg = handshake->msjs_recibidos;
		if (cant_msg == 0) {
			log_info(g_logger,"(SUSCRIPT_RECEIVED: GET_POKEMON | ID_SUSCRIPTOR = %d | Socket# = %d)", handshake->id_suscriptor, cliente_fd);
			start_suscription(g_suscriptores,handshake);
		}
		while (is_active_suscriptor(g_suscriptores, id_suscriptor)) {
			socket->cant_msg_enviados = cant_msg;
			if (cant_msg >= 0 && cant_msg < 10000){
				enviar_msjs_get(socket);
				free(handshake);
				serve_client(socket);
				free(socket);
			}
			else {
				enviar_cola_vacia(socket, id_suscriptor);
				sleep(15);
				free(handshake);
				serve_client(socket);
				free(socket);
			}
		}
		socket->cant_msg_enviados = cant_msg;
		enviar_suscript_end(socket, id_suscriptor);
		remove_ended_suscriptor(g_suscriptores,id_suscriptor);
		free(handshake);
		free(socket);
		pthread_exit(EXIT_SUCCESS);
	case SUSCRIP_LOCALIZED:
		handshake = rcv_handshake_suscripcion(socket, &size);
		id_suscriptor = handshake->id_suscriptor;
		cant_msg = handshake->msjs_recibidos;
		if (cant_msg == 0) {
			log_info(g_logger,"(SUSCRIPT_RECEIVED: LOCALIZED_POKEMON | ID_SUSCRIPTOR = %d | Socket# = %d)", handshake->id_suscriptor, cliente_fd);
			start_suscription(g_suscriptores,handshake);
		}
		while (is_active_suscriptor(g_suscriptores, id_suscriptor)) {
			socket->cant_msg_enviados = cant_msg;
			if (cant_msg >= 0 && cant_msg < 10000){
				enviar_msjs_localized(socket);
				free(handshake);
				serve_client(socket);
				free(socket);
			}
			else {
				enviar_cola_vacia(socket, id_suscriptor);
				sleep(5);
				free(handshake);
				serve_client(socket);
				free(socket);
			}
		}
		socket->cant_msg_enviados = cant_msg;
		enviar_suscript_end(socket, id_suscriptor);
		remove_ended_suscriptor(g_suscriptores,id_suscriptor);
		free(handshake);
		free(socket);
		pthread_exit(EXIT_SUCCESS);
	case 0:
		pthread_exit(NULL);
	case -1:
		pthread_exit(NULL);
	}
	free(handshake);
	free(socket);
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
	int *id_mensaje = msg + offset;
	offset += sizeof(int);
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

	log_info(g_logger, "(MSG-BODY= %d | %s | %d | %d | %d -- SIZE = %d Bytes)",
			*id_mensaje, nombrePokemon, posicion->pos_x, posicion->pos_y,
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

t_handsake_suscript *rcv_handshake_suscripcion(t_socket_cliente *socket, int *size)
{
	t_handsake_suscript *handshake = malloc(sizeof(t_handsake_suscript));
	int socket_cliente = socket->cliente_fd;
	void *msg;
	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	msg = malloc(*size);
	recv(socket_cliente, msg, *size, MSG_WAITALL);
	int offset = 0;
	memcpy(&(handshake->id_suscriptor), msg + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&(handshake->msjs_recibidos), msg + offset, sizeof(int));
	free(msg);
	return handshake;
}

t_handsake_suscript *rcv_fin_suscripcion(int socket_cliente, int *size)
{
	void *msg;
	t_handsake_suscript *handshake = malloc(sizeof(t_handsake_suscript));
	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	msg = malloc(*size);
	recv(socket_cliente, msg, *size, MSG_WAITALL);
	int offset = 0;
	memcpy(&(handshake->id_suscriptor), msg + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&(handshake->msjs_recibidos), msg + offset, sizeof(int));
	log_info(g_logger,"(RECEIVING: END_SUSCRIPTION|ID_SUSCRIPTOR = %d|Socket# = %d)", handshake->id_suscriptor, socket_cliente);
	free(msg);
	return handshake;
}

void start_suscription(t_list *suscriptores,t_handsake_suscript *handshake)
{
	t_suscriptor *suscriptor=malloc(sizeof(t_suscriptor));
	suscriptor->id_suscriptor = handshake->id_suscriptor;
	suscriptor->cant_msg = handshake->msjs_recibidos;
	suscriptor->is_active = 1;
	list_add(suscriptores,suscriptor);
}

void stop_suscription(t_list *suscriptores, t_handsake_suscript *handshake)
{
	int id_suscriptor = handshake->id_suscriptor;
	t_suscriptor *suscriptor = obtengo_suscriptor(suscriptores , id_suscriptor);
	suscriptor->is_active = 0;
}

void remove_ended_suscriptor(t_list *suscriptores, int id_suscriptor)
{
	t_suscriptor *suscriptor = obtengo_suscriptor(suscriptores , id_suscriptor);
	delete_suscriptor(suscriptores, suscriptor);
}

void delete_suscriptor(t_list *suscriptores, void *suscriptor)
{
	t_suscriptor *suscrip_t = (t_suscriptor*) suscriptor;
	bool compare_suscript(void *otro_suscriptor){
		t_suscriptor *otro_suscrip_t = (t_suscriptor*) otro_suscriptor;
		bool condition = otro_suscrip_t->id_suscriptor == suscrip_t->id_suscriptor;
		return condition;
	}
	list_remove_by_condition(suscriptores, compare_suscript);
}

bool is_active_suscriptor(t_list *suscriptores, int id_suscriptor)
{
	t_suscriptor *suscriptor = obtengo_suscriptor(suscriptores, id_suscriptor);
	return suscriptor->is_active;
}

t_suscriptor *obtengo_suscriptor(t_list *suscriptores, int id_suscriptor)
{
	bool mismo_id_suscriptor(void *suscriptor)
	{
		t_suscriptor* t_suscript = (t_suscriptor*) suscriptor;
		bool condition = id_suscriptor == t_suscript->id_suscriptor;
		return condition;
	}

	t_suscriptor *suscriptor_buscado;
	suscriptor_buscado = list_find(suscriptores, mismo_id_suscriptor);
	if (suscriptor_buscado != NULL) {
		return suscriptor_buscado;
	} else {
		log_error(g_logger,"(ERROR|SUSCRIPTOR NO_EXISTE)");
		return NULL;
	}
}

void devolver_posiciones(int socket_cliente, char* pokemon,	int* encontroPokemon) {

	printf("El socket es : %d \n", socket_cliente);

	//Por como funciona la funcion strcat, guardo espacio para la ruta completa en el primer argumento
	char* ruta = malloc(20 + strlen(pokemon) + 1);
	strcpy(ruta, "/home/utnso/config/");

		//Creo la ruta segun el pokemon
		char* path = strcat(ruta, pokemon);

		printf("El tamanio del path es: %d \n", strlen(path));

		printf("El path del pokemon es: %s \n", path);

		FILE* posiciones = fopen(path, "r");
	//Verifico si el archivo existe (fopen devuelve null si no existe)
	if (posiciones != NULL) {
		//Asigno 1 haciendo referencia a que encontro el archivo
		*encontroPokemon = 1;

		char* line = malloc(100);
		size_t len = 0;
		ssize_t read = getline(&line, &len, posiciones);

		//Verifico que si el archivo esta vacio
		if (read == -1) {
			devolver_recepcion_fail(socket_cliente, "SE ENCONTRO EL ARCHIVO PERO ESTA VACIO");
			//Si tiene contenido, hago el flujo de carga
		} else {

			printf("Se encontró el archivo con contenido\n");
			t_config* config = config_create("/home/utnso/config/gameboy.config");
			int idMensajeUnico = config_get_int_value(config,"ID_MENSAJE_UNICO");

			t_list* listaPosiciones = list_create();

			rewind(posiciones);

			while ((read = getline(&line, &len, posiciones)) != -1) {
				printf("Entro a leer lista\n");

				char** keyValue = string_split(line, "=");

				printf("Luego de primer split\n");

				char* key = keyValue[0];

				int cantidad = atoi(keyValue[1]);

				char** posiciones = string_split(key, "-");

				printf("Luego de segundo split\n");

				int posicionX = atoi(posiciones[0]);
				int posicionY = atoi(posiciones[1]);

				t_posicion_pokemon *posicion = malloc(
						sizeof(t_posicion_pokemon));

				posicion->cantidad = cantidad;
				posicion->pos_x = posicionX;
				posicion->pos_y = posicionY;

				printf("Pokemon %s : \n", pokemon);
				printf("Posicion X: %d \n", posicion->pos_x);
				printf("Posicion Y: %d \n", posicion->pos_y);
				printf("Cantidad: %d \n", posicion->cantidad);
				printf("------------------------------ \n");

				list_add(listaPosiciones, posicion);

				liberar_listas(keyValue);
				liberar_listas(posiciones);

			}
			printf("Salgo de leer lista\n");

			int size_pokemon = strlen(pokemon) + 1;
			printf("pokemon:%s -tamaño:%d\n",pokemon, size_pokemon);
			int cantidadDePosiciones = list_size(listaPosiciones);
			printf("La cantidad de posiciones en la lista es %d: \n",cantidadDePosiciones);
			int totalBytes = (2 + (cantidadDePosiciones * 3)) * sizeof(int) + size_pokemon;
			printf("La cantidad de bytes son :%d \n", totalBytes);
			void* stream = malloc(totalBytes);
			int offset = 0;
			memcpy(stream + offset, &(idMensajeUnico), sizeof(int));
			offset += sizeof(int);
			memcpy(stream + offset, &(cantidadDePosiciones), sizeof(int));
			offset += sizeof(int);
			printf("Comienzo a serializar\n");

			for (int procesados = 0; procesados < cantidadDePosiciones;	procesados++) {
				t_posicion_pokemon* posicionActual = list_get(listaPosiciones,procesados);
				memcpy(stream + offset, &(posicionActual->cantidad),sizeof(int));
				offset += sizeof(int);
				memcpy(stream + offset, &(posicionActual->pos_x), sizeof(int));
				offset += sizeof(int);
				printf("la posicion x es : %d \n", posicionActual->pos_x);
				memcpy(stream + offset, &(posicionActual->pos_y), sizeof(int));
				offset += sizeof(int);
				printf("la posicion y es : %d \n", posicionActual->pos_y);
				printf("El offset es: %d \n", offset);
			}
			memcpy(stream + offset, pokemon, size_pokemon);
			printf("Termine de armar el stream \n");

			t_paquete* paquete = malloc(sizeof(t_paquete));
			paquete->codigo_operacion = LOCALIZED_BROKER;
			paquete->buffer = malloc(sizeof(t_stream));
			paquete->buffer->size = totalBytes;
			paquete->buffer->data = stream;

			printf("Termine de armar el paquete \n");

			int totalBuffer = paquete->buffer->size + (2 * (sizeof(int)));

			printf("El total del tamanio de a_enviar es: %d \n", totalBuffer);

			void* a_enviar = serializar_paquete(paquete, totalBuffer);

			printf("Termine de serializar paquete \n");

			send(socket_cliente, a_enviar, totalBuffer, MSG_WAITALL);

			printf("Enviado el paquete \n");

			free(a_enviar);

			printf("Liberado a_enviar \n");

			free(stream);
			free(paquete->buffer);
			free(paquete);

			printf("Liberado el paquete \n");


			liberar_lista_posiciones(listaPosiciones);

			printf("Destruida la lista \n");

			txt_close_file(posiciones);

			config_destroy(config);
		}

		free(line);

	} else {
		//Asigno 0 haciendo referencia a que no encontro el archivo
		*encontroPokemon = 0;
	}

	free(ruta);

}

void* serializar_paquete(t_paquete* paquete, int bytes) {
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->data,
			paquete->buffer->size);
	desplazamiento += paquete->buffer->size;
	return magic;
}

void devolver_id_mensaje_propio(int socket_cliente) {
	t_paquete* paquete = malloc(sizeof(t_paquete));
	int id_mensaje = ID_MSG_RTA;
	log_info(g_logger, "(SENDING: ID_MSG:%d)", id_mensaje);

	paquete->codigo_operacion = ID_MENSAJE;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = sizeof(int);
	paquete->buffer->data = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->data, &(id_mensaje), paquete->buffer->size);

	int bytes = paquete->buffer->size + 2 * sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);
	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

void liberar_lista_posiciones(t_list* lista){
	for(int i = 0; i< list_size(lista); i++){
		free(list_get(lista,i));
	}

	list_destroy(lista);
}

void liberar_listas(char** lista) {

	int contador = 0;
	while (lista[contador] != NULL) {
		free(lista[contador]);
		contador++;
	}

	free(lista);
}

void devolver_recepcion_ok(int socket_cliente) {
	t_paquete* paquete = malloc(sizeof(t_paquete));
	int respuesta = RESPUESTA_OK;
	paquete->codigo_operacion = MSG_CONFIRMED;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = sizeof(int);
	paquete->buffer->data = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->data, &respuesta, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2 * sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);
	if (send(socket_cliente, a_enviar, bytes, MSG_WAITALL) != bytes) {
		log_error(g_logger, "(SENDING: MSG_ERROR)");
	}
	log_info(g_logger, "(SENDING: MSG_CONFIRMED)");
	free(a_enviar);
	eliminar_paquete(paquete);
}

void devolver_recepcion_fail(int socket_cliente, char* mensajeError) {
	t_paquete* paquete = malloc(sizeof(t_paquete));

	log_info(g_logger, "(RESPUESTA = %s)", mensajeError);

	paquete->codigo_operacion = MSG_ERROR;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = strlen(mensajeError) + 1;
	paquete->buffer->data = malloc(paquete->buffer->size);

	memcpy(paquete->buffer->data, mensajeError, paquete->buffer->size);

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
	paquete->buffer->data = stream;

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
	paquete->buffer->data = stream;

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
	send(socket_cliente, a_enviar, bytes,MSG_WAITALL);

	free(a_enviar);
	eliminar_paquete(paquete);
}

void enviar_cola_vacia(t_socket_cliente *socket, int id_suscriptor) {
	t_paquete* paquete = malloc(sizeof(t_paquete));
	log_info(g_logger, "(SENDING: EMPTY_QUEUE | ID_SUSCRIPTOR = %d)",
			id_suscriptor);
	paquete->codigo_operacion = COLA_VACIA;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = sizeof(int);
	paquete->buffer->data = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->data, &(id_suscriptor), paquete->buffer->size);
	int bytes = paquete->buffer->size + 2 * sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);
	if (send(socket->cliente_fd, a_enviar, bytes, MSG_WAITALL) != bytes) {
		exit(EXIT_FAILURE);
	}
	free(a_enviar);
	eliminar_paquete(paquete);
}

void enviar_msjs_get(t_socket_cliente *socket)
{
	t_msg_get *msg_get = malloc(sizeof(t_msg_get));
	t_paquete* paquete = malloc(sizeof(t_paquete));
	int socket_cliente = socket->cliente_fd;
	msg_get->id_mensaje = socket->cant_msg_enviados + 700;
	msg_get->pokemon = "Charizard";
	paquete->codigo_operacion = GET_GAMECARD;
	paquete->buffer = malloc(sizeof(t_stream));
	int size_pokemon = strlen(msg_get->pokemon) + 1;
	paquete->buffer->size = sizeof(int) + size_pokemon;
	paquete->buffer->data = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->data + offset, &(msg_get->id_mensaje), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, msg_get->pokemon, size_pokemon);
	int bytes = paquete->buffer->size + 2 * sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);
	if (send(socket_cliente, a_enviar, bytes, MSG_WAITALL) == bytes) {
		log_info(g_logger, "(SENDING: GET_POKEMON | Socket# = %d | ID_MENSAJE = %d | Pokemon = %s)",
			socket_cliente, msg_get->id_mensaje, msg_get->pokemon);
	}
	free(msg_get);
	free(a_enviar);
	eliminar_paquete(paquete);
}

void enviar_msjs_new(t_socket_cliente *socket)
{
	t_msg_new *msg_new = malloc(sizeof(t_msg_new));
	t_paquete* paquete = malloc(sizeof(t_paquete));
	msg_new->coord = malloc(sizeof(t_coordenada));
	msg_new->id_mensaje = socket->cant_msg_enviados + 550;
	msg_new->coord->pos_x = 25;
	msg_new->coord->pos_y = 52;
	msg_new->cantidad = 73;
	msg_new->pokemon = "Bulbasaur";
	paquete->codigo_operacion = NEW_GAMECARD;
	paquete->buffer = malloc(sizeof(t_stream));
	int size_pokemon = strlen(msg_new->pokemon) + 1;
	paquete->buffer->size = 4 * sizeof(int) + size_pokemon;
	paquete->buffer->data = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->data + offset, &(msg_new->id_mensaje),sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, &(msg_new->coord->pos_x), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, &(msg_new->coord->pos_y),sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, &(msg_new->cantidad), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, msg_new->pokemon, size_pokemon);
	int bytes = paquete->buffer->size + 2 * sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);
	if (send(socket->cliente_fd, a_enviar, bytes, MSG_WAITALL) != bytes) {
		exit(EXIT_FAILURE);
	}
	log_info(g_logger,"(SENDING: NEW_POKEMON|Socket#:%d|ID_MENSAJE=%d|Pokemon: %s|POS_X=%d|POS_Y=%d|QTY=%d)",
			socket->cliente_fd, msg_new->id_mensaje,msg_new->pokemon, msg_new->coord->pos_x, msg_new->coord->pos_y, msg_new->cantidad);
	free(msg_new->coord);
	free(msg_new);
	free(a_enviar);
	eliminar_paquete(paquete);
}

void enviar_msjs_appeared(t_socket_cliente *socket)
{
	t_msg_appeared *msg_appeared = malloc(sizeof(t_msg_appeared));
	t_paquete *paquete = malloc(sizeof(t_paquete));
	msg_appeared->coord = malloc(sizeof(t_coordenada));
	msg_appeared->coord->pos_x = 20;
	msg_appeared->coord->pos_y = 25;
	msg_appeared->pokemon = "Wartortle";
	paquete->codigo_operacion = APPEARED_TEAM;
	paquete->buffer = malloc(sizeof(t_stream));
	int size_pokemon = strlen(msg_appeared->pokemon) + 1;
	paquete->buffer->size = 2 * sizeof(int) + size_pokemon;
	paquete->buffer->data = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->data + offset, &(msg_appeared->coord->pos_x),sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, &(msg_appeared->coord->pos_y), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, msg_appeared->pokemon, size_pokemon);
	int bytes = paquete->buffer->size + 2 * sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);
	if (send(socket->cliente_fd, a_enviar, bytes, MSG_WAITALL) != bytes) {
		exit(EXIT_FAILURE);
	}
	log_info(g_logger,"(SENDING: APPEARED_POKEMON | Socket#: %d|Pokemon: %s|POS_X: %d|POS_Y: %d)",
			socket->cliente_fd, msg_appeared->pokemon, msg_appeared->coord->pos_x, msg_appeared->coord->pos_y);
	free(msg_appeared->coord);
	free(msg_appeared);
	free(a_enviar);
	eliminar_paquete(paquete);
}

void enviar_msjs_catch(t_socket_cliente *socket)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	t_msg_catch *msg_catch = malloc(sizeof(t_msg_catch));
	msg_catch->coord = malloc(sizeof(t_coordenada));
	msg_catch->id_mensaje = socket->cant_msg_enviados + 600;
	msg_catch->coord->pos_x = 78;
	msg_catch->coord->pos_y = 39;
	msg_catch->pokemon = "Charmeleon";
	paquete->codigo_operacion = CATCH_GAMECARD;
	paquete->buffer = malloc(sizeof(t_stream));
	int size_pokemon = strlen(msg_catch->pokemon) + 1;
	paquete->buffer->size = 3 * sizeof(int) + size_pokemon;
	paquete->buffer->data = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->data + offset, &(msg_catch->id_mensaje),sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, &(msg_catch->coord->pos_x),sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, &(msg_catch->coord->pos_y), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, msg_catch->pokemon, size_pokemon);
	int bytes = paquete->buffer->size + 2 * sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);
	if (send(socket->cliente_fd, a_enviar, bytes, MSG_WAITALL) != bytes) {
		exit(EXIT_FAILURE);
	}
	log_info(g_logger,"(SENDING: CATCH_POKEMON|Socket#: %d|ID_MESSAGE=%d|Pokemon: %s|POS_X=%d|POS_Y=%d)",
			socket->cliente_fd, msg_catch->id_mensaje, msg_catch->pokemon, msg_catch->coord->pos_x, msg_catch->coord->pos_y);
	free(msg_catch->coord);
	free(msg_catch);
	free(a_enviar);
	eliminar_paquete(paquete);
}

void enviar_msjs_caught(t_socket_cliente *socket)
{
	t_msg_caught *msg_caught = malloc(sizeof(t_msg_caught));
	t_paquete* paquete = malloc(sizeof(t_paquete));
	msg_caught->id_correlativo = socket->cant_msg_enviados + 600;
	msg_caught->resultado = FAIL;
	paquete->codigo_operacion = CAUGHT_TEAM;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = sizeof(int) + sizeof(t_result_caught);
	paquete->buffer->data = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->data + offset, &(msg_caught->id_correlativo),sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, &(msg_caught->resultado),sizeof(t_result_caught));
	int bytes = paquete->buffer->size + 2 * sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);
	if (send(socket->cliente_fd, a_enviar, bytes, MSG_WAITALL) != bytes) {
		exit(EXIT_FAILURE);
	}
	log_info(g_logger,"(SENDING: CAuGHT_POKEMON|Socket#: %d|ID_CORRELATIVE=%d|RESULT=%s)",
			socket->cliente_fd, msg_caught->id_correlativo, result_caught(msg_caught->resultado));
	free(msg_caught);
	free(a_enviar);
	eliminar_paquete(paquete);
}

void enviar_msjs_localized(t_socket_cliente *socket)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	t_msg_localized *msg_localized = malloc(sizeof(t_msg_localized));
	int cant_coord = cant_coordenadas(socket->cant_msg_enviados);
	msg_localized->posiciones = generar_posiciones_localized(cant_coord);
	msg_localized->pokemon = "Venusaur";
	paquete->codigo_operacion = LOCALIZED_TEAM;
	paquete->buffer = malloc(sizeof(t_stream));
	int size_pokemon = strlen(msg_localized->pokemon) + 1;
	int cant_elem = msg_localized->posiciones->coordenadas->elements_count;
	paquete->buffer->size = (cant_elem + 1) * sizeof(int) + size_pokemon;
	paquete->buffer->data = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->data + offset, &(msg_localized->posiciones->cantidad),sizeof(int));
	//Serializamos Lista de posiciones.
	for (int i = 0 ;i < cant_elem; i ++) {
		int coord = coordenada_posiciones(msg_localized->posiciones,i);
		offset += sizeof(int);
		memcpy(paquete->buffer->data + offset, &coord,sizeof(int));
	}
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, msg_localized->pokemon, size_pokemon);

	int bytes = paquete->buffer->size + 2 * sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);
	char *concatenada = concat_coord(msg_localized->posiciones);
		if (send(socket->cliente_fd, a_enviar, bytes, MSG_WAITALL) != bytes) {
		exit(EXIT_FAILURE);
	}
	log_info(g_logger,"(SENDING: LOCALIZED_POKEMON|Socket#: %d|Pokemon: %s|POSIC_QTY=%d|COORDS:=%s)",
		socket->cliente_fd, msg_localized->pokemon, msg_localized->posiciones->cantidad, concatenada);
	free(concatenada);
	eliminar_msg_localized(msg_localized);
	free(a_enviar);
	eliminar_paquete(paquete);
}

t_posiciones_localized *generar_posiciones_localized(int cantidad)
{
	t_posiciones_localized *posiciones = malloc(sizeof(t_posiciones_localized));
	posiciones->cantidad = cantidad;
	int elem = cantidad * 2;
	t_list *lista = list_create();
	for(int i = 0; i < elem; i++){
		int valor = rand() % 99;
		void *elem = malloc(sizeof(int));
		memcpy(elem, &valor, sizeof(int));
		list_add(lista,elem);
	}
	posiciones->coordenadas = lista;
	return posiciones;
}

int coordenada_posiciones(t_posiciones_localized* posiciones, int indice)
{
		t_list *coordxy =  posiciones->coordenadas;
		void *elem = list_get(coordxy,indice);
		int valor;
		memcpy(&valor, elem, sizeof(int));
		return valor;
}

char  *concat_coord(t_posiciones_localized *posiciones)
{
	int cant_elem = posiciones->coordenadas->elements_count;
	t_list *coordenadas = posiciones->coordenadas;
	char *cadena = malloc(200);
	strcpy(cadena, "[");
	for (int i = 0; i < cant_elem; i ++) {
		int valor;
		void *elemento;
		elemento = list_get(coordenadas,i);
		memcpy(&valor, elemento, sizeof(int));
		char *num = string_itoa(valor);
		char nume[3];
		strcpy(nume,num);
		strcat(cadena,nume);
		if (i == cant_elem - 1) {
			strcat(cadena,"] ");
		}
		else {
			strcat(cadena,"|");
		}
		free(num);
	}
	return cadena;
}

void eliminar_msg_localized(t_msg_localized *msg_localized)
{
	list_destroy_and_destroy_elements(msg_localized->posiciones->coordenadas,(void*) free);
	free(msg_localized->posiciones);
	free(msg_localized);
}

char *result_caught(t_result_caught resultado)
{
	char *result;
	switch(resultado) {
	case OK:
		result = "OK";
		break;
	case FAIL:
		result = "FAIL";
		break;
	case -1:
		result = " ";
		break;
	}
	return result;
}

void enviar_suscript_end(t_socket_cliente *socket, int id_suscriptor)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = SUSCRIP_END;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = sizeof(int);
	paquete->buffer->data = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->data, &(id_suscriptor), paquete->buffer->size);
	int bytes = paquete->buffer->size + 2 * sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);
	if (send(socket->cliente_fd, a_enviar, bytes, MSG_WAITALL) != bytes) {
		exit(EXIT_FAILURE);
	}
	log_info(g_logger, "(SENDING: END_SUSCRIPTION|ID_SUSCRIPTOR= %d|SENT-MSGS= %d)", id_suscriptor, socket->cant_msg_enviados);
	free(a_enviar);
	eliminar_paquete(paquete);
}

int tamano_recibido(int bytes) {
	return bytes + 2 * sizeof(int);
}

int cant_coordenadas(int valor)
{
	int resultado = valor % 10;
	if (resultado < 2) {
		return 2;
	} else {
		return resultado;
	}
}

void eliminar_paquete(t_paquete *paquete) {
	free(paquete->buffer->data);
	free(paquete->buffer);
	free(paquete);
}

void iniciar_estructuras(void)
{
	g_suscriptores = list_create();
}

void eliminar_estructuras(void)
{
	if (g_suscriptores->elements_count != 0) {
		for (int i = 0; i < g_suscriptores->elements_count; i ++) {
			t_suscriptor *suscriptor = list_remove(g_suscriptores, i);
			free(suscriptor);
		}
	list_destroy(g_suscriptores);
	}

}

void iniciar_logger(void) {
	g_logger = log_create("/home/utnso/logs/server.log", "SERVER", 1, LOG_LEVEL_INFO);
}
