/*
 * conexiones.c
 *
 *  Created on: 3 mar. 2019
 *      Author: utnso
 */

#include <delibird/estructuras.h>
#include "utils.h"

void atender_cliente_broker(t_socket_cliente_broker *socket) {
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
void process_request(op_code cod_op, t_socket_cliente_broker *socket) {
	int cliente_fd = socket->cliente_fd;
	int existePokemon;
	void* msg;

	switch (cod_op) {
	case CATCH_BROKER:;
		log_info(g_logger, "(RECEIVING: BROKER@CATCH_POKEMON | Socket#: %d)", cliente_fd);
		t_msg_catch_broker *msg_catch_broker = (t_msg_catch_broker*) msg;
		msg_catch_broker = rcv_msj_catch_broker(cliente_fd, g_logger);
		eliminar_msg_catch_broker(msg_catch_broker);
		enviar_id_mensaje(cliente_fd, g_logger, ID_MSG_RTA);
		// Devuelve al GAMEBOY un mensaje op_code = ID_MENSAJE c/id_mensaje propio
		break;
	case CATCH_GAMECARD:;
		log_info(g_logger, "(RECEIVING: GAMECARD@CATCH_POKEMON | Socket#: %d)",	cliente_fd);
		t_msg_catch_gamecard *msg_catch_gamecard = (t_msg_catch_gamecard*) msg;
		msg_catch_gamecard = rcv_msj_catch_gamecard(cliente_fd, g_logger);
		devolver_msj_caught_broker(msg_catch_gamecard, cliente_fd);
		eliminar_msg_catch_gamecard(msg_catch_gamecard);
		//El GameBoy tiene que recibir un mensaje op_code = CAUGHT_BROKER como respuesta
		break;
	case CAUGHT_BROKER:;
		log_info(g_logger, "(RECEIVING: BROKER@CAUGHT_POKEMON | Socket#: %d)", cliente_fd);
		t_msg_caught_broker *msg_caught_broker = (t_msg_caught_broker*) msg;
		msg_caught_broker = rcv_msj_caught_broker(cliente_fd, g_logger);
		free(msg_caught_broker);
		enviar_msg_confirmed(cliente_fd, g_logger);
		// El GameBoy no tiene que recibir ninguna repuesta en este tipo de mensaje.
		break;
	case GET_BROKER:;
		log_info(g_logger, "(RECEIVING: BROKER@GET_POKEMON | SOCKET#: %d)",	cliente_fd);
		t_msg_get_broker *msg_get_broker = (t_msg_get_broker*) msg;
		msg_get_broker = rcv_msj_get_broker(cliente_fd, g_logger);
		eliminar_msg_get_broker(msg_get_broker);
		enviar_id_mensaje(cliente_fd, g_logger, ID_MSG_RTA);
		// El GameBoy tiene que recibir un mensaje op_code = ID_MENSAJE con id_mensaje propio
		break;
	case GET_GAMECARD:;
		log_info(g_logger, "(RECEIVING: GAMECARD@GET_POKEMON | SOCKET#: %d)", cliente_fd);
		t_msg_get_gamecard *msg_get_gamecard = (t_msg_get_gamecard*) msg;
		msg_get_gamecard = rcv_msj_get_gamecard(cliente_fd, g_logger);
		int id_correlativo = msg_get_gamecard->id_mensaje;
		devolver_posiciones(cliente_fd, id_correlativo, msg_get_gamecard->pokemon, &existePokemon);
		log_info(g_logger, "VALOR ENCONTRO POKEMON: %d", existePokemon);
		//Verifico si encontro o no el archivo
		if (existePokemon == 0) {
			enviar_mensaje_error(cliente_fd, g_logger, "NO SE ENCONTRO EL ARCHIVO DEL POKEMON");
		}
		eliminar_msg_get_gamecard(msg_get_gamecard);
		break;
	case NEW_BROKER:;
		log_info(g_logger, "(RECEIVING: BROKER@NEW_POKEMON | SOCKET#: %d)", cliente_fd);
		t_msg_new_broker *msg_new_broker = (t_msg_new_broker*) msg;
		msg_new_broker = rcv_msj_new_broker(cliente_fd, g_logger);
		eliminar_msg_new_broker(msg_new_broker);
		enviar_msg_confirmed(cliente_fd, g_logger);
		// El GameBoy no tiene que recibir ninguna repuesta en este tipo de mensaje.
		break;
	case NEW_GAMECARD:;
		log_info(g_logger, "(RECEIVING: GAMECARD@NEW_POKEMON | SOCKET#: %d)", cliente_fd);
		t_msg_new_gamecard *msg_new_gamecard = (t_msg_new_gamecard*) msg;
		msg_new_gamecard = rcv_msj_new_gamecard(cliente_fd, g_logger);
		devolver_msj_appeared_broker(msg_new_gamecard, cliente_fd);
		eliminar_msg_new_gamecard(msg_new_gamecard);
		// El GameBoy tiene que recibir un mensaje op_code = APPEARED_BROKER
		break;
	case APPEARED_BROKER:;
		log_info(g_logger, "(RECEIVING: BROKER@APPEARED_POKEMON | Socket#: %d)", cliente_fd);
		t_msg_appeared_broker *msg_appeared_broker = (t_msg_appeared_broker*) msg;
		msg_appeared_broker = rcv_msj_appeared_broker(cliente_fd, g_logger);
		eliminar_msg_appeared_broker(msg_appeared_broker);
		enviar_msg_confirmed(cliente_fd, g_logger);
		// El GameBoy recibe RECIBIDO_OK para cerrar la comunicación
		break;
	case APPEARED_TEAM:;
		log_info(g_logger, "(RECEIVING: TEAM@APPEARED_POKEMON | Socket#: %d)", cliente_fd);
		t_msg_appeared_team *msg_appeared_team = (t_msg_appeared_team*) msg;
		msg_appeared_team = rcv_msj_appeared_team(cliente_fd, g_logger);
		eliminar_msg_appeared_team(msg_appeared_team);
		enviar_msg_confirmed(cliente_fd, g_logger);
		// El GameBoy no tiene que recibir ninguna repuesta en este tipo de mensaje.
		break;
	case FIN_SUSCRIPCION:;
		t_handsake_suscript *handshake = (t_handsake_suscript*) msg;
		handshake = rcv_msj_handshake_suscriptor(cliente_fd);
		log_info(g_logger,"(RECEIVING: END_SUSCRIPTION|ID_SUSCRIPTOR = %d|Socket# = %d)",handshake->id_suscriptor, cliente_fd);
		stop_suscription(g_suscriptores, handshake);
		enviar_msg_confirmed(cliente_fd, g_logger);
		free(handshake);
		break;
	case 0:
		pthread_exit(NULL);
	case -1:
		pthread_exit(NULL);
	}
	free(socket);
}

// MENSAJES QUE RECIBE en MODO SUSCRIPTOR
void process_suscript(op_code cod_op, t_socket_cliente_broker *socket) {
	int id_suscriptor;
	int cliente_fd = socket->cliente_fd;
	int cant_msg = socket->cant_msg_enviados;
	t_handsake_suscript *handshake;
	switch (cod_op) {
	case SUSCRIP_NEW:
		handshake = rcv_msj_handshake_suscriptor(cliente_fd);
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
				atender_cliente_broker(socket);
				free(socket);
			}
			else {
				enviar_msj_cola_vacia(socket, g_logger, id_suscriptor);
				sleep(1);
				free(handshake);
				atender_cliente_broker(socket);
				free(socket);
			}
		}
		socket->cant_msg_enviados = cant_msg;
		enviar_msj_suscript_end(socket, g_logger, id_suscriptor);
		remove_ended_suscriptor(g_suscriptores,id_suscriptor);
		free(handshake);
		free(socket);
		pthread_exit(EXIT_SUCCESS);
	case SUSCRIP_APPEARED:
		handshake = rcv_msj_handshake_suscriptor(cliente_fd);
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
				atender_cliente_broker(socket);
				free(socket);
			}
			else {
				enviar_msj_cola_vacia(socket, g_logger, id_suscriptor);
				sleep(15);
				free(handshake);
				atender_cliente_broker(socket);
				free(socket);
			}
		}
		socket->cant_msg_enviados = cant_msg;
		enviar_msj_suscript_end(socket, g_logger, id_suscriptor);
		remove_ended_suscriptor(g_suscriptores,id_suscriptor);
		free(handshake);
		free(socket);
		pthread_exit(EXIT_SUCCESS);
	case SUSCRIP_CATCH:
		handshake = rcv_msj_handshake_suscriptor(cliente_fd);
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
				atender_cliente_broker(socket);
				free(socket);
			}
			else {
				enviar_msj_cola_vacia(socket, g_logger, id_suscriptor);
				sleep(15);
				free(handshake);
				atender_cliente_broker(socket);
				free(socket);
			}
		}
		socket->cant_msg_enviados = cant_msg;
		enviar_msj_suscript_end(socket, g_logger, id_suscriptor);
		remove_ended_suscriptor(g_suscriptores,id_suscriptor);
		free(handshake);
		free(socket);
		pthread_exit(EXIT_SUCCESS);
	case SUSCRIP_CAUGHT:
		handshake = rcv_msj_handshake_suscriptor(cliente_fd);
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
				atender_cliente_broker(socket);
				free(socket);
			}
			else {
				enviar_msj_cola_vacia(socket, g_logger, id_suscriptor);
				sleep(15);
				free(handshake);
				atender_cliente_broker(socket);
				free(socket);
			}
		}
		socket->cant_msg_enviados = cant_msg;
		enviar_msj_suscript_end(socket, g_logger, id_suscriptor);
		remove_ended_suscriptor(g_suscriptores,id_suscriptor);
		free(handshake);
		free(socket);
		pthread_exit(EXIT_SUCCESS);
	case SUSCRIP_GET:
		handshake = rcv_msj_handshake_suscriptor(cliente_fd);
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
				atender_cliente_broker(socket);
				free(socket);
			}
			else {
				enviar_msj_cola_vacia(socket, g_logger, id_suscriptor);
				sleep(15);
				free(handshake);
				atender_cliente_broker(socket);
				free(socket);
			}
		}
		socket->cant_msg_enviados = cant_msg;
		enviar_msj_suscript_end(socket, g_logger, id_suscriptor);
		remove_ended_suscriptor(g_suscriptores,id_suscriptor);
		free(handshake);
		free(socket);
		pthread_exit(EXIT_SUCCESS);
	case SUSCRIP_LOCALIZED:
		handshake = rcv_msj_handshake_suscriptor(cliente_fd);
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
				atender_cliente_broker(socket);
				free(socket);
			}
			else {
				enviar_msj_cola_vacia(socket, g_logger, id_suscriptor);
				sleep(5);
				free(handshake);
				atender_cliente_broker(socket);
				free(socket);
			}
		}
		socket->cant_msg_enviados = cant_msg;
		enviar_msj_suscript_end(socket, g_logger, id_suscriptor);
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

