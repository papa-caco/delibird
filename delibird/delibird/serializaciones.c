/*
 * serializaciones.c
 *
 *  Created on: 3 may. 2020
 *      Author: Los Que Aprueban
 *
 */
#include "serializaciones.h"

t_paquete *empaquetar_msj_cola_vacia(uint32_t id_suscriptor)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = COLA_VACIA;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = sizeof(uint32_t);
	paquete->buffer->data = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->data, &(id_suscriptor), paquete->buffer->size);
	return paquete;
}

t_paquete *empaquetar_msj_suscript_end(uint32_t id_suscriptor)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = SUSCRIP_END;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = sizeof(uint32_t);
	paquete->buffer->data = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->data, &(id_suscriptor), paquete->buffer->size);
	return paquete;
}

t_paquete *empaquetar_msg_new_gamecard(t_msg_new_gamecard *msg_new_gamecard)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = NEW_GAMECARD;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = 5 * sizeof(uint32_t) + msg_new_gamecard->size_pokemon;
	paquete->buffer->data = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->data + offset, &(msg_new_gamecard->id_mensaje),sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, &(msg_new_gamecard->coord->pos_x), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, &(msg_new_gamecard->coord->pos_y),sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, &(msg_new_gamecard->cantidad), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, &(msg_new_gamecard->size_pokemon), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, msg_new_gamecard->pokemon, msg_new_gamecard->size_pokemon);
	return paquete;
}


t_paquete *empaquetar_msg_get_gamecard(t_msg_get_gamecard *msg_get_gamecard)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = GET_GAMECARD;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = 2 * sizeof(uint32_t) + msg_get_gamecard->size_pokemon;
	paquete->buffer->data = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->data + offset, &(msg_get_gamecard->id_mensaje), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, &(msg_get_gamecard->size_pokemon), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, msg_get_gamecard->pokemon, msg_get_gamecard->size_pokemon);
	return paquete;
}

t_paquete *empaquetar_msg_get_broker(t_msg_get_broker *msg_get_broker)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = GET_BROKER;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = msg_get_broker->size_pokemon + sizeof(uint32_t);
	paquete->buffer->data = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->data + offset, &(msg_get_broker->size_pokemon), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, msg_get_broker->pokemon, msg_get_broker->size_pokemon);
	return paquete;
}

t_paquete *empaquetar_msg_new_broker(t_msg_new_broker *msg_new_broker)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = NEW_BROKER;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = 4 * sizeof(uint32_t) + msg_new_broker->size_pokemon;
	paquete->buffer->data = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->data + offset, &(msg_new_broker->coordenada->pos_x), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, &(msg_new_broker->coordenada->pos_y), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, &(msg_new_broker->cantidad), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, &(msg_new_broker->size_pokemon), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, msg_new_broker->pokemon, msg_new_broker->size_pokemon);
	return paquete;
}

t_paquete *empaquetar_msg_catch_gamecard(t_msg_catch_gamecard *msg_catch_gamecard)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = CATCH_GAMECARD;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = 4 * sizeof(uint32_t) + msg_catch_gamecard->size_pokemon;
	paquete->buffer->data = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->data + offset, &(msg_catch_gamecard->id_mensaje),sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, &(msg_catch_gamecard->coord->pos_x),sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, &(msg_catch_gamecard->coord->pos_y), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, &(msg_catch_gamecard->size_pokemon), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, msg_catch_gamecard->pokemon, msg_catch_gamecard->size_pokemon);
	return paquete;
}

t_paquete *empaquetar_msg_appeared_team(t_msg_appeared_team *msg_appeared_team)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = APPEARED_TEAM;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = 5 * sizeof(uint32_t) + msg_appeared_team->size_pokemon;
	paquete->buffer->data = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->data + offset, &(msg_appeared_team->id_mensaje), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, &(msg_appeared_team->id_correlativo), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, &(msg_appeared_team->coord->pos_x), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, &(msg_appeared_team->coord->pos_y), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, &(msg_appeared_team->size_pokemon), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, msg_appeared_team->pokemon, msg_appeared_team->size_pokemon);
	return paquete;
}

t_paquete *empaquetar_msg_caught_team(t_msg_caught_team *msg_caught_team)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = CAUGHT_TEAM;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = 2 * sizeof(uint32_t) + sizeof(t_result_caught);
	paquete->buffer->data = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->data + offset, &(msg_caught_team->id_mensaje),sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, &(msg_caught_team->id_correlativo),sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, &(msg_caught_team->resultado),sizeof(t_result_caught));
	return paquete;
}

