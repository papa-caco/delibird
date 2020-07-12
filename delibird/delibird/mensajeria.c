/*
 * mensajeria.c
 *
 *  Created on: 4 may. 2020
 *
 *  Author: Los-Que-Aprueban
 *
 */
#include "mensajeria.h"

ssize_t enviar_msj_cola_vacia(t_socket_cliente_broker *socket, t_log *logger, int id_suscriptor)
{
	t_paquete* paquete = empaquetar_msj_cola_vacia(id_suscriptor);
	ssize_t sent_bytes = 0;
	int bytes = tamano_paquete(paquete);
	void* a_enviar = serializar_paquete(paquete, bytes);
	eliminar_paquete(paquete);
	sent_bytes = send(socket->cliente_fd, a_enviar, bytes, MSG_WAITALL);
	if (sent_bytes != bytes) {
		log_error(logger, "(SENDING: EMPTY_QUEUE FAILED)");
	} else {
		log_warning(logger, "(SENDING|Socket:%d|EMPTY_QUEUE -- No More Messages...|ID_SUSCRIPTOR:%d)",
			socket->cliente_fd, id_suscriptor);
	}
	free(a_enviar);
	return sent_bytes;
}

void enviar_msj_suscript_end(t_socket_cliente_broker *socket, t_log * logger, int id_suscriptor)
{
	t_paquete* paquete = empaquetar_msj_suscript_end(id_suscriptor);
	int bytes = tamano_paquete(paquete);
	void* a_enviar = serializar_paquete(paquete, bytes);
	eliminar_paquete(paquete);
	if (send(socket->cliente_fd, a_enviar, bytes, MSG_WAITALL) != bytes) {
		log_error(logger, "(SENDING: SUSCRIPTION_ENDED FAILED)");
	} else {
		log_debug(logger, "(SENDING|Socket:%d|SUSCRIPTION_ENDED|ID_SUSCRIPTOR:%d|SENT-MSGS:%d)",
			socket->cliente_fd, id_suscriptor, socket->cant_msg_enviados);
	}
	free(a_enviar);
}

ssize_t enviar_msj_get_gamecard(t_socket_cliente_broker *socket,t_log *logger,t_msg_get_gamecard *msg_get_gamecard)
{
	t_paquete *paquete = empaquetar_msg_get_gamecard(msg_get_gamecard);
	ssize_t sent_bytes = 0;
	int bytes = tamano_paquete(paquete);
	void* a_enviar = serializar_paquete(paquete, bytes);
	eliminar_paquete(paquete);
	sent_bytes = send(socket->cliente_fd, a_enviar, bytes, MSG_WAITALL);
	if ( sent_bytes != bytes) {
		log_error(logger, "(SENDING: GET_POKEMON FAILED)");
	} else {
		log_info(logger, "(SENDING|Socket:%d|GET_POKEMON|ID_MENSAJE:%d|%s|SIZE:%d Bytes)",
		socket->cliente_fd, msg_get_gamecard->id_mensaje, msg_get_gamecard->pokemon, bytes);
	}
	free(a_enviar);
	return sent_bytes;
}

ssize_t enviar_msj_new_gamecard(t_socket_cliente_broker *socket,t_log *logger,t_msg_new_gamecard *msg_new_gamecard)
{
	t_paquete *paquete = empaquetar_msg_new_gamecard(msg_new_gamecard);
	ssize_t sent_bytes = 0;
	int bytes = tamano_paquete(paquete);
	void* a_enviar = serializar_paquete(paquete, bytes);
	eliminar_paquete(paquete);
	sent_bytes = send(socket->cliente_fd, a_enviar, bytes, MSG_WAITALL);
 	if ( sent_bytes != bytes) {
		log_error(logger, "(SENDING: NEW_POKEMON FAILED)");
	} else {
		log_info(logger,"(SENDING|Socket:%d|NEW_POKEMON|ID_MENSAJE:%d|%s|POS_X:%d|POS_Y:%d|QTY:%d|SIZE:%d Bytes)",
			socket->cliente_fd, msg_new_gamecard->id_mensaje,msg_new_gamecard->pokemon, msg_new_gamecard->coord->pos_x,
			msg_new_gamecard->coord->pos_y, msg_new_gamecard->cantidad,  bytes);
	}
	free(a_enviar);
	return sent_bytes;
}

