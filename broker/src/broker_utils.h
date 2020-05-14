/*
 ============================================================================
 Name        : broker_utils.h

 Author      : Los Que Aprueban.

 Created on	 : 9 may. 2020

 Version     :

 Description :
 ============================================================================
 */
#ifndef SRC_BROKER_UTILS_H_
#define SRC_BROKER_UTILS_H_

#include <delibird/estructuras.h>
#include <delibird/conexiones.h>
#include <delibird/mensajeria.h>
#include <delibird/serializaciones.h>

typedef enum Algoritmo_Memoria
{
	BS = 550,
	PARTICIONES,
} t_algoritmo_memoria;

typedef enum Algoritmo_Reemplazo
{
	FIFO = 560,
	LRU,
} t_algoritmo_reemplazo;

typedef enum Algoritmo_Particion_Libre
{
	FF = 570,
	BF,
} t_algoritmo_part_libre;

typedef struct Configuracion_Broker
{
	char *ip_broker;
	char *puerto_broker;
	int tamano_memoria;
	int tamano_minimo_particion;
	int freceuncia_compactacion;
	t_algoritmo_memoria algoritmo_memoria;
	t_algoritmo_reemplazo algoritmo_reemplazo;
	t_algoritmo_part_libre algoritmo_particion_libre;
	char *ruta_log;
} t_config_broker;

//-------------VARIABLES GLOBALES-------------------//

t_log *g_logger;

t_config *g_config;

t_config_broker *g_config_broker;

int g_msg_counter;

t_list *g_queue_get_pokemon;

pthread_t g_thread_server;


//---------------FIRMA DE FUNCIONES------------------//

void iniciar_log_broker(void);

void leer_config_broker(char *path);

void iniciar_estructuras_broker(void);

void inicio_server_broker(void);

void atender_publicacion(op_code cod_op, t_socket_cliente_broker *socket);

void atender_suscripcion(op_code cod_op, t_socket_cliente_broker *socket);

t_algoritmo_memoria algoritmo_memoria(char *valor);

t_algoritmo_part_libre algoritmo_part_libre(char *valor);

t_algoritmo_reemplazo algoritmo_reemplazo(char *valor);

void incremento_cnt_id_mensajes(void);

t_list *lista_numerica(void);

#endif /* SRC_BROKER_UTILS_H_ */