void start_suscription(t_list *suscriptores,t_handsake_suscript *handshake)
{
	t_suscriptor_broker *suscriptor=malloc(sizeof(t_suscriptor_broker));
	suscriptor->id_suscriptor = handshake->id_suscriptor;
	suscriptor->cant_msg = handshake->msjs_recibidos;
	suscriptor->is_active = 1;
	list_add(suscriptores,suscriptor);
}

void stop_suscription(t_list *suscriptores, t_handsake_suscript *handshake)
{
	int id_suscriptor = handshake->id_suscriptor;
	t_suscriptor_broker *suscriptor = obtengo_suscriptor(suscriptores , id_suscriptor);
	suscriptor->is_active = 0;
}

void remove_ended_suscriptor(t_list *suscriptores, int id_suscriptor)
{
	t_suscriptor_broker *suscriptor = obtengo_suscriptor(suscriptores , id_suscriptor);
	delete_suscriptor(suscriptores, suscriptor);
	free(suscriptor);
}

void delete_suscriptor(t_list *suscriptores, void *suscriptor)
{
	t_suscriptor_broker *suscrip_t = (t_suscriptor_broker*) suscriptor;
	bool compare_suscript(void *otro_suscriptor){
		t_suscriptor_broker *otro_suscrip_t = (t_suscriptor_broker*) otro_suscriptor;
		bool condition = otro_suscrip_t->id_suscriptor == suscrip_t->id_suscriptor;
		return condition;
	}
	list_remove_by_condition(suscriptores, compare_suscript);
}