t_paquete *empaquetar_msg_localized_team(t_msg_localized_team *msg_localized_team)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = LOCALIZED_TEAM;
	paquete->buffer = malloc(sizeof(t_stream));
	int cant_posiciones = msg_localized_team->posiciones->cant_posic;
	paquete->buffer->size = ((cant_posiciones * 2) + 4) * sizeof(uint32_t) + msg_localized_team->size_pokemon;
	paquete->buffer->data = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->data + offset, &(msg_localized_team->id_mensaje),sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, &(msg_localized_team->id_correlativo),sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, &(msg_localized_team->posiciones->cant_posic),sizeof(uint32_t));
	//Serializamos Lista de posiciones.
	for (int i = 0 ;i < cant_posiciones; i ++) {
		offset += sizeof(uint32_t);
		t_coordenada *coordxy = list_get(msg_localized_team->posiciones->coordenadas, i);
		memcpy(paquete->buffer->data + offset,&(coordxy->pos_x) ,sizeof(uint32_t));
		offset += sizeof(uint32_t);
		memcpy(paquete->buffer->data + offset,&(coordxy->pos_y) ,sizeof(uint32_t));
	}
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, &(msg_localized_team->size_pokemon),sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, msg_localized_team->pokemon, msg_localized_team->size_pokemon);
	return paquete;
}

t_paquete *empaquetar_msg_appeared_broker(t_msg_appeared_broker *msg_appeared_broker)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = APPEARED_BROKER;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = 4 * sizeof(uint32_t) + msg_appeared_broker->size_pokemon;
	paquete->buffer->data = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->data + offset, &(msg_appeared_broker->id_correlativo),sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, &(msg_appeared_broker->coordenada->pos_x),sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, &(msg_appeared_broker->coordenada->pos_y), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, &(msg_appeared_broker->size_pokemon), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, msg_appeared_broker->pokemon, msg_appeared_broker->size_pokemon);
	return paquete;
}

t_paquete *empaquetar_msg_confirmed(void)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	int respuesta = RESPUESTA_OK;
	paquete->codigo_operacion = MSG_CONFIRMED;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = sizeof(int);
	paquete->buffer->data = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->data, &respuesta, paquete->buffer->size);
	return paquete;
}

t_paquete *empaquetar_msg_fail(char *mensajeError)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = MSG_ERROR;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = strlen(mensajeError) + 1;
	paquete->buffer->data = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->data, mensajeError, paquete->buffer->size);
	return paquete;
}

t_paquete *empaquetar_msg_caught_broker(t_msg_caught_broker *msg_caught_broker)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = CAUGHT_BROKER;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = sizeof(uint32_t) + sizeof(t_result_caught);
	paquete->buffer->data = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->data + offset, &(msg_caught_broker->id_correlativo),sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, &(msg_caught_broker->resultado),sizeof(t_result_caught));
	return paquete;
}

t_paquete *empaquetar_msg_localized_broker(t_msg_localized_broker *msg_localized)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = LOCALIZED_BROKER;
	paquete->buffer = malloc(sizeof(t_stream));
	int cant_posiciones = msg_localized->posiciones->cant_posic;
	int totalBytes = (3 + (cant_posiciones * 2)) * sizeof(uint32_t) + msg_localized->size_pokemon;
	paquete->buffer->size = totalBytes;
	paquete->buffer->data = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->data + offset, &(msg_localized->id_correlativo), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, &(msg_localized->posiciones->cant_posic), sizeof(uint32_t));
	//Serializamos Lista de posiciones.
	for (int i = 0 ;i < cant_posiciones; i ++) {
		offset += sizeof(uint32_t);
		t_coordenada *coordxy = list_get(msg_localized->posiciones->coordenadas, i);
		memcpy(paquete->buffer->data + offset,&(coordxy->pos_x) ,sizeof(uint32_t));
		offset += sizeof(uint32_t);
		memcpy(paquete->buffer->data + offset,&(coordxy->pos_y) ,sizeof(uint32_t));
	}
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, &(msg_localized->size_pokemon), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, msg_localized->pokemon, msg_localized->size_pokemon);
	return paquete;
}

