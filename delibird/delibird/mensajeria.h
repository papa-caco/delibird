/*
 * mensajeria.h
 *
 *  Created on: 4 may. 2020
 *
 *  Author: Los-Que-Aprueban
 *
 *
 */

#ifndef DELIBIRD_MENSAJERIA_H_
#define DELIBIRD_MENSAJERIA_H_
#include "estructuras.h"
#include "serializaciones.h"

// -------------------FUNCIONES PARA ENVIO DE MENSAJES DESDE EL BROKER A SUSCRIPTORES--------//

ssize_t enviar_msj_cola_vacia(t_socket_cliente_broker *socket, t_log *logger, int id_suscriptor);

void enviar_msj_suscript_end(t_socket_cliente_broker *socket, t_log * logger, int id_suscriptor);

ssize_t enviar_msj_get_gamecard(t_socket_cliente_broker *socket, t_log *logger, t_msg_get_gamecard *msg_get_gamecard);

ssize_t enviar_msj_new_gamecard(t_socket_cliente_broker *socket, t_log *logger, t_msg_new_gamecard *msg_new_gamecard);

ssize_t enviar_msj_catch_gamecard(t_socket_cliente_broker *socket, t_log *logger, t_msg_catch_gamecard *msg_catch_gamecard);

ssize_t enviar_msj_appeared_team(t_socket_cliente_broker *socket, t_log *logger, t_msg_appeared_team *msg_appeared_team);

ssize_t enviar_msj_caught_team(t_socket_cliente_broker *socket, t_log *logger, t_msg_caught_team *msg_caught_team);

ssize_t enviar_msj_localized_team(t_socket_cliente_broker *socket, t_log *logger, t_msg_localized_team *msg_localized_team);

void enviar_id_mensaje(int socket_cliente, t_log *logger, int id_mensaje);

// -------------------FUNCIONES PARA ENVIO DE MENSAJES DESDE EL GAMECARD A BROKER o GAMEBOY---------//

void enviar_msj_appeared_broker(int cliente_fd, t_log *logger, t_msg_appeared_broker *msg_appeared_broker);

void enviar_msj_caught_broker(int cliente_fd, t_log *logger, t_msg_caught_broker *msg_caught_broker);

void enviar_msj_localized_broker(int cliente_fd, t_log *logger, t_msg_localized_broker *msg_localized_broker);

void enviar_msj_get_broker(int cliente_fd, t_log *logger,t_msg_get_broker *msg_get_broker);

void enviar_msj_catch_broker(int cliente_fd, t_log *logger, t_msg_catch_broker *msg_catch_broker);

void enviar_msj_new_broker(int cliente_fd, t_log *logger, t_msg_new_broker *msg_new_broker);

void enviar_solicitud_fin_suscripcion(int socket_cliente, t_log *logger, t_handsake_suscript *handshake);

// ------FUNCIONES PARA RECEPCION DE MENSAJES DEL DEL BROKER ENVIADOS POR OTROS PROCESOS----------//

t_msg_new_broker *rcv_msj_new_broker(int socket_cliente, t_log *logger);

t_msg_catch_broker *rcv_msj_catch_broker(int socket_cliente, t_log *logger);

t_msg_get_broker *rcv_msj_get_broker(int socket_cliente, t_log *logger);

t_msg_caught_broker *rcv_msj_caught_broker(int socket_cliente, t_log *logger);

t_msg_appeared_broker *rcv_msj_appeared_broker(int socket_cliente, t_log *logger);

t_msg_localized_broker *rcv_msj_localized_broker(int socket_cliente, t_log *logger);

t_handsake_suscript *rcv_msj_handshake_suscriptor(int socket_cliente);

void enviar_msj_handshake_suscriptor(int socket_cliente, t_log *logger, t_handsake_suscript *handshake);

// --------------FUNCIONES PARA RECEPCION DE MENSAJES DEL GAMECARD ENVIADOS POR BROKER O GAMBOY----------//

t_msg_new_gamecard *rcv_msj_new_gamecard(int socket_cliente, t_log *logger);

t_msg_catch_gamecard *rcv_msj_catch_gamecard(int socket_cliente, t_log *logger);

t_msg_get_gamecard *rcv_msj_get_gamecard(int socket_cliente, t_log *logger);

// --------------FUNCIONES PARA RECEPCION DE MENSAJES DEL TEAM ENVIADOS POR BROKER------------------------//

t_msg_caught_team *rcv_msj_caught_team(int socket_cliente, t_log *logger);

t_msg_appeared_team *rcv_msj_appeared_team(int socket_cliente, t_log *logger);

t_msg_localized_team *rcv_msj_localized_team(int socket_cliente, t_log *logger);

// --------------FUNCIONES PARA RECEPCION y ENVIO DE MENSAJES DE USO COMUN--------------------------------//

void enviar_msg_confirmed(int socket_cliente, t_log *logger);

void enviar_mensaje_error(int socket_cliente,t_log *logger, char *mensajeError);

int rcv_codigo_operacion(int socket_cliente);

int rcv_msg_confirmed(int socket_cliente, t_log *logger);

int rcv_msg_suscrip_end(int socket_cliente);

int rcv_msj_cola_vacia(int socket_cliente, t_log *logger);

int rcv_id_mensaje(int socket_cliente, t_log *logger);

// -----------------------------BORRADO DE MENSAJES - LIBERACION DE MEMORIA------------------------------//

void eliminar_msg_localized_team(t_msg_localized_team *msg_localized_team);

void eliminar_msg_new_broker(t_msg_new_broker *msg_new_broker);

void eliminar_msg_catch_broker(t_msg_catch_broker *msg_catch_broker);

void eliminar_msg_get_broker(t_msg_get_broker *msg_get_broker);

void eliminar_msg_appeared_broker(t_msg_appeared_broker *msg_appeared_broker);

void eliminar_msg_localized_broker(t_msg_localized_broker *msg_localized_broker);

void eliminar_msg_new_gamecard(t_msg_new_gamecard *msg_new_gamecard);

void eliminar_msg_catch_gamecard(t_msg_catch_gamecard *msg_catch_gamecard);

void eliminar_msg_get_gamecard(t_msg_get_gamecard *msg_get_gamecard);

void eliminar_msg_appeared_team(t_msg_appeared_team *msg_appeared_team);

// -------------------------------------------FUNCIONES AUXILIARES---------------------------------------//

void *recibir_buffer(int socket_cliente, int *size);

int tamano_recibido(int bytes);

char *result_caught(t_result_caught resultado);

char *concat_posiciones(t_list *posiciones);

void eliminar_lista(t_list *lista);

char *obtengo_cola_suscripcion(op_code codigo_operacion);

char *nombre_cola(t_tipo_mensaje cola);

#endif /* DELIBIRD_MENSAJERIA_H_ */