bool is_active_suscriptor(t_list *suscriptores, int id_suscriptor)
{
	t_suscriptor_broker *suscriptor = obtengo_suscriptor(suscriptores, id_suscriptor);
	return suscriptor->is_active;
}

t_suscriptor_broker *obtengo_suscriptor(t_list *suscriptores, int id_suscriptor)
{
	bool mismo_id_suscriptor(void *suscriptor)
	{
		t_suscriptor_broker* t_suscript = (t_suscriptor_broker*) suscriptor;
		bool condition = id_suscriptor == t_suscript->id_suscriptor;
		return condition;
	}
	t_suscriptor_broker *suscriptor_buscado;
	suscriptor_buscado = list_find(suscriptores, mismo_id_suscriptor);
	if (suscriptor_buscado != NULL) {
		return suscriptor_buscado;
	} else {
		log_error(g_logger,"(ERROR|SUSCRIPTOR NO_EXISTE)");
		return NULL;
	}
}

void devolver_posiciones(int socket_cliente, int id_correlativo, char* pokemon,	int* encontroPokemon) {

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

		char *line = malloc(1);
		size_t len = 0;
		ssize_t read = getline(&line, &len, posiciones);

		//Verifico que si el archivo esta vacio
		if (read == -1) {
			enviar_mensaje_error(socket_cliente, g_logger, "SE ENCONTRO EL ARCHIVO PERO ESTA VACIO");
			//Si tiene contenido, hago el flujo de carga
		} else {
			free(line);
			printf("Se encontró el archivo con contenido\n");
			t_msg_localized_broker *msg_localized_broker = malloc(sizeof(t_msg_localized_broker));
			msg_localized_broker->id_correlativo = id_correlativo;
			msg_localized_broker->posiciones = malloc(sizeof(t_posiciones_localized));
			msg_localized_broker->posiciones->cant_posic = 0;
			msg_localized_broker->posiciones->coordenadas = list_create();
			line = malloc(len);
			rewind(posiciones);
			while ((read = getline(&line, &len, posiciones)) != -1){
				printf("Entro a leer lista\n");

				char** keyValue = string_split(line, "=");
				free(line);
				printf("Luego de primer split\n");

				char* key = keyValue[0];

				int cantidad = atoi(keyValue[1]);

				char** posiciones = string_split(key, "-");

				printf("Luego de segundo split\n");

				int posicionX = atoi(posiciones[0]);
				int posicionY = atoi(posiciones[1]);

				/*t_posicion_pokemon *posicion = malloc(sizeof(t_posicion_pokemon));

				posicion->cantidad = cantidad;
				posicion->pos_x = posicionX;
				posicion->pos_y = posicionY;*/

				printf("Pokemon %s : \n", pokemon);
				printf("Posicion X: %d \n", posicionX);
				printf("Posicion Y: %d \n", posicionY);
				printf("Cantidad: %d \n", cantidad);
				printf("------------------------------ \n");
				t_coordenada *coord_xy = malloc(sizeof(t_coordenada));
				coord_xy->pos_x = posicionX;
				coord_xy->pos_y = posicionY;
				list_add(msg_localized_broker->posiciones->coordenadas, coord_xy);
				msg_localized_broker->posiciones->cant_posic += 1;
				liberar_listas(keyValue);
				liberar_listas(posiciones);
				line = malloc(len);
			}
			free(line);

			printf("Salgo de leer lista\n");

			int size_pokemon = strlen(pokemon) + 1;
			msg_localized_broker->pokemon = malloc(size_pokemon);
			printf("pokemon:%s -tamaño:%d\n", pokemon, size_pokemon);
			memcpy(msg_localized_broker->pokemon, pokemon, size_pokemon);

			printf("La cantidad de posiciones en la lista es %d: \n",
					msg_localized_broker->posiciones->cant_posic);
			enviar_msj_localized_al_broker(msg_localized_broker);
			enviar_msj_localized_broker(socket_cliente, g_logger, msg_localized_broker);



			txt_close_file(posiciones);
		}


	} else {
		//Asigno 0 haciendo referencia a que no encontro el archivo
		*encontroPokemon = 0;
	}
	free(ruta);
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