t_paquete *empaquetar_msg_catch_broker(t_msg_catch_broker *msg_catch_broker)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = CATCH_BROKER;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = 3 * sizeof(uint32_t) + msg_catch_broker->size_pokemon;
	paquete->buffer->data = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->data + offset, &(msg_catch_broker->coordenada->pos_x), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, &(msg_catch_broker->coordenada->pos_y), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, &(msg_catch_broker->size_pokemon), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, msg_catch_broker->pokemon, msg_catch_broker->size_pokemon);
	return paquete;
}

t_paquete *empaquetar_msg_handshake_suscript(t_handsake_suscript *handshake)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = SUSCRIBER_ACK;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size =  3 * sizeof(uint32_t) + sizeof(t_tipo_mensaje);
	paquete->buffer->data = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->data + offset, &(handshake->id_suscriptor),sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, &(handshake->id_recibido),sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, &(handshake->cola_id),sizeof(t_tipo_mensaje));
	offset += sizeof(t_tipo_mensaje);
	memcpy(paquete->buffer->data + offset, &(handshake->msjs_recibidos),sizeof(uint32_t));
	return paquete;
}

t_paquete *empaquetar_fin_suscripcion(t_handsake_suscript *handshake)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->codigo_operacion = FIN_SUSCRIPCION;
	paquete->buffer->size = 3 * sizeof(uint32_t) + sizeof(t_tipo_mensaje);
	paquete->buffer->data = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->data + offset, &(handshake->id_suscriptor),sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, &(handshake->id_recibido),sizeof(uint32_t));
	offset += sizeof(t_tipo_mensaje);
	memcpy(paquete->buffer->data + offset, &(handshake->cola_id),sizeof(t_tipo_mensaje));
	offset += sizeof(uint32_t);
	memcpy(paquete->buffer->data + offset, &(handshake->msjs_recibidos),sizeof(uint32_t));
	return paquete;
}

t_paquete *empaquetar_id_mensaje(int id_mensaje)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = ID_MENSAJE;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = sizeof(uint32_t);
	paquete->buffer->data = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->data, &(id_mensaje), paquete->buffer->size);
	return paquete;
}

int coordenada_posiciones(t_posiciones_localized* posiciones, int indice)
{
		t_list *coordxy =  posiciones->coordenadas;
		void *elem = list_get(coordxy,indice);
		int valor;
		memcpy(&valor, elem, sizeof(int));
		return valor;
}

void *serializar_paquete(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;
	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(op_code));
	desplazamiento += sizeof(op_code);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(magic + desplazamiento, paquete->buffer->data,paquete->buffer->size);
	return magic;
}

void eliminar_paquete(t_paquete *paquete) {
	free(paquete->buffer->data);
	free(paquete->buffer);
	free(paquete);
}


