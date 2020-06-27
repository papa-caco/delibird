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
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <semaphore.h>
#include <sys/time.h>
#include <sys/mman.h>
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
	SUSCRIBER_ACK = 120,
	SUSCRIP_END,
	COLA_VACIA,
} op_code;

typedef enum Resultado_Caught{
	FAIL = 1000,
	OK,
} t_result_caught;

typedef struct Suscriptor_Broker{
	int id_suscriptor;
	bool enabled;
	sem_t sem_cont_msjs;
	int cant_msg;
} t_suscriptor_broker;

typedef struct Msg_Handshake_Suscriptor{
	uint32_t id_suscriptor;
	uint32_t id_recibido;
	t_tipo_mensaje cola_id;
	uint32_t msjs_recibidos;
} t_handsake_suscript;

typedef struct Coordenada{
	uint32_t pos_x;
	uint32_t pos_y;
} t_coordenada;

/* 	Esta Estructura se usa en la lista
 posiciones de t_msg_localized_broker*/

typedef struct Posicion_Pokemon{
	int pos_x;
	int pos_y;
	int cantidad;
} t_posicion_pokemon;

typedef struct Posiciones_Localized{
	uint32_t cant_posic;
	t_list *coordenadas;
} t_posiciones_localized;

typedef struct Socket_Cliente{
	int cliente_fd;
	int cant_msg_enviados;
} t_socket_cliente_broker;

//------ ESTRUCTURAS PARA MENSAJES QUE RECIBE BROKER - LOS QUE ENVIAN TEAM, GAMECARD y GAMEBOY ----//

typedef struct Msg_Get_Broker{
	uint32_t size_pokemon;
	char *pokemon;
} t_msg_get_broker;

typedef struct Msg_New_Broker{
	t_coordenada *coordenada;
	uint32_t cantidad;
	uint32_t size_pokemon;
	char *pokemon;
} t_msg_new_broker;

typedef struct Msg_Catch_Broker{
	t_coordenada *coordenada;
	uint32_t size_pokemon;
	char *pokemon;
} t_msg_catch_broker;

typedef struct Msg_Localized_Broker{
	uint32_t id_correlativo;
	t_posiciones_localized *posiciones;
	uint32_t size_pokemon;
	char *pokemon;
} t_msg_localized_broker;

typedef struct Msg_Caught_Broker{
	uint32_t id_correlativo;
	t_result_caught resultado;
} t_msg_caught_broker;

typedef struct Msg_Appeared_Broker{
	uint32_t id_correlativo;
	t_coordenada *coordenada;
	uint32_t size_pokemon;
	char *pokemon;
} t_msg_appeared_broker;

//------ ESTRUCTURAS PARA MENSAJES QUE ENVIA BROKER A SUSCRIPTORES-----------//

typedef struct Msg_New_GameCard{
	uint32_t id_mensaje;
	t_coordenada *coord;
	uint32_t cantidad;
	uint32_t size_pokemon;
	char *pokemon;
} t_msg_new_gamecard;

typedef struct Msg_Get_GameCard{
	uint32_t id_mensaje;
	uint32_t size_pokemon;
	char *pokemon;
} t_msg_get_gamecard;

typedef struct Msg_Catch_GameCard{
	uint32_t id_mensaje;
	t_coordenada *coord;
	uint32_t size_pokemon;
	char *pokemon;
} t_msg_catch_gamecard;

typedef struct Msg_Appeared_Team{
	uint32_t id_mensaje;
	uint32_t id_correlativo;
	t_coordenada *coord;
	uint32_t size_pokemon;
	char *pokemon;
} t_msg_appeared_team;

typedef struct Msg_Caught_Team{
	uint32_t id_mensaje;
	uint32_t id_correlativo;
	t_result_caught resultado;
} t_msg_caught_team;

typedef struct Msg_Localized_Team{
	uint32_t id_mensaje;
	uint32_t id_correlativo;
	t_posiciones_localized *posiciones;
	uint32_t size_pokemon;
	char *pokemon;
} t_msg_localized_team;

//----------------------------ESTRUCTURAS DE USO COMUN------------------------------//

typedef struct Stream{
	uint32_t size;
	void* data;
} t_stream;

typedef struct Paquete{
	op_code codigo_operacion;
	t_stream* buffer;
} t_paquete;


#endif /* SRC_ESTRUCTURAS_H_ */
