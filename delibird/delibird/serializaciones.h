/*
 * serializaciones.h
 *
 *  Created on: 3 may. 2020
 *      Author: utnso
 */

#ifndef SRC_SERIALIZACIONES_H_
#define SRC_SERIALIZACIONES_H_
#include "estructuras.h"

// -------------------FUNCIONES DE SERIALIZACION PARA MENSAJES QUE ENVIA BROKER A SUSCRIPTORES--------//

t_paquete *empaquetar_msj_cola_vacia(int id_suscriptor);

t_paquete *empaquetar_msj_suscript_end(int id_suscriptor);

t_paquete *empaquetar_msg_new_gamecard(t_msg_new_gamecard *msg_new_gamecard);

t_paquete *empaquetar_msg_get_gamecard(t_msg_get_gamecard *msg_get_gamecard);

t_paquete *empaquetar_msg_catch_gamecard(t_msg_catch_gamecard *msg_catch_gamecard);

t_paquete *empaquetar_msg_appeared_team(t_msg_appeared_team *msg_appeared_team);

t_paquete *empaquetar_msg_caught_team(t_msg_caught_team *msg_caught_team);

t_paquete *empaquetar_msg_localized_team(t_msg_localized_team *msg_localized_team);

int coordenada_posiciones(t_posiciones_localized* posiciones, int indice);

// --------------FUNCIONES DE SERIALIZACION PARA MENSAJES QUE ENVIAN GAMECARD y TEAM al BROKER o GAMEBOY-----------//

t_paquete *empaquetar_msg_appeared_broker(t_msg_appeared_broker *msg_appeared_broker);

t_paquete *empaquetar_msg_caught_broker(t_msg_caught_broker *msg_caught_broker);

t_paquete *empaquetar_msg_localized_broker(t_msg_localized_broker *msg_localized);

t_paquete *empaquetar_msg_catch_broker(t_msg_catch_broker *msg_catch_broker);

t_paquete *empaquetar_msg_get_broker(t_msg_get_broker *msg_get_broker);

t_paquete *empaquetar_msg_new_broker(t_msg_new_broker *msg_new_broker);

t_paquete *empaquetar_msg_handshake_suscript(t_handsake_suscript *handshake, op_code codigo_operacion);

t_paquete *empaquetar_fin_suscripcion(int id_suscriptor);

t_paquete *empaquetar_id_mensaje(int id_mensaje);

// -------------------FUNCIONES DE DESERIALIZACION PARA MENSAJES QUE RECIBE BROKER------------------------//

t_msg_new_broker *deserializar_msg_new_broker(void *msg, int size);

t_msg_catch_broker *deserializar_msg_catch_broker(void *msg, int size);

t_msg_get_broker *deserializar_msg_get_broker(void *msg, int size);

t_msg_caught_broker *deserializar_msg_caught_broker(void *msg, int size);

t_msg_appeared_broker *deserializar_msg_appeared_broker(void *msg, int size);

t_msg_localized_broker *deserializar_msg_localized_broker(void *msg, int size);

t_handsake_suscript *deserializar_handshake_suscriptor(void *msg, int size);

// -------------------FUNCIONES DE DESERIALIZACION PARA MENSAJES QUE RECIBE GAMECARD----------------------//

t_msg_new_gamecard *deserializar_msg_new_gamecard(void *msg, int size);

t_msg_catch_gamecard *deserializar_msg_catch_gamecard(void *msg, int size);

t_msg_get_gamecard *deserializar_msg_get_gamecard(void *msg, int size);

// -------------------FUNCIONES DE DESERIALIZACION PARA MENSAJES QUE RECIBE TEAM-------------------------//

t_msg_caught_team *deserializar_msg_caught_team(void *msg);

t_msg_appeared_team *deserializar_msg_appeared_team(void *msg, int size);

t_msg_localized_team *deserializar_msg_localized_team(void *msg, int size);

// --------------------- FUNCIONES DE SERIALIZACION PARA USO COMUN -------------------------------------//

t_paquete *empaquetar_msg_confirmed(void);

t_paquete *empaquetar_msg_fail(char *mensajeError);

void *serializar_paquete(t_paquete* paquete, int bytes);

void eliminar_paquete(t_paquete *paquete);

int tamano_paquete(t_paquete *paquete);

#endif /* SRC_SERIALIZACIONES_H_ */
