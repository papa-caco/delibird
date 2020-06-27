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
#include <signal.h>
#include <delibird/estructuras.h>
#include <delibird/conexiones.h>
#include <delibird/mensajeria.h>
#include <delibird/serializaciones.h>

typedef enum Algoritmo_Memoria
{
	BS = 550,
	PARTICIONES,
	SWAP,
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
	int dir_base_part_inicial;
	t_stream *msg_data;
	t_list *sended_suscriptors;
	bool recibido_por_todos;
} t_queue_msg;

typedef struct Cache_Part_Din{
	t_algoritmo_memoria tipo_cache;
	int cnt_id_partition;
	int cnt_order_fifo;
	int min_size_part;
	int dir_base_part;
	int used_space;
	int total_space;
	t_list *partition_table;
	void *partition_repo;
} t_cache_particiones;

typedef struct Particion_Dinamica{
	int id_particion;
	int orden_fifo;
	int dir_base;
	int dir_heap;
	uint32_t data_size;
	bool presencia;
	int id_mensaje;
	t_tipo_mensaje id_cola;
} t_particion_dinamica;

typedef struct Configuracion_Broker
{
	char *ip_broker;
	char *puerto_broker;
	size_t tamano_memoria;
	size_t tamano_swap;
	int tamano_minimo_particion;
	int frecuencia_compactacion;
	t_algoritmo_memoria algoritmo_memoria;
	t_algoritmo_reemplazo algoritmo_reemplazo;
	t_algoritmo_part_libre algoritmo_particion_libre;
	int trigger_mensajes_borrar;
	char *ruta_log;
	char *ruta_swap;
} t_config_broker;



#endif /* SRC_BROKER_ESTRUCTURAS_H_ */