ssize_t enviar_msj_catch_gamecard(t_socket_cliente_broker *socket,t_log *logger,t_msg_catch_gamecard *msg_catch_gamecard)
{
	t_paquete* paquete = empaquetar_msg_catch_gamecard(msg_catch_gamecard);
	ssize_t sent_bytes = 0;
	int bytes = tamano_paquete(paquete);
	void* a_enviar = serializar_paquete(paquete, bytes);
	eliminar_paquete(paquete);
	sent_bytes = send(socket->cliente_fd, a_enviar, bytes, MSG_WAITALL);
	if (sent_bytes != bytes) {
		log_error(logger, "(SENDING: CATCH_POKEMON FAILED)");
	} else {
		log_info(logger,"(SENDING|Socket:%d|CATCH_POKEMON|ID_MESSAGE:%d|%s|POS_X:%d|POS_Y:%d)",
			socket->cliente_fd, msg_catch_gamecard->id_mensaje, msg_catch_gamecard->pokemon,
			msg_catch_gamecard->coord->pos_x, msg_catch_gamecard->coord->pos_y);
	}
	free(a_enviar);
	return sent_bytes;
}

ssize_t enviar_msj_appeared_team(t_socket_cliente_broker *socket, t_log *logger, t_msg_appeared_team *msg_appeared_team)
{
	t_paquete *paquete = empaquetar_msg_appeared_team(msg_appeared_team);
	int bytes = tamano_paquete(paquete);
	void* a_enviar = serializar_paquete(paquete, bytes);
	eliminar_paquete(paquete);
	ssize_t sent_bytes = send(socket->cliente_fd, a_enviar, bytes, MSG_WAITALL);
	if (sent_bytes != bytes) {
		log_error(logger, "(SENDING APPEARED_POKEMON FAILED)");
	} else {
		log_info(logger,"(SENDING|Socket:%d|APPEARED_POKEMON|ID_MSG:%d|ID_CORRELATIVE:%d|%s|POS_X:%d|POS_Y:%d|SIZE:%d Bytes)",
			socket->cliente_fd, msg_appeared_team->id_mensaje,msg_appeared_team->id_correlativo,msg_appeared_team->pokemon,
			msg_appeared_team->coord->pos_x, msg_appeared_team->coord->pos_y, bytes);
	}
	free(a_enviar);
	return sent_bytes;
}

ssize_t enviar_msj_caught_team(t_socket_cliente_broker *socket, t_log *logger, t_msg_caught_team *msg_caught_team)
{
	t_paquete* paquete = empaquetar_msg_caught_team(msg_caught_team);
	ssize_t sent_bytes;
	int bytes = tamano_paquete(paquete);
	void* a_enviar = serializar_paquete(paquete, bytes);
	eliminar_paquete(paquete);
	sent_bytes = send(socket->cliente_fd, a_enviar, bytes, MSG_WAITALL);
	if (sent_bytes != bytes) {
		log_error(logger, "(SENDING: CAUGHT_POKEMON FAILED)");
	} else {
		log_info(logger,"(SENDING|Socket:%d|CAUGHT_POKEMON|ID_MSG:%d|ID_CORRELATIVE:%d|RESULT:%s)",
			socket->cliente_fd, msg_caught_team->id_mensaje, msg_caught_team->id_correlativo, result_caught(msg_caught_team->resultado));
		free(msg_caught_team);
	}
	free(a_enviar);
	return sent_bytes;
}

ssize_t enviar_msj_localized_team(t_socket_cliente_broker *socket, t_log *logger, t_msg_localized_team *msg_localized_team)
{
	t_paquete* paquete = empaquetar_msg_localized_team(msg_localized_team);
	ssize_t sent_bytes;
	int bytes = tamano_paquete(paquete);
	void* a_enviar = serializar_paquete(paquete, bytes);
	eliminar_paquete(paquete);
	char *concatenada = concat_posiciones(msg_localized_team->posiciones->coordenadas);
	sent_bytes = send(socket->cliente_fd, a_enviar, bytes, MSG_WAITALL);
	if (sent_bytes != bytes) {
		log_error(logger, "(SENDING: LOCALIZED_POKEMON FAILED)");
	} else {
		log_info(logger,"(SENDING|Socket:%d|LOCALIZED_POKEMON|ID_MSG:%d|ID_CORRELATIVE:%d|%s|POSIC_QTY=%d|{Xn|Yn}:=%s|SIZE:%d)",
			socket->cliente_fd,  msg_localized_team->id_mensaje,msg_localized_team->id_correlativo,
			msg_localized_team->pokemon, msg_localized_team->posiciones->cant_posic, concatenada,bytes);
		free(concatenada);
	}
	free(a_enviar);
	return sent_bytes;
}

