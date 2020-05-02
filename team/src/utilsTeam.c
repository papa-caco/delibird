/*
 * conexiones.c
 *
 *  Created on: 3 mar. 2019
 *      Author: utnso
 */

#include"utilsTeam.h"
#include"teamInitializer.c"


void iniciar_team(void){
	iniciar_logger();
	iniciar_servidor();
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

t_handsake_suscript *rcv_handshake_suscripcion(t_socket_cliente *socket,
		int *size) {
	t_handsake_suscript *handshake = malloc(sizeof(t_handsake_suscript));
	int socket_cliente = socket->cliente_fd;
	void *msg;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	msg = malloc(*size);
	recv(socket_cliente, msg, *size, MSG_WAITALL);
	int offset = 0;
	memcpy(&(handshake->valor_handshake), msg + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&(handshake->id_suscriptor), msg + offset, sizeof(int));
	handshake->msjs_recibidos = 0;
	free(msg);
	return handshake;
}

t_handsake_suscript *rcv_fin_suscripcion(t_socket_cliente *socket, int *size) {
	t_handsake_suscript *handshake = malloc(sizeof(t_handsake_suscript));
	void *msg;
	int socket_cliente = socket->cliente_fd;
	int cant_enviados = socket->cant_msg_enviados;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	msg = malloc(*size);
	recv(socket_cliente, msg, *size, MSG_WAITALL);
	int offset = 0;
	memcpy(&(handshake->id_suscriptor), msg + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&(handshake->msjs_recibidos), msg + offset, sizeof(int));
	handshake->valor_handshake = HANDSHAKE_SUSCRIPTOR;
	if (cant_enviados == handshake->msjs_recibidos) {
		log_info(g_logger,
				"(END_SUSCRIPTION_OK: ID_SUSCRIPTOR = %d | Socket# = %d | SENT_MSGs = %d)",
				handshake->id_suscriptor, socket_cliente, cant_enviados);
	} else {
		log_error(g_logger,
				"(END_SUSCRIPTION: ID_SUSCRIPTOR = %d | Socket# = %d | SENT_MSGs = %d != RCVD_MSGs = %d)",
				handshake->id_suscriptor, socket_cliente, cant_enviados,
				handshake->msjs_recibidos);

	}
	free(msg);
	return handshake;
}

void devolver_posiciones(int socket_cliente, char* pokemon,
		int* encontroPokemon) {

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

		char* line = NULL;
		size_t len = 0;
		ssize_t read = getline(&line, &len, posiciones);

		//Verifico que si el archivo esta vacio
		if (read == -1) {
			devolver_recepcion_fail(socket_cliente,
					"SE ENCONTRO EL ARCHIVO PERO ESTA VACIO");
			free(line);
			//Si tiene contenido, hago el flujo de carga
		} else {

			printf("Se encontró el archivo con contenido\n");
			t_config* config = config_create(
					"/home/utnso/config/gameboy.config");

			int idMensajeUnico = config_get_int_value(config,
					"ID_MENSAJE_UNICO");

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

			int cantidadDePosiciones = list_size(listaPosiciones);

			printf("La cantidad de posiciones en la lista es %d: \n",
					cantidadDePosiciones);

			int totalBytes = sizeof(int)
					+ cantidadDePosiciones * 3 * sizeof(int);

			printf("La cantidad de bytes son :%d \n", totalBytes);

			void* stream = malloc(totalBytes);

			int offset = 0;

			memcpy(stream + offset, &(idMensajeUnico), sizeof(int));
			offset += sizeof(int);

			printf("Comienzo a serializar\n");

			for (int procesados = 0; procesados < cantidadDePosiciones;
					procesados++) {

				t_posicion_pokemon* posicionActual = list_get(listaPosiciones,
						procesados);

				memcpy(stream + offset, &(posicionActual->cantidad),
						sizeof(int));
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
	memcpy(magic + desplazamiento, paquete->buffer->stream,
			paquete->buffer->size);
	desplazamiento += paquete->buffer->size;
	return magic;
}

void devolver_id_mensaje_propio(int socket_cliente) {
	t_paquete* paquete = malloc(sizeof(t_paquete));
	int id_mensaje = ID_MSG_RTA;
	log_info(g_logger, "(SENDING: ID_MENSAJE= %d)", id_mensaje);

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
	char *respuesta = RESPUESTA_OK;
	log_info(g_logger, "(SENDING: %s)", respuesta);

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

void devolver_recepcion_fail(int socket_cliente, char* mensajeError) {
	t_paquete* paquete = malloc(sizeof(t_paquete));

	log_info(g_logger, "(RESPUESTA = %s)", mensajeError);

	paquete->codigo_operacion = MSG_ERROR;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = strlen(mensajeError) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);

	memcpy(paquete->buffer->stream, mensajeError, paquete->buffer->size);

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

void enviar_cola_vacia(int socket_cliente, int id_suscriptor) {
	t_paquete* paquete = malloc(sizeof(t_paquete));
	log_info(g_logger, "(SENDING: EMPTY_QUEUE | ID_SUSCRIPTOR = %d)",
			id_suscriptor);
	paquete->codigo_operacion = COLA_VACIA;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = sizeof(int);
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, &(id_suscriptor), paquete->buffer->size);
	int bytes = paquete->buffer->size + 2 * sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);
	send(socket_cliente, a_enviar, bytes, 0);
	free(a_enviar);
	eliminar_paquete(paquete);
}

void enviar_msjs_get(int socket_cliente, int id_mensaje) {
	t_msg_get *msg_get = malloc(sizeof(t_msg_get));
	t_paquete* paquete = malloc(sizeof(t_paquete));
	msg_get->id_mensaje = id_mensaje + 1000;
	msg_get->pokemon = "Charizard";
	paquete->codigo_operacion = GET_GAMECARD;
	paquete->buffer = malloc(sizeof(t_stream));
	int size_pokemon = strlen(msg_get->pokemon) + 1;
	paquete->buffer->size = sizeof(int) + size_pokemon;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->stream + offset, &(msg_get->id_mensaje),
			sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->stream + offset, msg_get->pokemon, size_pokemon);

	int bytes = paquete->buffer->size + 2 * sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);
	log_info(g_logger,
			"(SENDING: GET_POKEMON | Socket# = %d | ID_MENSAJE = %d | Pokemon = %s)",
			socket_cliente, msg_get->id_mensaje, msg_get->pokemon);
	send(socket_cliente, a_enviar, bytes, 0);
	free(msg_get);
	free(a_enviar);
	eliminar_paquete(paquete);
}

