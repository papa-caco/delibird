/*
 * serializaciones.c
 *
 *  Created on: 3 may. 2020
 *      Author: Los Que Aprueban
 *
 */
#include "serializaciones.h"

t_paquete *empaquetar_msj_cola_vacia(int id_suscriptor)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = COLA_VACIA;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = sizeof(int);
	paquete->buffer->data = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->data, &(id_suscriptor), paquete->buffer->size);
	return paquete;
}

t_paquete *empaquetar_msj_suscript_end(int id_suscriptor)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = SUSCRIP_END;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = sizeof(int);
	paquete->buffer->data = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->data, &(id_suscriptor), paquete->buffer->size);
	return paquete;
}

t_paquete *empaquetar_msg_new_gamecard(t_msg_new_gamecard *msg_new_gamecard)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = NEW_GAMECARD;
	paquete->buffer = malloc(sizeof(t_stream));
	int size_pokemon = strlen(msg_new_gamecard->pokemon) + 1;
	paquete->buffer->size = 4 * sizeof(int) + size_pokemon;
	paquete->buffer->data = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->data + offset, &(msg_new_gamecard->id_mensaje),sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, &(msg_new_gamecard->coord->pos_x), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, &(msg_new_gamecard->coord->pos_y),sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, &(msg_new_gamecard->cantidad), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, msg_new_gamecard->pokemon, size_pokemon);
	return paquete;
}


t_paquete *empaquetar_msg_get_gamecard(t_msg_get_gamecard *msg_get_gamecard)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = GET_GAMECARD;
	paquete->buffer = malloc(sizeof(t_stream));
	int size_pokemon = strlen(msg_get_gamecard->pokemon) + 1;
	paquete->buffer->size = sizeof(int) + size_pokemon;
	paquete->buffer->data = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->data + offset, &(msg_get_gamecard->id_mensaje), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, msg_get_gamecard->pokemon, size_pokemon);
	return paquete;
}

t_paquete *empaquetar_msg_get_broker(t_msg_get_broker *msg_get_broker)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = GET_BROKER;
	paquete->buffer = malloc(sizeof(t_stream));
	int size_pokemon = strlen(msg_get_broker->pokemon) + 1;
	paquete->buffer->size = size_pokemon;
	paquete->buffer->data = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->data, msg_get_broker->pokemon, size_pokemon);
	return paquete;
}

t_paquete *empaquetar_msg_new_broker(t_msg_new_broker *msg_new_broker)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = NEW_BROKER;
	paquete->buffer = malloc(sizeof(t_stream));
	int size_pokemon = strlen(msg_new_broker->pokemon) + 1;
	paquete->buffer->size = 3 * sizeof(int) + size_pokemon;
	paquete->buffer->data = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->data + offset, &(msg_new_broker->coordenada->pos_x), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, &(msg_new_broker->coordenada->pos_y), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, &(msg_new_broker->cantidad), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, msg_new_broker->pokemon, size_pokemon);
	return paquete;
}

t_paquete *empaquetar_msg_catch_gamecard(t_msg_catch_gamecard *msg_catch_gamecard)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = CATCH_GAMECARD;
	paquete->buffer = malloc(sizeof(t_stream));
	int size_pokemon = strlen(msg_catch_gamecard->pokemon) + 1;
	paquete->buffer->size = 3 * sizeof(int) + size_pokemon;
	paquete->buffer->data = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->data + offset, &(msg_catch_gamecard->id_mensaje),sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, &(msg_catch_gamecard->coord->pos_x),sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, &(msg_catch_gamecard->coord->pos_y), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, msg_catch_gamecard->pokemon, size_pokemon);
	return paquete;
}