void enviar_msj_appeared_broker(int cliente_fd, t_log *logger, t_msg_appeared_broker *msg_appeared_broker)
{
	t_paquete *paquete = empaquetar_msg_appeared_broker(msg_appeared_broker);
	int bytes = tamano_paquete(paquete);
	void* a_enviar = serializar_paquete(paquete, bytes);
	eliminar_paquete(paquete);
	if (send(cliente_fd, a_enviar, bytes, MSG_WAITALL) != bytes) {
		log_error(logger, "(SENDING: APPEARED_POKEMON FAILED)");
	} else {
		log_info(logger,"(SENDING: APPEARED_POKEMON|%s|ID_CORRELATIVE:%d|Pos_X:%lu|Pos_Y:%lu|SIZE:%d Bytes)",
			msg_appeared_broker->pokemon, msg_appeared_broker->id_correlativo,
			msg_appeared_broker->coordenada->pos_x, msg_appeared_broker->coordenada->pos_y, bytes);
	}
	free(a_enviar);
}

void enviar_msj_caught_broker(int cliente_fd, t_log *logger, t_msg_caught_broker *msg_caught_broker)
{
	t_paquete* paquete = empaquetar_msg_caught_broker(msg_caught_broker);
	int bytes = tamano_paquete(paquete);
	void* a_enviar = serializar_paquete(paquete, bytes);
	eliminar_paquete(paquete);
	if (send(cliente_fd, a_enviar, bytes, MSG_WAITALL) != bytes) {
		log_error(logger, "(SENDING: CAUGHT_POKEMON FAILED)");
	} else {
		log_info(logger,"(SENDING: CAUGHT_POKEMON|ID_CORRELATIVE=%d|RESULT=%s|Size:%d Bytes)",
			msg_caught_broker->id_correlativo, result_caught(msg_caught_broker->resultado),bytes);
	}
	free(a_enviar);
}

void enviar_msj_localized_broker(int cliente_fd, t_log *logger, t_msg_localized_broker *msg_localized_broker)
{
	t_paquete* paquete = empaquetar_msg_localized_broker(msg_localized_broker);
	int bytes = tamano_paquete(paquete);
	void* a_enviar = serializar_paquete(paquete, bytes);
	eliminar_paquete(paquete);
	if (send(cliente_fd, a_enviar, bytes, MSG_WAITALL) != bytes) {
		log_error(logger, "(SENDING: LOCALIZED_POKEMON FAILED)");
	} else {
		char *concatenada = concat_posiciones(msg_localized_broker->posiciones->coordenadas);
		log_info(logger,"(SENDING: LOCALIZED_POKEMON|Socket#:%d|ID_CORRELATIVE=%d|%s|QTY_POS:%d|{Xn|Yn}:%s|SIZE:%d Bytes)",
				cliente_fd, msg_localized_broker->id_correlativo, msg_localized_broker->pokemon,
				msg_localized_broker->posiciones->cant_posic,concatenada, bytes);
		free(concatenada);
	}
	free(a_enviar);
}

void enviar_msj_get_broker(int cliente_fd, t_log *logger,t_msg_get_broker *msg_get_broker)
{
	t_paquete *paquete = empaquetar_msg_get_broker(msg_get_broker);
	int bytes = tamano_paquete(paquete);
	void* a_enviar = serializar_paquete(paquete, bytes);
	eliminar_paquete(paquete);
	if (send(cliente_fd, a_enviar, bytes, MSG_WAITALL) != bytes) {
		log_error(logger, "(SENDING: GET_POKEMON FAILED)");
	} else {
		log_info(logger, "(SENDING: GET_POKEMON|%s|Size:%d Bytes)", msg_get_broker->pokemon, bytes);
	}
	free(a_enviar);
}