t_msg_new_broker *deserializar_msg_new_broker(void *msg)
{
	t_msg_new_broker *msg_new_broker = malloc(sizeof(t_msg_new_broker));
	msg_new_broker->coordenada = malloc(sizeof(t_coordenada));
	int offset = 0;
	memcpy(&(msg_new_broker->coordenada->pos_x),msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(msg_new_broker->coordenada->pos_y), msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(msg_new_broker->cantidad), msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(msg_new_broker->size_pokemon), msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	msg_new_broker->pokemon = malloc(msg_new_broker->size_pokemon);
	memcpy(msg_new_broker->pokemon, msg + offset, msg_new_broker->size_pokemon);
	return msg_new_broker;
}

t_msg_catch_broker *deserializar_msg_catch_broker(void *msg)
{
	t_msg_catch_broker *msg_catch_broker = malloc(sizeof(t_msg_catch_broker));
	msg_catch_broker->coordenada = malloc(sizeof(t_coordenada));
	int offset = 0;
	memcpy(&(msg_catch_broker->coordenada->pos_x),msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(msg_catch_broker->coordenada->pos_y), msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(msg_catch_broker->size_pokemon), msg + offset, sizeof(uint32_t));
	msg_catch_broker->pokemon = malloc(msg_catch_broker->size_pokemon);
	offset += sizeof(uint32_t);
	memcpy(msg_catch_broker->pokemon, msg + offset, msg_catch_broker->size_pokemon);
	return msg_catch_broker;
}

t_msg_get_broker *deserializar_msg_get_broker(void *msg)
{
	t_msg_get_broker *msg_get_broker = malloc(sizeof(t_msg_get_broker));
	int offset = 0;
	memcpy(&(msg_get_broker->size_pokemon), msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	msg_get_broker->pokemon = malloc(msg_get_broker->size_pokemon);
	memcpy(msg_get_broker->pokemon, msg + offset, msg_get_broker->size_pokemon);
	return msg_get_broker;
}

t_msg_caught_broker *deserializar_msg_caught_broker(void *msg)
{
	t_msg_caught_broker *msg_caught_broker = malloc(sizeof(t_msg_caught_broker));
	int offset = 0;
	memcpy(&(msg_caught_broker->id_correlativo),msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(msg_caught_broker->resultado), msg + offset, sizeof(t_result_caught));
	return msg_caught_broker;
}

t_msg_appeared_broker *deserializar_msg_appeared_broker(void *msg)
{
	t_msg_appeared_broker *msg_appeared_broker = malloc(sizeof(t_msg_appeared_broker));
	msg_appeared_broker->coordenada = malloc(sizeof(t_coordenada));
	int offset = 0;
	memcpy(&(msg_appeared_broker->id_correlativo), msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(msg_appeared_broker->coordenada->pos_x),msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(msg_appeared_broker->coordenada->pos_y), msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(msg_appeared_broker->size_pokemon), msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	msg_appeared_broker->pokemon = malloc(msg_appeared_broker->size_pokemon);
	memcpy(msg_appeared_broker->pokemon, msg + offset, msg_appeared_broker->size_pokemon);
	return msg_appeared_broker;
}

t_msg_localized_broker *deserializar_msg_localized_broker(void *msg)
{
	t_msg_localized_broker *msg_localized_broker = malloc(sizeof(t_msg_localized_broker));
	msg_localized_broker->posiciones = malloc(sizeof(t_posiciones_localized));
	msg_localized_broker->posiciones->coordenadas = list_create();
	int offset = 0;
	memcpy(&(msg_localized_broker->id_correlativo), msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(msg_localized_broker->posiciones->cant_posic), msg + offset, sizeof(uint32_t));
	int cant_posiciones = msg_localized_broker->posiciones->cant_posic;
	offset += sizeof(uint32_t);
	for (int i = 0; i < cant_posiciones; i ++) {
		t_coordenada *coord = malloc(sizeof(t_coordenada));
		memcpy(&coord->pos_x,msg + offset, sizeof(uint32_t));
		offset += sizeof(uint32_t);
		memcpy(&coord->pos_y,msg + offset, sizeof(uint32_t));
		list_add(msg_localized_broker->posiciones->coordenadas, coord);
		offset += sizeof(uint32_t);
	}
	memcpy(&(msg_localized_broker->size_pokemon), msg + offset, sizeof(uint32_t));
	msg_localized_broker->pokemon = malloc(msg_localized_broker->size_pokemon);
	offset += sizeof(uint32_t);
	memcpy(msg_localized_broker->pokemon, msg + offset, msg_localized_broker->size_pokemon);
	return msg_localized_broker;
}

t_handsake_suscript *deserializar_handshake_suscriptor(void *msg)
{
	t_handsake_suscript *handshake = malloc(sizeof(t_handsake_suscript));
	int offset = 0;
	memcpy(&(handshake->id_suscriptor), msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(handshake->id_recibido), msg + offset, sizeof(uint32_t));
	offset += sizeof(t_tipo_mensaje);
	memcpy(&(handshake->cola_id), msg + offset, sizeof(t_tipo_mensaje));
	offset += sizeof(uint32_t);
	memcpy(&(handshake->msjs_recibidos), msg + offset, sizeof(uint32_t));
	return handshake;
}

t_msg_new_gamecard *deserializar_msg_new_gamecard(void *msg)
{
	t_msg_new_gamecard *msg_new_gamecard = malloc(sizeof(t_msg_new_gamecard));
	msg_new_gamecard->coord = malloc(sizeof(t_coordenada));
	int offset = 0;
	memcpy(&(msg_new_gamecard->id_mensaje),msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(msg_new_gamecard->coord->pos_x),msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(msg_new_gamecard->coord->pos_y), msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(msg_new_gamecard->cantidad), msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(msg_new_gamecard->size_pokemon), msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	msg_new_gamecard->pokemon = malloc(msg_new_gamecard->size_pokemon);
	memcpy(msg_new_gamecard->pokemon, msg + offset, msg_new_gamecard->size_pokemon);
	return msg_new_gamecard;
}

t_msg_catch_gamecard *deserializar_msg_catch_gamecard(void *msg)
{
	t_msg_catch_gamecard *msg_catch_gamecard = malloc(sizeof(t_msg_catch_gamecard));
	msg_catch_gamecard->coord = malloc(sizeof(t_coordenada));
	int offset = 0;
	memcpy(&(msg_catch_gamecard->id_mensaje),msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(msg_catch_gamecard->coord->pos_x),msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(msg_catch_gamecard->coord->pos_y), msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(msg_catch_gamecard->size_pokemon), msg + offset, sizeof(uint32_t));
	msg_catch_gamecard->pokemon = malloc(msg_catch_gamecard->size_pokemon);
	offset += sizeof(uint32_t);
	memcpy(msg_catch_gamecard->pokemon, msg + offset, msg_catch_gamecard->size_pokemon);
	return msg_catch_gamecard;
}

t_msg_get_gamecard *deserializar_msg_get_gamecard(void *msg)
{
	t_msg_get_gamecard *msg_get_gamecard = malloc(sizeof(t_msg_get_gamecard));
	int offset = 0;
	memcpy(&(msg_get_gamecard->id_mensaje),msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(msg_get_gamecard->size_pokemon),msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	msg_get_gamecard->pokemon = malloc(msg_get_gamecard->size_pokemon);
	memcpy(msg_get_gamecard->pokemon, msg + offset, msg_get_gamecard->size_pokemon);
	return msg_get_gamecard;
}

t_msg_caught_team *deserializar_msg_caught_team(void *msg)
{
	t_msg_caught_team *msg_caught_team = malloc(sizeof(t_msg_caught_team));
	int offset = 0;
	memcpy(&(msg_caught_team->id_mensaje),msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(msg_caught_team->id_correlativo),msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(msg_caught_team->resultado), msg + offset, sizeof(t_result_caught));
	return msg_caught_team;
}

t_msg_appeared_team *deserializar_msg_appeared_team(void *msg)
{
	t_msg_appeared_team *msg_appeared_team = malloc(sizeof(t_msg_appeared_team));
	msg_appeared_team->coord = malloc(sizeof(t_coordenada));
	int offset = 0;
	memcpy(&(msg_appeared_team->id_mensaje),msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(msg_appeared_team->id_correlativo),msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(msg_appeared_team->coord->pos_x),msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(msg_appeared_team->coord->pos_y), msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(msg_appeared_team->size_pokemon), msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	msg_appeared_team->pokemon = malloc(msg_appeared_team->size_pokemon);
	memcpy(msg_appeared_team->pokemon, msg + offset, msg_appeared_team->size_pokemon);
	return msg_appeared_team;
}

t_msg_localized_team *deserializar_msg_localized_team(void *msg)
{
	t_msg_localized_team *msg_localized_team = malloc(sizeof(t_msg_localized_team));
	msg_localized_team->posiciones = malloc(sizeof(t_posiciones_localized));
	msg_localized_team->posiciones->coordenadas = list_create();
	int offset = 0;
	memcpy(&(msg_localized_team->id_mensaje), msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(msg_localized_team->id_correlativo), msg + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(msg_localized_team->posiciones->cant_posic), msg + offset, sizeof(uint32_t));
	int cant_elem = msg_localized_team->posiciones->cant_posic;
	offset += sizeof(uint32_t);
	for (int i = 0; i < cant_elem; i ++) {
		t_coordenada *coord = malloc(sizeof(t_coordenada));
		memcpy(&coord->pos_x,msg + offset, sizeof(uint32_t));
		offset += sizeof(uint32_t);
		memcpy(&coord->pos_y,msg + offset, sizeof(uint32_t));
		list_add(msg_localized_team->posiciones->coordenadas, coord);
		offset += sizeof(uint32_t);
	}
	memcpy(&(msg_localized_team->size_pokemon), msg + offset, sizeof(uint32_t));
	msg_localized_team->pokemon = malloc(msg_localized_team->size_pokemon);
	offset += sizeof(uint32_t);
	memcpy(msg_localized_team->pokemon, msg + offset, msg_localized_team->size_pokemon);
	return msg_localized_team;
}

int tamano_paquete(t_paquete *paquete) {
	return paquete->buffer->size + sizeof(paquete->codigo_operacion) + sizeof(uint32_t);
}