t_paquete *empaquetar_msg_appeared_team(t_msg_appeared_team *msg_appeared_team)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = APPEARED_TEAM;
	paquete->buffer = malloc(sizeof(t_stream));
	int size_pokemon = strlen(msg_appeared_team->pokemon) + 1;
	paquete->buffer->size = 3 * sizeof(int) + size_pokemon;
	paquete->buffer->data = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->data + offset, &(msg_appeared_team->id_mensaje),sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, &(msg_appeared_team->coord->pos_x),sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, &(msg_appeared_team->coord->pos_y), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, msg_appeared_team->pokemon, size_pokemon);
	return paquete;
}

t_paquete *empaquetar_msg_caught_team(t_msg_caught_team *msg_caught_team)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = CAUGHT_TEAM;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = 2 * sizeof(int) + sizeof(t_result_caught);
	paquete->buffer->data = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->data + offset, &(msg_caught_team->id_mensaje),sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, &(msg_caught_team->id_correlativo),sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, &(msg_caught_team->resultado),sizeof(t_result_caught));
	return paquete;
}

t_paquete *empaquetar_msg_localized_team(t_msg_localized_team *msg_localized_team)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = LOCALIZED_TEAM;
	paquete->buffer = malloc(sizeof(t_stream));
	int size_pokemon = strlen(msg_localized_team->pokemon) + 1;
	int cant_posiciones = msg_localized_team->posiciones->cant_posic;
	paquete->buffer->size = ((cant_posiciones * 2) + 3) * sizeof(int) + size_pokemon;
	paquete->buffer->data = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->data + offset, &(msg_localized_team->id_mensaje),sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, &(msg_localized_team->id_correlativo),sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, &(msg_localized_team->posiciones->cant_posic),sizeof(int));
	//Serializamos Lista de posiciones.
	for (int i = 0 ;i < cant_posiciones; i ++) {
		offset += sizeof(int);
		t_coordenada *coordxy = list_get(msg_localized_team->posiciones->coordenadas, i);
		memcpy(paquete->buffer->data + offset,&(coordxy->pos_x) ,sizeof(int));
		offset += sizeof(int);
		memcpy(paquete->buffer->data + offset,&(coordxy->pos_y) ,sizeof(int));
	}
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, msg_localized_team->pokemon, size_pokemon);
	return paquete;
}

t_paquete *empaquetar_msg_appeared_broker(t_msg_appeared_broker *msg_appeared_broker)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = APPEARED_BROKER;
	paquete->buffer = malloc(sizeof(t_stream));
	int size_pokemon = strlen(msg_appeared_broker->pokemon) + 1;
	paquete->buffer->size = 3 * sizeof(int) + size_pokemon;
	paquete->buffer->data = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->data + offset, &(msg_appeared_broker->id_correlativo),sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, &(msg_appeared_broker->coordenada->pos_x),sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, &(msg_appeared_broker->coordenada->pos_y), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, msg_appeared_broker->pokemon, size_pokemon);
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
	paquete->buffer->size = sizeof(int) + sizeof(t_result_caught);
	paquete->buffer->data = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->data + offset, &(msg_caught_broker->id_correlativo),sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, &(msg_caught_broker->resultado),sizeof(t_result_caught));
	return paquete;
}

t_paquete *empaquetar_msg_localized_broker(t_msg_localized_broker *msg_localized)
{
	int size_pokemon = strlen(msg_localized->pokemon) + 1;
	int cant_posiciones = msg_localized->posiciones->cant_posic;
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = LOCALIZED_BROKER;
	paquete->buffer = malloc(sizeof(t_stream));
	int totalBytes = (2 + (cant_posiciones * 2)) * sizeof(int) + size_pokemon;
	paquete->buffer->size = totalBytes;
	paquete->buffer->data = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->data + offset, &(msg_localized->id_correlativo), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, &(msg_localized->posiciones->cant_posic), sizeof(int));
	//Serializamos Lista de posiciones.
	for (int i = 0 ;i < cant_posiciones; i ++) {
		offset += sizeof(int);
		t_coordenada *coordxy = list_get(msg_localized->posiciones->coordenadas, i);
		memcpy(paquete->buffer->data + offset,&(coordxy->pos_x) ,sizeof(int));
		offset += sizeof(int);
		memcpy(paquete->buffer->data + offset,&(coordxy->pos_y) ,sizeof(int));
	}
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, msg_localized->pokemon, size_pokemon);
	return paquete;
}