void enviar_msj_catch_broker(int cliente_fd, t_log *logger, t_msg_catch_broker *msg_catch_broker)
{
	t_paquete *paquete = empaquetar_msg_catch_broker(msg_catch_broker);
	int bytes = tamano_paquete(paquete);
	void *a_enviar = serializar_paquete(paquete, bytes);
	eliminar_paquete(paquete);
	if (send(cliente_fd, a_enviar, bytes, MSG_WAITALL) != bytes) {
		log_error(logger, "(SENDING: CATCH_POKEMON FAILED)");
	} else {
		log_info(logger, "(SENDING: CATCH_POKEMON|%s|POS_X:%d|POS_Y:%d|SIZE:%d Bytes)",
				msg_catch_broker->pokemon, msg_catch_broker->coordenada->pos_x, msg_catch_broker->coordenada->pos_y, bytes);
	}
	free(a_enviar);
}

void enviar_msj_new_broker(int cliente_fd, t_log *logger, t_msg_new_broker *msg_new_broker)
{
	t_paquete *paquete = empaquetar_msg_new_broker(msg_new_broker);
	int bytes = tamano_paquete(paquete);
	void* a_enviar = serializar_paquete(paquete, bytes);
	eliminar_paquete(paquete);
	if (send(cliente_fd, a_enviar, bytes, MSG_WAITALL) != bytes) {
		exit(EXIT_FAILURE);
	} else {
		log_info(logger,"(SENDING: NEW_POKEMON|%s|POS_X:%d|POS_Y:%d|QTY=%d|SIZE: %d Bytes)",
			msg_new_broker->pokemon, msg_new_broker->coordenada->pos_x,
			msg_new_broker->coordenada->pos_y, msg_new_broker->cantidad, bytes);
	}
	free(a_enviar);
}

void enviar_solicitud_fin_suscripcion(int socket_cliente, t_log *logger, t_handsake_suscript *handshake)
{
	void *a_enviar;
	t_paquete *paquete = empaquetar_fin_suscripcion(handshake);
	int bytes = tamano_paquete(paquete);
	a_enviar = serializar_paquete(paquete, bytes);
	eliminar_paquete(paquete);
	if (send(socket_cliente, a_enviar, bytes, MSG_WAITALL) != bytes) {
		exit(EXIT_FAILURE);
	} else {
		char *name_cola = nombre_cola(handshake->cola_id);
		log_debug(logger, "(REQUIRING END_SUSCRIPTION_TO: %s|ID_SUSCRIPTOR:%d)",
			name_cola, handshake->id_suscriptor);
	}
	free(a_enviar);
}

void enviar_mensaje_error(int socket_cliente, t_log *logger, char *mensajeError) {
	t_paquete* paquete = empaquetar_msg_fail(mensajeError);
	int bytes = tamano_paquete(paquete);
	void* a_enviar = serializar_paquete(paquete, bytes);
	eliminar_paquete(paquete);
	if (send(socket_cliente, a_enviar, bytes, 0) != bytes) {
		exit(EXIT_FAILURE);
	} else {
		log_warning(logger,"(SENDING_TO Socket:%d|%s)", socket_cliente, mensajeError);
	}
	free(a_enviar);
}

void enviar_msg_confirmed(int socket_cliente, t_log *logger) {
	t_paquete* paquete = empaquetar_msg_confirmed();
	int bytes = tamano_paquete(paquete);
	void* a_enviar = serializar_paquete(paquete, bytes);
	eliminar_paquete(paquete);
	if (send(socket_cliente, a_enviar, bytes, MSG_WAITALL) != bytes) {
		log_error(logger, "(SEND_TO Socket:%d|MSG_ERROR)", socket_cliente);
	} else {
		log_info(logger, "(SENDING_TO Socket:%d|MSG_CONFIRMED)", socket_cliente);
	}
	free(a_enviar);
}

void enviar_id_mensaje(int socket_cliente, t_log *logger, int id_mensaje)
{
	t_paquete* paquete = empaquetar_id_mensaje(id_mensaje);
	int bytes = tamano_paquete(paquete);
	void* a_enviar = serializar_paquete(paquete, bytes);
	eliminar_paquete(paquete);
	if (send(socket_cliente, a_enviar, bytes, 0) != bytes) {
		exit(EXIT_FAILURE);
	} else {
		log_info(logger, "(SENDING_TO Socket:%d|ID_MSG:%d)",socket_cliente, id_mensaje);
	}
	free(a_enviar);
}

