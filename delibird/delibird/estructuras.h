/*
 * estructuras.h
 *
 *  Created on: 2 may. 2020
 *      Author: Los-Que-Aprueban
 */

#ifndef SRC_ESTRUCTURAS_H_
#define SRC_ESTRUCTURAS_H_
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netdb.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <semaphore.h>
#include <sys/time.h>
#include <unistd.h>

#define RESPUESTA_OK 1001

typedef enum Proceso{
	UNKNOWN_PROC = 299, SUSCRIPTOR, GAMECARD, BROKER, TEAM, GAMEBOY,
} t_proceso;

typedef enum Tipo_Mensaje{
	UNKNOWN_QUEUE = 399,
	NEW_POKEMON,
	APPEARED_POKEMON,
	CATCH_POKEMON,
	CAUGHT_POKEMON,
	GET_POKEMON,
	LOCALIZED_POKEMON,
} t_tipo_mensaje;

typedef enum Codigo_Operacion
{
	ID_MENSAJE = 10,
	MSG_CONFIRMED,
	MSG_ERROR,
	FIN_SUSCRIPCION,
	NEW_BROKER = 20,
	APPEARED_BROKER,
	CATCH_BROKER,
	CAUGHT_BROKER,
	GET_BROKER,
	LOCALIZED_BROKER,
	NEW_GAMECARD = 40,
	CATCH_GAMECARD,
	GET_GAMECARD,
	APPEARED_TEAM = 80,
	CAUGHT_TEAM,
	LOCALIZED_TEAM,
	SUSCRIP_NEW = 120,
	SUSCRIP_APPEARED,
	SUSCRIP_CATCH,
	SUSCRIP_CAUGHT,
	SUSCRIP_GET,
	SUSCRIP_LOCALIZED,
	SUSCRIP_END,
	COLA_VACIA,
} op_code;

typedef enum Resultado_Caught{
	FAIL = 1000,
	OK,
} t_result_caught;

typedef struct Suscriptor_Broker{
	int id_suscriptor;
	bool is_active;
	int cant_msg;
} t_suscriptor_broker;

typedef struct Coordenada{
	int pos_x;
	int pos_y;
} t_coordenada;

/* 	Esta Estructura se usa en la lista
 posiciones de t_msg_localized_broker*/

typedef struct Posicion_Pokemon{
	int pos_x;
	int pos_y;
	int cantidad;
} t_posicion_pokemon;

typedef struct Posiciones_Localized_Team{
	int cant_posiciones;
	t_list *coordenadas;
} t_posiciones_localized_team;

typedef struct Socket_Cliente{
	int cliente_fd;
	int cant_msg_enviados;
} t_socket_cliente_broker;

//------ ESTRUCTURAS PARA MENSAJES QUE RECIBE BROKER - LOS QUE ENVIAN TEAM, GAMECARD y GAMEBOY ----//

typedef struct Msg_New_Broker{
	t_coordenada *coordenada;
	int cantidad;
	char *pokemon;
} t_msg_new_broker;

typedef struct Msg_Catch_Broker{
	t_coordenada *coordenada;
	char *pokemon;
} t_msg_catch_broker;

typedef struct Msg_Get_Broker{
	char *pokemon;
} t_msg_get_broker;

typedef struct Msg_Caught_Broker{
	int id_correlativo;
	t_result_caught resultado;
} t_msg_caught_broker;

typedef struct Msg_Appeared_Broker{
	int id_correlativo;
	t_coordenada *coordenada;
	char *pokemon;
} t_msg_appeared_broker;

typedef struct Msg_Localized_Broker{
	int id_correlativo;
	int cant_posiciones;
	t_list *posiciones;
	char *pokemon;
} t_msg_localized_broker;

typedef struct Msg_Handshake_Suscriptor{
	int id_suscriptor;
	int msjs_recibidos;
} t_handsake_suscript;

//------ ESTRUCTURAS PARA MENSAJES QUE ENVIA BROKER A SUSCRIPTORES-----------//

typedef struct Msg_New_GameCard{
	int id_mensaje;
	t_coordenada *coord;
	int cantidad;
	char *pokemon;
} t_msg_new_gamecard;

typedef struct Msg_Get_GameCard{
	int id_mensaje;
	char *pokemon;
} t_msg_get_gamecard;

typedef struct Msg_Catch_GameCard{
	int id_mensaje;
	t_coordenada *coord;
	char *pokemon;
} t_msg_catch_gamecard;

typedef struct Msg_Appeared_Team{
	int id_mensaje;
	t_coordenada *coord;
	char *pokemon;
} t_msg_appeared_team;

typedef struct Msg_Caught_Team{
	int id_mensaje;
	int id_correlativo;
	t_result_caught resultado;
} t_msg_caught_team;

typedef struct Msg_Localized_Team{
	int id_mensaje;
	int id_correlativo;
	t_posiciones_localized_team *posiciones;
	char *pokemon;
} t_msg_localized_team;

//----------------------------ESTRUCTURAS DE USO COMUN------------------------------//

typedef struct Stream{
	int size;
	void* data;
} t_stream;

typedef struct Paquete{
	op_code codigo_operacion;
	t_stream* buffer;
} t_paquete;


#endif /* SRC_ESTRUCTURAS_H_ */
