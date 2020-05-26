/*
 ============================================================================
 Name        : broker_serializ.c

 Author      : Los Que Aprueban.

 Created on	 : 16 may. 2020

 Version     :

 Description :
 ============================================================================
 */
#include "broker_serializ.h"

t_stream *serializar_msg_get(t_msg_get_broker *msg_get)
{
	t_stream *data_msg = malloc(sizeof(t_stream));
	int cant_letras = msg_get->size_pokemon - 1;
	data_msg->size = espacio_cache_msg_get(msg_get);
	data_msg->data = malloc(data_msg->size);
	int offset = 0;
	memcpy(data_msg->data + offset, &(cant_letras), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(data_msg->data + offset, msg_get->pokemon, cant_letras);
	return data_msg;
}

t_msg_get_gamecard *deserializar_msg_get(t_stream *data_msg)
{
	t_msg_get_gamecard *msg_get = malloc(sizeof(t_msg_get_gamecard));
	int offset = 0;
	int cant_letras;
	char *c = "\0";
	memcpy(&cant_letras, data_msg->data + offset, sizeof(uint32_t));
	msg_get->size_pokemon = cant_letras + 1;
	offset += sizeof(uint32_t);
	msg_get->pokemon = malloc(msg_get->size_pokemon);
	memcpy(msg_get->pokemon, data_msg->data + offset,cant_letras);
	memcpy(msg_get->pokemon + cant_letras, c,1);
	return msg_get;
}

t_stream *serializar_msg_new(t_msg_new_broker *msg_new)
{
	t_stream *data_msg = malloc(sizeof(t_stream));
	int cant_letras = msg_new->size_pokemon - 1;
	data_msg->size = espacio_cache_msg_new(msg_new);
	data_msg->data = malloc(data_msg->size);
	int offset = 0;
	memcpy(data_msg->data + offset, &(msg_new->coordenada->pos_x), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(data_msg->data + offset, &(msg_new->coordenada->pos_y), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(data_msg->data + offset, &(msg_new->cantidad), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(data_msg->data + offset, &(cant_letras), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(data_msg->data + offset, msg_new->pokemon, cant_letras);
	return data_msg;
}

t_msg_new_gamecard *deserializar_msg_new(t_stream *data_msg)
{
	t_msg_new_gamecard *msg_new = malloc(sizeof(t_msg_new_gamecard));
	msg_new->coord = malloc(sizeof(t_coordenada));
	int offset = 0;
	int cant_letras;
	char *c = "\0";
	memcpy(&(msg_new->coord->pos_x),data_msg->data + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(msg_new->coord->pos_y), data_msg->data + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(msg_new->cantidad), data_msg->data + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&cant_letras, data_msg->data + offset, sizeof(uint32_t));
	msg_new->size_pokemon = cant_letras + 1;
	offset += sizeof(uint32_t);
	msg_new->pokemon = malloc(msg_new->size_pokemon);
	memcpy(msg_new->pokemon, data_msg->data + offset,cant_letras);
	memcpy(msg_new->pokemon + cant_letras, c,1);
	return msg_new;
}

t_stream *serializar_msg_catch(t_msg_catch_broker *msg_catch)
{
	t_stream *data_msg = malloc(sizeof(t_stream));
	int cant_letras = msg_catch->size_pokemon - 1;
	data_msg->size = espacio_cache_msg_catch(msg_catch);
	data_msg->data = malloc(data_msg->size);
	int offset = 0;
	memcpy(data_msg->data + offset, &(msg_catch->coordenada->pos_x), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(data_msg->data + offset, &(msg_catch->coordenada->pos_y), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(data_msg->data + offset, &(cant_letras), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(data_msg->data + offset, msg_catch->pokemon, cant_letras);
	return data_msg;
}

t_msg_catch_gamecard *deserializar_msg_catch(t_stream *data_msg)
{
	t_msg_catch_gamecard *msg_catch = malloc(sizeof(t_msg_catch_gamecard));
	msg_catch->coord = malloc(sizeof(t_coordenada));
	int offset = 0;
	int cant_letras;
	char *c = "\0";
	memcpy(&(msg_catch->coord->pos_x),data_msg->data + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(msg_catch->coord->pos_y), data_msg->data + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&cant_letras, data_msg->data + offset, sizeof(uint32_t));
	msg_catch->size_pokemon = cant_letras + 1;
	offset += sizeof(uint32_t);
	msg_catch->pokemon = malloc(msg_catch->size_pokemon);
	memcpy(msg_catch->pokemon, data_msg->data + offset,cant_letras);
	memcpy(msg_catch->pokemon + cant_letras, c,1);
	return msg_catch;
}

t_stream *serializar_msg_appeared(t_msg_appeared_broker *msg_appeared)
{
	t_stream *data_msg = malloc(sizeof(t_stream));
	int cant_letras = msg_appeared->size_pokemon - 1;
	data_msg->size = espacio_cache_msg_appeared(msg_appeared);
	data_msg->data = malloc(data_msg->size);
	int offset = 0;
	memcpy(data_msg->data + offset, &(msg_appeared->coordenada->pos_x), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(data_msg->data + offset, &(msg_appeared->coordenada->pos_y), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(data_msg->data + offset, &(cant_letras), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(data_msg->data + offset, msg_appeared->pokemon, cant_letras);
	return data_msg;
}

t_msg_appeared_team *deserializar_msg_appeared(t_stream *data_msg)
{
	t_msg_appeared_team *msg_appeared = malloc(sizeof(t_msg_appeared_team));
	msg_appeared->coord = malloc(sizeof(t_coordenada));
	int offset = 0;
	int cant_letras;
	char *c = "\0";
	memcpy(&(msg_appeared->coord->pos_x),data_msg->data + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(msg_appeared->coord->pos_y), data_msg->data + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&cant_letras, data_msg->data + offset, sizeof(uint32_t));
	msg_appeared->size_pokemon = cant_letras + 1;
	offset += sizeof(uint32_t);
	msg_appeared->pokemon = malloc(msg_appeared->size_pokemon);
	memcpy(msg_appeared->pokemon, data_msg->data + offset,cant_letras);
	memcpy(msg_appeared->pokemon + cant_letras, c,1);
	return msg_appeared;
}

t_stream *serializar_msg_caught(t_msg_caught_broker *msg_caught)
{
	t_stream *data_msg = malloc(sizeof(t_stream));
	data_msg->size = espacio_cache_msg_caught(msg_caught);
	data_msg->data = malloc(data_msg->size);
	memcpy(data_msg->data, &(msg_caught->resultado), data_msg->size);
	return data_msg;
}

t_msg_caught_team *deserializar_msg_caught(t_stream *data_msg)
{
	t_msg_caught_team *msg_caught = malloc(sizeof(t_msg_caught_team));
	memcpy(&msg_caught->resultado, data_msg->data,data_msg->size);
	return msg_caught;
}

t_stream *serializar_msg_localized(t_msg_localized_broker *msg_localized)
{
	t_stream *data_msg = malloc(sizeof(t_stream));
	int cant_letras = msg_localized->size_pokemon - 1;
	int cant_posic = msg_localized->posiciones->cant_posic;
	data_msg->size = espacio_cache_msg_localized(msg_localized);
	data_msg->data = malloc(data_msg->size);
	int offset = 0;
	memcpy(data_msg->data + offset, &cant_posic, sizeof(uint32_t));
	for (int i = 0; i < cant_posic; i ++) {
		offset += sizeof(uint32_t);
		t_coordenada *coordxy = list_get(msg_localized->posiciones->coordenadas, i);
		memcpy(data_msg->data + offset,&(coordxy->pos_x) ,sizeof(uint32_t));
		offset += sizeof(uint32_t);
		memcpy(data_msg->data + offset,&(coordxy->pos_y) ,sizeof(uint32_t));
	}
	offset += sizeof(uint32_t);
	memcpy(data_msg->data + offset, &(cant_letras), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(data_msg->data + offset, msg_localized->pokemon, cant_letras);
	return data_msg;
}

t_msg_localized_team *deserializar_msg_localized(t_stream *data_msg)
{
	t_msg_localized_team *msg_localized = malloc(sizeof(t_msg_localized_team));
	msg_localized->posiciones = malloc(sizeof(t_posiciones_localized));
	msg_localized->posiciones->coordenadas = list_create();
	int offset = 0;
	int cant_letras;
	char *c = "\0";
	memcpy(&(msg_localized->posiciones->cant_posic), data_msg->data + offset, sizeof(uint32_t));
	int cant_elem = msg_localized->posiciones->cant_posic;
	offset += sizeof(uint32_t);
	for (int i = 0; i < cant_elem; i ++) {
		t_coordenada *coord = malloc(sizeof(t_coordenada));
		memcpy(&coord->pos_x, data_msg->data + offset, sizeof(uint32_t));
		offset += sizeof(uint32_t);
		memcpy(&coord->pos_y, data_msg->data + offset, sizeof(uint32_t));
		list_add(msg_localized->posiciones->coordenadas, coord);
		offset += sizeof(uint32_t);
	}
	memcpy(&cant_letras, data_msg->data + offset, sizeof(uint32_t));
	msg_localized->size_pokemon = cant_letras + 1;
	offset += sizeof(uint32_t);
	msg_localized->pokemon = malloc(msg_localized->size_pokemon);
	memcpy(msg_localized->pokemon, data_msg->data + offset,cant_letras);
	memcpy(msg_localized->pokemon + cant_letras, c,1);
	return msg_localized;
}