void enviar_msj_handshake_suscriptor(int socket_cliente, t_log *logger, t_handsake_suscript *handshake)
{
	t_paquete* paquete = empaquetar_msg_handshake_suscript(handshake);
	int bytes = tamano_paquete(paquete);
	void* a_enviar = serializar_paquete(paquete, bytes);
	eliminar_paquete(paquete);
	if (send(socket_cliente, a_enviar, bytes, MSG_WAITALL) != bytes) {
		exit(EXIT_FAILURE);
	} else {
		char *cola = nombre_cola(handshake->cola_id);
		log_info(logger,"(SENDING ACK|%s|ID_MSG:%d->CONFIRMED|ID_SUSCRIPTOR:%d)",
			cola, handshake->id_recibido, handshake->id_suscriptor);
	}
	free(a_enviar);
}

char *concat_posiciones(t_list *posiciones)
{
	int cant_elem = posiciones->elements_count;
		char *cadena = malloc(200);
	strcpy(cadena, "[");
	for (int i = 0; i < cant_elem; i ++) {
		t_coordenada *coord_xy =  list_get(posiciones,i);
		int pos_x,pos_y;
		memcpy(&pos_x,&coord_xy->pos_x, sizeof(int));
		memcpy(&pos_y,&coord_xy->pos_y, sizeof(int));
		char *posx = string_itoa(pos_x);
		char *posy = string_itoa(pos_y);
		char auxiliar[3];
		strcpy(auxiliar,posx);
		strcat(cadena,auxiliar);
		strcat(cadena,"|");
		strcpy(auxiliar,posy);
		strcat(cadena,auxiliar);
		if (i == cant_elem - 1) {
			strcat(cadena,"]");
		}
		else {
			strcat(cadena,"|");
		}
		free(posx);
		free(posy);
	}
	return cadena;
}

void eliminar_msg_localized_broker(t_msg_localized_broker *msg_localized_broker)
{
	list_destroy_and_destroy_elements(msg_localized_broker->posiciones->coordenadas,(void*) free);
	free(msg_localized_broker->posiciones);
	free(msg_localized_broker->pokemon);
	free(msg_localized_broker);
}

void eliminar_msg_localized_team(t_msg_localized_team *msg_localized)
{
	list_destroy_and_destroy_elements(msg_localized->posiciones->coordenadas,(void*) free);
	free(msg_localized->posiciones);
	free(msg_localized->pokemon);
	free(msg_localized);
}

t_msg_new_broker *rcv_msj_new_broker(int socket_cliente, t_log *logger)
{
	int size;
	void *msg = recibir_buffer(socket_cliente, &size);
	t_msg_new_broker *msg_new_broker = deserializar_msg_new_broker(msg);
	free(msg);
	int tamano = tamano_recibido(size);
	log_info(logger, "(RECEIVING_FROM SOCKET:%d|NEW_POKEMON|%s|POS_X:%d|POS_Y:%d|QTY:%d|SIZE:%d Bytes)",
		socket_cliente, msg_new_broker->pokemon, msg_new_broker->coordenada->pos_x,
		msg_new_broker->coordenada->pos_y, msg_new_broker->cantidad, tamano);
	return msg_new_broker;
}

t_msg_catch_broker *rcv_msj_catch_broker(int socket_cliente, t_log *logger)
{
	int size;
	void *msg = recibir_buffer(socket_cliente, &size);
	t_msg_catch_broker *msg_catch_broker = deserializar_msg_catch_broker(msg);
	free(msg);
	int tamano = tamano_recibido(size);
	log_info(logger, "(RECEIVING_FROM SOCKET:%d|CATCH_POKEMON|%s|POS_X:%d|POS_Y:%d|SIZE:%d Bytes)",
		socket_cliente,	msg_catch_broker->pokemon, msg_catch_broker->coordenada->pos_x,
		msg_catch_broker->coordenada->pos_y, tamano);
	return msg_catch_broker;
}

t_msg_get_broker *rcv_msj_get_broker(int socket_cliente, t_log *logger)
{
	int size;
	void *msg = recibir_buffer(socket_cliente, &size);
	t_msg_get_broker *msg_get_broker = deserializar_msg_get_broker(msg);
	free(msg);
	int tamano = tamano_recibido(size);
	log_info(logger, "(RECEIVING_FROM SOCKET:%d|GET_POKEMON|%s|SIZE:%d Bytes)",
			socket_cliente, msg_get_broker->pokemon, tamano);
	return msg_get_broker;
}