void devolver_msj_caught_broker(t_msg_catch_gamecard *msg_catch_gamecard, int cliente_fd)
{
	t_msg_caught_broker *msg_caught_broker = malloc(sizeof(t_msg_caught_broker));
	msg_caught_broker->id_correlativo = msg_catch_gamecard->id_mensaje;
	msg_caught_broker->resultado = OK;
	enviar_msj_caught_broker(cliente_fd, g_logger, msg_caught_broker);
	free(msg_caught_broker);
}

void devolver_msj_appeared_broker(t_msg_new_gamecard *msg_new_gamecard, int socket_cliente)
{
	t_msg_appeared_broker *msg_appeared_broker = malloc(sizeof(t_msg_appeared_broker));
	msg_appeared_broker->coordenada = malloc(sizeof(t_coordenada));
	int size_pokemon = strlen(msg_new_gamecard->pokemon) + 1;
	msg_appeared_broker->pokemon = malloc(size_pokemon);
	msg_appeared_broker->id_correlativo = msg_new_gamecard->id_mensaje;
	msg_appeared_broker->coordenada->pos_x = msg_new_gamecard->coord->pos_x;
	msg_appeared_broker->coordenada->pos_y = msg_new_gamecard->coord->pos_y;
	memcpy(msg_appeared_broker->pokemon,msg_new_gamecard->pokemon,size_pokemon);
	enviar_msj_appeared_broker(socket_cliente, g_logger, msg_appeared_broker);
	eliminar_msg_appeared_broker(msg_appeared_broker);
}

