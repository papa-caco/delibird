/*
 ============================================================================
 Name        : broker_estructuras.h

 Author      : Los Que Aprueban.

 Created on	 : 16 may. 2020

 Version     :

 Description :
 ============================================================================
 */
#ifndef SRC_BROKER_ESTRUCTURAS_H_
#define SRC_BROKER_ESTRUCTURAS_H_
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

typedef struct Cola_Broker{
	t_tipo_mensaje id_queue;
	t_list *mensajes_cola;
	t_list *suscriptores;
} t_broker_queue;

typedef struct Mensaje_Cola{
	int id_mensaje;
	int id_correlativo;
	t_tipo_mensaje tipo_mensaje;
	int id_cache_segmemt;
	t_stream *msg_data;
	t_list *sended_suscriptors;
	t_list *receipt_confirmed;
	bool es_victima_reemplazo;
} t_queue_msg;

typedef struct Configuracion_Broker
{
	char *ip_broker;
	char *puerto_broker;
	size_t tamano_memoria;
	int tamano_minimo_particion;
	int freceuncia_compactacion;
	t_algoritmo_memoria algoritmo_memoria;
	t_algoritmo_reemplazo algoritmo_reemplazo;
	t_algoritmo_part_libre algoritmo_particion_libre;
	char *ruta_log;
} t_config_broker;



#endif /* SRC_BROKER_ESTRUCTURAS_H_ */