t_msg_caught_broker *rcv_msj_caught_broker(int socket_cliente, t_log *logger)
{
	int size;
	void *msg = recibir_buffer(socket_cliente, &size);
	t_msg_caught_broker *msg_caught_broker = deserializar_msg_caught_broker(msg);
	free(msg);
	int tamano = tamano_recibido(size);
	log_info(logger, "(RECEIVING_FROM SOCKET:%d|CAUGHT_POKEMON|ID_CORRELATIVE:%d|RESULT:%s|SIZE:%d Bytes)",
			socket_cliente, msg_caught_broker->id_correlativo,	result_caught(msg_caught_broker->resultado), tamano);
	return msg_caught_broker;
}

t_msg_appeared_broker *rcv_msj_appeared_broker(int socket_cliente, t_log *logger)
{
	int size;
	void *msg = recibir_buffer(socket_cliente, &size);
	t_msg_appeared_broker *msg_appeared_broker = deserializar_msg_appeared_broker(msg);
	free(msg);
	int tamano = tamano_recibido(size);
	log_info(logger, "(RECEIVING_FROM SOCKET:%d|APPEARED_POKEMON|%s|ID_CORRELATIVE:%d|POS_X:%d|POS_Y:%d|SIZE:%d Bytes)",
		socket_cliente, msg_appeared_broker->pokemon, msg_appeared_broker->id_correlativo,
		msg_appeared_broker->coordenada->pos_x, msg_appeared_broker->coordenada->pos_y, tamano);
	return msg_appeared_broker;
}

t_msg_localized_broker *rcv_msj_localized_broker(int socket_cliente, t_log *logger)
{
	int size;
	void *msg = recibir_buffer(socket_cliente, &size);
	t_msg_localized_broker *msg_localized_broker = deserializar_msg_localized_broker(msg);
	free(msg);
	int tamano = tamano_recibido(size);
	char *concatenada = concat_posiciones(msg_localized_broker->posiciones->coordenadas);
	log_info(logger,"(RECEIVING_FROM SOCKET:%d|LOCALIZED_POKEMON|%s|ID_CORRELATIVE:%d|POS_QTY=%d|{Xn|Yn}:%s|SIZE:%d Bytes)",
		socket_cliente, msg_localized_broker->pokemon, msg_localized_broker->id_correlativo,
		msg_localized_broker->posiciones->cant_posic, concatenada,tamano);
	free(concatenada);
	return msg_localized_broker;
}

t_handsake_suscript *rcv_msj_handshake_suscriptor(int socket_cliente)
{
	int size;
	void *msg = recibir_buffer(socket_cliente, &size);
	t_handsake_suscript *handshake = deserializar_handshake_suscriptor(msg);
	free(msg);
	return handshake;
}

t_msg_new_gamecard *rcv_msj_new_gamecard(int socket_cliente, t_log *logger)
{
	int size;
	void *msg = recibir_buffer(socket_cliente, &size);
	t_msg_new_gamecard *msg_new_gamecard = deserializar_msg_new_gamecard(msg);
	free(msg);
	int tamano = tamano_recibido(size);
	log_info(logger, "(RECEIVING: FROM NEW_POKEMON |  ID_MSG:%d | %s | POS_X:%d | POS_Y:%d | QTY:%d|SIZE:%d Bytes)",
			msg_new_gamecard->id_mensaje, msg_new_gamecard->pokemon, msg_new_gamecard->coord->pos_x,
			msg_new_gamecard->coord->pos_y, msg_new_gamecard->cantidad, tamano);
	return msg_new_gamecard;
}

t_msg_catch_gamecard *rcv_msj_catch_gamecard(int socket_cliente, t_log *logger)
{
	int size;
	void *msg = recibir_buffer(socket_cliente, &size);
	t_msg_catch_gamecard *msg_catch_gamecard = deserializar_msg_catch_gamecard(msg);
	free(msg);
	int tamano = tamano_recibido(size);
	log_info(logger, "(RECEIVING: FROM CATCH_POKEMON | ID_MSG:%d | %s | POS_X:%d | POS_Y:%d -- SIZE = %d Bytes)",
			msg_catch_gamecard->id_mensaje, msg_catch_gamecard->pokemon, msg_catch_gamecard->coord->pos_x,
			msg_catch_gamecard->coord->pos_y, tamano);
	return msg_catch_gamecard;
}

