#ifndef CONEXIONES_H_
#define CONEXIONES_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<commons/txt.h>
#include<commons/collections/list.h>
#include<string.h>
#include<pthread.h>
#include<stdbool.h>
#include<delibird/estructuras.h>
#include<delibird/conexiones.h>
#include<delibird/serializaciones.h>
#include<delibird/mensajeria.h>

#define IP "127.0.0.1"
#define PUERTO "5001"
#define IP_BROKER "127.0.0.1"
#define RUTA_LOG "logs/testserver.log"
#define RUTA_ARCHIVOS "config/"
#define PUERTO_BROKER "6009"
#define ID_MSG_RTA 5535
#define RESPUESTA_OK 1001

//-----------------Variables Globales----------------------------

pthread_t thread;

t_log *g_logger;

t_list *g_suscriptores;

//-----------------Firma de Funciones----------------------------

void process_request(op_code cod_op, t_socket_cliente_broker *socket);

void process_suscript(op_code cod_op, t_socket_cliente_broker *socket);

void start_suscription(t_list *suscriptores,t_handsake_suscript *suscrpitor);

void stop_suscription(t_list *suscriptores, t_handsake_suscript *suscrpitor);

bool is_active_suscriptor(t_list *suscriptores, int id_suscriptor);

t_suscriptor_broker *obtengo_suscriptor(t_list *suscriptores, int id_suscriptor);

void remove_ended_suscriptor(t_list *suscriptores, int id_suscriptor);

void delete_suscriptor(t_list *suscriptores, void *suscriptor);

void liberar_listas(char** lista);

void devolver_msj_caught_broker(t_msg_catch_gamecard *msg_catch_gamecard, int cliente_fd);

void devolver_msj_appeared_broker(t_msg_new_gamecard *msg_new_gamecard,int cliente_fd);

void devolver_posiciones(int socket_cliente, int id_correlativo, char* pokemon, int* encontroPokemon); //Hace un send de la lista de posiciones y cantidad de un pokemon

void enviar_msjs_cola_broker(t_tipo_mensaje cola, t_socket_cliente_broker *socket);

void enviar_msjs_get(t_socket_cliente_broker *socket);

void enviar_msjs_new(t_socket_cliente_broker *socket);

void enviar_msjs_appeared(t_socket_cliente_broker *socket);

void enviar_msjs_catch(t_socket_cliente_broker *socket);

void enviar_msjs_caught(t_socket_cliente_broker *socket);

void enviar_msjs_localized(t_socket_cliente_broker *socket);

void enviar_msj_localized_al_broker(t_msg_localized_broker *msg_localized);

void connect_broker_send_msg(t_msg_localized_broker *msg_localized);

t_posiciones_localized *generar_posiciones_localized(int cantidad);

void liberar_lista_posiciones(t_list* list);

int cant_coordenadas(int valor);

void iniciar_estructuras(void);

void eliminar_estructuras(void);

void iniciar_logger(void);

#endif /* CONEXIONES_H_ */