t_paquete *empaquetar_msg_catch_broker(t_msg_catch_broker *msg_catch_broker)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = CATCH_BROKER;
	paquete->buffer = malloc(sizeof(t_stream));
	int size_pokemon = strlen(msg_catch_broker->pokemon) + 1;
	paquete->buffer->size = 2 * sizeof(int) + size_pokemon;
	paquete->buffer->data = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->data + offset, &(msg_catch_broker->coordenada->pos_x), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, &(msg_catch_broker->coordenada->pos_y), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, msg_catch_broker->pokemon, size_pokemon);
	return paquete;
}

t_paquete *empaquetar_msg_handshake_suscript(t_handsake_suscript *handshake, op_code codigo_operacion)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = codigo_operacion;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size =  2 * sizeof(int);
	paquete->buffer->data = malloc(paquete->buffer->size);
	int offset = 0;
	memcpy(paquete->buffer->data + offset, &(handshake->id_suscriptor),sizeof(int));
	offset += sizeof(int);
	memcpy(paquete->buffer->data + offset, &(handshake->msjs_recibidos),sizeof(int));
	return paquete;
}

t_paquete *empaquetar_fin_suscripcion(int id_suscriptor)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->codigo_operacion = FIN_SUSCRIPCION;
	paquete->buffer->size = sizeof(int);
	paquete->buffer->data = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->data , &(id_suscriptor), sizeof(int));
	return paquete;
}

t_paquete *empaquetar_id_mensaje(int id_mensaje)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = ID_MENSAJE;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = sizeof(int);
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
	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->data,paquete->buffer->size);
	return magic;
}

void eliminar_paquete(t_paquete *paquete) {
	free(paquete->buffer->data);
	free(paquete->buffer);
	free(paquete);
}