t_msg_get_gamecard *rcv_msj_get_gamecard(int socket_cliente, t_log *logger)
{
	int size;
	void *msg = recibir_buffer(socket_cliente, &size);
	t_msg_get_gamecard *msg_get_gamecard = deserializar_msg_get_gamecard(msg);
	free(msg);
	int tamano = tamano_recibido(size);
	log_info(logger, "(RECEIVING: FROM GET_POKEMON | ID_MSG:%d | %s -- SIZE = %d Bytes)", msg_get_gamecard->id_mensaje, msg_get_gamecard->pokemon, tamano);
	return msg_get_gamecard;
}

t_msg_caught_team *rcv_msj_caught_team(int socket_cliente, t_log *logger)
{
	int size;
	void *msg = recibir_buffer(socket_cliente, &size);
	t_msg_caught_team *msg_caught_team = deserializar_msg_caught_team(msg);
	free(msg);
	int tamano = tamano_recibido(size);
	log_info(logger, "(RECEIVING: FROM: CAUGHT_POKEMON| ID_MSG:%d| ID_RELATIVE:%d | RESULT:%s -- SIZE: %d Bytes)",
			msg_caught_team->id_mensaje, msg_caught_team->id_correlativo, result_caught(msg_caught_team->resultado), tamano);
	return msg_caught_team;
}

t_msg_appeared_team *rcv_msj_appeared_team(int socket_cliente, t_log *logger)
{
	int size;
	void *msg = recibir_buffer(socket_cliente, &size);
	t_msg_appeared_team *msg_appeared_team = deserializar_msg_appeared_team(msg);
	free(msg);
	int tamano = tamano_recibido(size);
	log_info(logger, "(RECEIVING: APPEARED_POKEMON|ID_MSG:%d|ID_CORRELATIVE:%d|%s|POS_X:%d|POS_Y:%d|SIZE:%d Bytes)",
		msg_appeared_team->id_mensaje, msg_appeared_team->id_correlativo, msg_appeared_team->pokemon,
		msg_appeared_team->coord->pos_x,msg_appeared_team->coord->pos_y, tamano);
	return msg_appeared_team;
}

t_msg_localized_team *rcv_msj_localized_team(int socket_cliente, t_log *logger)
{
	int size;
	void *msg = recibir_buffer(socket_cliente, &size);
	t_msg_localized_team *msg_localized_team = deserializar_msg_localized_team(msg);
	free(msg);
	int tamano = tamano_recibido(size);
	char *concatenada = concat_posiciones(msg_localized_team->posiciones->coordenadas);
	log_info(logger,"(RECEIVING: FROM LOCALIZED_POKEMON|ID_MSG:%d|ID_CORRELATIVE:%d|%s|POSIC_QTY=%d|{Xn|Yn}:%s|SIZE:%d Bytes)",
		msg_localized_team->id_mensaje,msg_localized_team->id_correlativo,
		msg_localized_team->pokemon, msg_localized_team->posiciones->cant_posic, concatenada, tamano);
	free(concatenada);
	return msg_localized_team;
}

int rcv_codigo_operacion(int socket_cliente)
{
	int code;
	recv(socket_cliente, &code, sizeof(op_code), MSG_WAITALL);
	return code;
}

int rcv_msg_confirmed(int socket_cliente, t_log *logger)
{
	int size;
	void *a_recibir = recibir_buffer(socket_cliente, &size);
	int respuesta;
	memcpy(&respuesta, a_recibir, sizeof(int));
	if (respuesta == RESPUESTA_OK) {
		log_info(logger, "(RECEIVING: MSG_CONFIRMED)");
	}
	free(a_recibir);
	return respuesta;
}

int rcv_msg_suscrip_end(int socket_cliente)
{
	int size;
	void *a_recibir = recibir_buffer(socket_cliente, &size);
	int id_suscriptor;
	memcpy(&id_suscriptor, a_recibir, sizeof(int));
	free(a_recibir);
	return id_suscriptor;
}