void enviar_msjs_get(t_socket_cliente_broker *socket)
{
	t_msg_get_gamecard *msg_get = malloc(sizeof(t_msg_get_gamecard));
	msg_get->id_mensaje = socket->cant_msg_enviados + 700;
	msg_get->pokemon = "Charizard";
	enviar_msj_get_gamecard(socket, g_logger, msg_get);
	free(msg_get);
}

void enviar_msjs_new(t_socket_cliente_broker *socket)
{
	t_msg_new_gamecard *msg_new = malloc(sizeof(t_msg_new_gamecard));
	msg_new->coord = malloc(sizeof(t_coordenada));
	msg_new->id_mensaje = socket->cant_msg_enviados + 550;
	msg_new->coord->pos_x = 25;
	msg_new->coord->pos_y = 52;
	msg_new->cantidad = 73;
	msg_new->pokemon = "Bulbasaur";
	enviar_msj_new_gamecard(socket, g_logger, msg_new);
	free(msg_new->coord);
	free(msg_new);
}

void enviar_msjs_catch(t_socket_cliente_broker *socket)
{
	t_msg_catch_gamecard *msg_catch = malloc(sizeof(t_msg_catch_gamecard));
	msg_catch->coord = malloc(sizeof(t_coordenada));
	msg_catch->id_mensaje = socket->cant_msg_enviados + 600;
	msg_catch->coord->pos_x = 78;
	msg_catch->coord->pos_y = 39;
	msg_catch->pokemon = "Charmeleon";
	enviar_msj_catch_gamecard(socket, g_logger, msg_catch);
	free(msg_catch->coord);
	free(msg_catch);
}