t_msg_new_broker *deserializar_msg_new_broker(void *msg, int size)
{
	t_msg_new_broker *msg_new_broker = malloc(sizeof(t_msg_new_broker));
	msg_new_broker->coordenada = malloc(sizeof(t_coordenada));
	int size_pokemon = size - 3 * sizeof(int);
	msg_new_broker->pokemon = malloc(size_pokemon);
	int offset = 0;
	memcpy(&(msg_new_broker->coordenada->pos_x),msg + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&(msg_new_broker->coordenada->pos_y), msg + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&(msg_new_broker->cantidad), msg + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(msg_new_broker->pokemon, msg + offset, size_pokemon);
	return msg_new_broker;
}

t_msg_catch_broker *deserializar_msg_catch_broker(void *msg, int size)
{
	t_msg_catch_broker *msg_catch_broker = malloc(sizeof(t_msg_catch_broker));
	msg_catch_broker->coordenada = malloc(sizeof(t_coordenada));
	int size_pokemon = size - 2 * sizeof(int);
	msg_catch_broker->pokemon = malloc(size_pokemon);
	int offset = 0;
	memcpy(&(msg_catch_broker->coordenada->pos_x),msg + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&(msg_catch_broker->coordenada->pos_y), msg + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(msg_catch_broker->pokemon, msg + offset, size_pokemon);
	return msg_catch_broker;
}

t_msg_get_broker *deserializar_msg_get_broker(void *msg, int size)
{
	t_msg_get_broker *msg_get_broker = malloc(sizeof(t_msg_get_broker));
	msg_get_broker->pokemon = malloc(size);
	memcpy(msg_get_broker->pokemon, msg, size);
	return msg_get_broker;
}

t_msg_caught_broker *deserializar_msg_caught_broker(void *msg, int size)
{
	t_msg_caught_broker *msg_caught_broker = malloc(sizeof(t_msg_caught_broker));
	int offset = 0;
	memcpy(&(msg_caught_broker->id_correlativo),msg + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&(msg_caught_broker->resultado), msg + offset, sizeof(t_result_caught));
	return msg_caught_broker;
}

t_msg_appeared_broker *deserializar_msg_appeared_broker(void *msg, int size)
{
	t_msg_appeared_broker *msg_appeared_broker = malloc(sizeof(t_msg_appeared_broker));
	msg_appeared_broker->coordenada = malloc(sizeof(t_coordenada));
	int size_pokemon = size - 3 * sizeof(int);
	msg_appeared_broker->pokemon = malloc(size_pokemon);
	int offset = 0;
	memcpy(&(msg_appeared_broker->id_correlativo), msg + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&(msg_appeared_broker->coordenada->pos_x),msg + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&(msg_appeared_broker->coordenada->pos_y), msg + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(msg_appeared_broker->pokemon, msg + offset, size_pokemon);
	return msg_appeared_broker;
}

t_msg_localized_broker *deserializar_msg_localized_broker(void *msg, int size)
{
	t_msg_localized_broker *msg_localized_broker = malloc(sizeof(t_msg_localized_broker));
	msg_localized_broker->posiciones = malloc(sizeof(t_posiciones_localized));
	msg_localized_broker->posiciones->coordenadas = list_create();
	int offset = 0;
	memcpy(&(msg_localized_broker->id_correlativo), msg + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&(msg_localized_broker->posiciones->cant_posic), msg + offset, sizeof(int));
	int cant_posiciones = msg_localized_broker->posiciones->cant_posic;
	offset += sizeof(int);
	for (int i = 0; i < cant_posiciones; i ++) {
		t_coordenada *coord = malloc(sizeof(t_coordenada));
		memcpy(&coord->pos_x,msg + offset, sizeof(int));
		offset += sizeof(int);
		memcpy(&coord->pos_y,msg + offset, sizeof(int));
		list_add(msg_localized_broker->posiciones->coordenadas, coord);
		offset += sizeof(int);
	}
	int size_pokemon = size - ((cant_posiciones * 2) + 2) * sizeof(int);
	msg_localized_broker->pokemon = malloc(size_pokemon);
	memcpy(msg_localized_broker->pokemon, msg + offset, size_pokemon);
	return msg_localized_broker;
}

t_handsake_suscript *deserializar_handshake_suscriptor(void *msg, int size)
{
	t_handsake_suscript *handshake = malloc(sizeof(t_handsake_suscript));
	int offset = 0;
	memcpy(&(handshake->id_suscriptor), msg + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&(handshake->msjs_recibidos), msg + offset, sizeof(int));
	return handshake;
}

t_msg_new_gamecard *deserializar_msg_new_gamecard(void *msg, int size)
{
	t_msg_new_gamecard *msg_new_gamecard = malloc(sizeof(t_msg_new_gamecard));
	msg_new_gamecard->coord = malloc(sizeof(t_coordenada));
	int size_pokemon = size - 4 * sizeof(int);
	msg_new_gamecard->pokemon = malloc(size_pokemon);
	int offset = 0;
	memcpy(&(msg_new_gamecard->id_mensaje),msg + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&(msg_new_gamecard->coord->pos_x),msg + offset, sizeof(int));
		offset += sizeof(int);
	memcpy(&(msg_new_gamecard->coord->pos_y), msg + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&(msg_new_gamecard->cantidad), msg + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(msg_new_gamecard->pokemon, msg + offset, size_pokemon);
	return msg_new_gamecard;
}

t_msg_catch_gamecard *deserializar_msg_catch_gamecard(void *msg, int size)
{
	t_msg_catch_gamecard *msg_catch_gamecard = malloc(sizeof(t_msg_catch_gamecard));
	msg_catch_gamecard->coord = malloc(sizeof(t_coordenada));
	int size_pokemon = size - 3 * sizeof(int);
	msg_catch_gamecard->pokemon = malloc(size_pokemon);
	int offset = 0;
	memcpy(&(msg_catch_gamecard->id_mensaje),msg + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&(msg_catch_gamecard->coord->pos_x),msg + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&(msg_catch_gamecard->coord->pos_y), msg + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(msg_catch_gamecard->pokemon, msg + offset, size_pokemon);
	return msg_catch_gamecard;
}

t_msg_get_gamecard *deserializar_msg_get_gamecard(void *msg, int size)
{
	t_msg_get_gamecard *msg_get_gamecard = malloc(sizeof(t_msg_get_gamecard));
	int size_pokemon = size - sizeof(int);
	msg_get_gamecard->pokemon = malloc(size_pokemon);
	int offset = 0;
	memcpy(&(msg_get_gamecard->id_mensaje),msg + offset, sizeof(int));
		offset += sizeof(int);
	memcpy(msg_get_gamecard->pokemon, msg + offset, size_pokemon);
	return msg_get_gamecard;
}

t_msg_caught_team *deserializar_msg_caught_team(void *msg)
{
	t_msg_caught_team *msg_caught_team = malloc(sizeof(t_msg_caught_team));
	int offset = 0;
	memcpy(&(msg_caught_team->id_mensaje),msg + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&(msg_caught_team->id_correlativo),msg + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&(msg_caught_team->resultado), msg + offset, sizeof(t_result_caught));
	return msg_caught_team;
}

t_msg_appeared_team *deserializar_msg_appeared_team(void *msg, int size)
{
	t_msg_appeared_team *msg_appeared_team = malloc(sizeof(t_msg_appeared_team));
	msg_appeared_team->coord = malloc(sizeof(t_coordenada));
	int size_pokemon = size - 3 * sizeof(int);
	msg_appeared_team->pokemon = malloc(size_pokemon);
	int offset = 0;
	memcpy(&(msg_appeared_team->id_mensaje),msg + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&(msg_appeared_team->coord->pos_x),msg + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&(msg_appeared_team->coord->pos_y), msg + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(msg_appeared_team->pokemon, msg + offset, size_pokemon);
	return msg_appeared_team;
}

t_msg_localized_team *deserializar_msg_localized_team(void *msg, int size)
{
	t_msg_localized_team *msg_localized_team = malloc(sizeof(t_msg_localized_team));
	msg_localized_team->posiciones = malloc(sizeof(t_posiciones_localized));
	msg_localized_team->posiciones->coordenadas = list_create();
	int offset = 0;
	memcpy(&(msg_localized_team->id_mensaje), msg + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&(msg_localized_team->id_correlativo), msg + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&(msg_localized_team->posiciones->cant_posic), msg + offset, sizeof(int));
	int cant_elem = msg_localized_team->posiciones->cant_posic;
	offset += sizeof(int);
	for (int i = 0; i < cant_elem; i ++) {
		t_coordenada *coord = malloc(sizeof(t_coordenada));
		memcpy(&coord->pos_x,msg + offset, sizeof(int));
		offset += sizeof(int);
		memcpy(&coord->pos_y,msg + offset, sizeof(int));
		list_add(msg_localized_team->posiciones->coordenadas, coord);
		offset += sizeof(int);
	}
	int size_pokemon = size - ((cant_elem * 2) + 3) * sizeof(int);
	msg_localized_team->pokemon = malloc(size_pokemon);
	memcpy(msg_localized_team->pokemon, msg + offset, size_pokemon);
	return msg_localized_team;
}

int tamano_paquete(t_paquete *paquete) {
	return paquete->buffer->size + sizeof(paquete->codigo_operacion)
			+ sizeof(paquete->buffer->size);
}