void enviar_msjs_new(int socket_cliente, int id_mensaje) {
	t_msg_new *msg_new = malloc(sizeof(t_msg_new));
	t_paquete* paquete = malloc(sizeof(t_paquete));
	msg_new->id_mensaje = id_mensaje + 1100;
	msg_new->pos_x = 20;
	msg_new->pos_y = 25;
	msg_new->cantidad = 30;
	msg_new->pokemon = "Bulbasaur";
	paquete->codigo_operacion = NEW_GAMECARD;
	paquete->buffer = malloc(sizeof(t_stream));
	int size_pokemon = strlen(msg_new->pokemon) + 1;
	paquete->buffer->size = 4 * sizeof(int) + size_pokemon;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->stream + offset, &(msg_new->id_mensaje),
			sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->stream + offset, &(msg_new->pos_x), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->stream + offset, &(msg_new->pos_y), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->stream + offset, &(msg_new->cantidad), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->stream + offset, msg_new->pokemon, size_pokemon);
	int bytes = paquete->buffer->size + 2 * sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);
	log_info(g_logger,
			"(SENDING: NEW_POKEMON | Socket#: %d|ID_MSJ: = %d|Pokemon: %s|POS_X: %d|POS_Y: %d|QTY: %d)",
			socket_cliente, msg_new->id_mensaje, msg_new->pokemon,
			msg_new->pos_x, msg_new->pos_y, msg_new->cantidad);
	send(socket_cliente, a_enviar, bytes, 0);
	free(msg_new);
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