int rcv_msj_cola_vacia(int socket_cliente, t_log *logger)
{
	int size;
	void *a_recibir = recibir_buffer(socket_cliente, &size);
	int id_suscriptor;
	memcpy(&id_suscriptor, a_recibir, size);
	free(a_recibir);
	log_warning(logger,"(EMPTY_QUEUE--No More Messages...|ID_SUSCRIPTOR: %d )", id_suscriptor);
	return id_suscriptor;
}

int rcv_id_mensaje(int socket_cliente, t_log *logger)
{
	int size;
	void *a_recibir = recibir_buffer(socket_cliente, &size);
	int id_mensaje;
	memcpy(&id_mensaje, a_recibir, sizeof(int));
	free(a_recibir);
	log_info(logger, "(RECEIVING: ID_MSG:%d)", id_mensaje);
	return id_mensaje;
}

void eliminar_msg_new_broker(t_msg_new_broker *msg_new_broker)
{
	free(msg_new_broker->pokemon);
	free(msg_new_broker->coordenada);
	free(msg_new_broker);
}

void eliminar_msg_catch_broker(t_msg_catch_broker *msg_catch_broker)
{
	free(msg_catch_broker->pokemon);
	free(msg_catch_broker->coordenada);
	free(msg_catch_broker);
}

void eliminar_msg_get_broker(t_msg_get_broker *msg_get_broker)
{
	free(msg_get_broker->pokemon);
	free(msg_get_broker);
}

void eliminar_msg_appeared_broker(t_msg_appeared_broker *msg_appeared_broker)
{
	free(msg_appeared_broker->coordenada);
	free(msg_appeared_broker->pokemon);
	free(msg_appeared_broker);
}

void eliminar_msg_appeared_team(t_msg_appeared_team *msg_appeared_team)
{
	free(msg_appeared_team->coord);
	free(msg_appeared_team->pokemon);
	free(msg_appeared_team);
}

void eliminar_msg_new_gamecard(t_msg_new_gamecard *msg_new_gamecard)
{
	free(msg_new_gamecard->coord);
	free(msg_new_gamecard->pokemon);
	free(msg_new_gamecard);
}

void eliminar_msg_catch_gamecard(t_msg_catch_gamecard *msg_catch_gamecard)
{
	free(msg_catch_gamecard->coord);
	free(msg_catch_gamecard->pokemon);
	free(msg_catch_gamecard);
}

void eliminar_msg_get_gamecard(t_msg_get_gamecard *msg_get_gamecard)
{
	free(msg_get_gamecard->pokemon);
	free(msg_get_gamecard);
}

void eliminar_msg_data(t_stream *msg_buffer)
{
	free(msg_buffer->data);
	free(msg_buffer);
}

int tamano_recibido(int bytes)
{
	return bytes + sizeof(uint32_t) + sizeof(op_code);
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

void *recibir_buffer(int socket_cliente, int *size)
{
	void *msg;
	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	msg = malloc(*size);
	recv(socket_cliente, msg, *size, MSG_WAITALL);
	return msg;
}

void eliminar_lista(t_list *lista)
{
	int cant_elem = lista->elements_count;
	for (int j = 0; j < cant_elem; j++) {
		free(list_get(lista,j));
	}
	list_destroy(lista);
}

bool verdadero_falso(char *valor)
{
	bool resultado;
	if(strcmp(valor,"TRUE") == 0) {
		resultado = true;
	}
	else if (strcmp(valor,"FALSE") == 0) {
		resultado = false;
	}
	return resultado;
}

char *nombre_cola(t_tipo_mensaje cola)
{
	char *nombre_cola;
	switch (cola) {
		case NEW_POKEMON:
			nombre_cola = "NEW_POKEMON";
			break;
		case APPEARED_POKEMON:
			nombre_cola = "APPEARED_POKEMON";
			break;
		case CATCH_POKEMON:
			nombre_cola = "CATCH_POKEMON";
			break;
		case CAUGHT_POKEMON:
			nombre_cola = "CAUGHT_POKEMON";
			break;
		case GET_POKEMON:
			nombre_cola = "GET_POKEMON";
			break;
		case LOCALIZED_POKEMON:
			nombre_cola = "LOCALIZED_POKEMON";
			break;
		default:
			nombre_cola = "";
			break;
	}
	return nombre_cola;
}