void enviar_msjs_appeared(t_socket_cliente_broker *socket)
{
	t_msg_appeared_team *msg_appeared = malloc(sizeof(t_msg_appeared_team));
	msg_appeared->id_mensaje = socket->cant_msg_enviados + 360;
	msg_appeared->coord = malloc(sizeof(t_coordenada));
	msg_appeared->coord->pos_x = 20;
	msg_appeared->coord->pos_y = 25;
	msg_appeared->pokemon = "Wartortle";
	enviar_msj_appeared_team(socket, g_logger, msg_appeared);
	free(msg_appeared->coord);
	free(msg_appeared);
}

void enviar_msjs_caught(t_socket_cliente_broker *socket)
{
	t_msg_caught_team *msg_caught = malloc(sizeof(t_msg_caught_team));
	msg_caught->id_mensaje = socket->cant_msg_enviados + 601;
	msg_caught->id_correlativo = socket->cant_msg_enviados + 600;
	msg_caught->resultado = FAIL;
	enviar_msj_caught_team(socket, g_logger, msg_caught);
}

void enviar_msjs_localized(t_socket_cliente_broker *socket)
{
	t_msg_localized_team *msg_localized = malloc(sizeof(t_msg_localized_team));
	msg_localized->id_mensaje = socket->cant_msg_enviados + 931;
	msg_localized->id_correlativo = socket->cant_msg_enviados + 930;
	int cant_coord = cant_coordenadas(socket->cant_msg_enviados);
	msg_localized->posiciones = generar_posiciones_localized(cant_coord);
	msg_localized->pokemon = "Venusaur";
	enviar_msj_localized_team(socket, g_logger, msg_localized);
	list_destroy_and_destroy_elements(msg_localized->posiciones->coordenadas,(void*)free);
	free(msg_localized->posiciones);
	free(msg_localized);
}

void enviar_msj_localized_al_broker(t_msg_localized_broker *msg_localized)
{
	pthread_create(&thread, NULL, (void*) connect_broker_send_msg, msg_localized);
	pthread_detach(thread);
}

void connect_broker_send_msg(t_msg_localized_broker *msg_localized)
{
	int cliente_fd = crear_conexion(IP_BROKER, PUERTO_BROKER, g_logger, "BROKER", "LOCALIZED_POKEMON");
	if (cliente_fd > 0) {
		enviar_msj_localized_broker(cliente_fd, g_logger, msg_localized);
		int cod_operacion = rcv_codigo_operacion(cliente_fd);
		if (cod_operacion == ID_MENSAJE) {
			rcv_id_mensaje(cliente_fd, g_logger);
		}
		close(cliente_fd);
	}
	eliminar_msg_localized_broker(msg_localized);
}

t_posiciones_localized *generar_posiciones_localized(int cantidad)
{
	t_posiciones_localized *posiciones = malloc(sizeof(t_posiciones_localized));
	posiciones->cant_posic = cantidad;
	posiciones->coordenadas = list_create();
	for(int i = 0; i < cantidad; i++){
		t_coordenada *coord = malloc(sizeof(t_coordenada));
		coord->pos_x = rand() % 99;
		coord->pos_y = rand() % 99;
		list_add(posiciones->coordenadas,coord);
	}
	return posiciones;
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

void iniciar_estructuras(void)
{
	g_suscriptores = list_create();
}

void eliminar_estructuras(void)
{
	if (g_suscriptores->elements_count != 0) {
		for (int i = 0; i < g_suscriptores->elements_count; i ++) {
			t_suscriptor_broker *suscriptor = list_remove(g_suscriptores, i);
			free(suscriptor);
		}
	list_destroy(g_suscriptores);
	}
}

void iniciar_logger(void) {
	g_logger = log_create("/home/utnso/logs/server.log", "SERVER", 1, LOG_LEVEL_INFO);
}
