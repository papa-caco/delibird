/*
 ============================================================================
 Name        : broker_serializ.h

 Author      : Los Que Aprueban.

 Created on	 : 16 may. 2020

 Version     :

 Description :
 ============================================================================
 */
#ifndef SRC_BROKER_SERIALIZ_H_
#define SRC_BROKER_SERIALIZ_H_
#include "broker_estructuras.h"
#include "broker_oper.h"
#include "broker_utils.h"

//-----FIRMA DE LAS FUNCIONES PARA SERIALIZAR Y DESERIALIZAR LOS MENSAJES QUE RECIBE BROKER Y GUARDA EN CACHE---//

t_stream *serializar_msg_get(t_msg_get_broker *msg_get);

t_msg_get_gamecard *deserializar_msg_get(t_stream *data_msg);

t_stream *serializar_msg_new(t_msg_new_broker *msg_new);

t_msg_new_gamecard *deserializar_msg_new(t_stream *data_msg);

t_stream *serializar_msg_catch(t_msg_catch_broker *msg_catch);

t_msg_catch_gamecard *deserializar_msg_catch(t_stream *data_msg);

t_stream *serializar_msg_appeared(t_msg_appeared_broker *msg_appeared);

t_msg_appeared_team *deserializar_msg_appeared(t_stream *data_msg);

t_stream *serializar_msg_caught(t_msg_caught_broker *msg_caught);

t_msg_caught_team *deserializar_msg_caught(t_stream *data_msg);

t_stream *serializar_msg_localized(t_msg_localized_broker *msg_localized);

t_msg_localized_team *deserializar_msg_localized(t_stream *data_msg);

#endif /* SRC_BROKER_SERIALIZ_H_ */
