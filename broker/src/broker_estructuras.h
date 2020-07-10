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
#include <time.h>
#include <math.h>
#include <commons/process.h>
#include <commons/bitarray.h>
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
	int cnt_order_lru;
	int min_size_part;
	int dir_base_part;
	int used_space;
	int total_space;
	t_list *partition_table;
	void *partition_repo;
} t_cache_particiones;

typedef struct Particion_Dinamica
{
	int id_particion;
	int orden_fifo;
	int last_used;
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
	int tamano_minimo_particion;
	int frecuencia_compactacion;
	t_algoritmo_memoria algoritmo_memoria;
	t_algoritmo_reemplazo algoritmo_reemplazo;
	t_algoritmo_part_libre algoritmo_particion_libre;
	int bit_arrays_bs;
	char *ruta_log;
	bool show_bitmaps_bs;
	bool show_logs_on_screen;
	char *dump_file;
} t_config_broker;

typedef struct Posiciones_Buddy
{
	int orden;
	int buddy_size;
	int cant_buddys;
	int free_buddys;
	t_bitarray *bitmap_buddy;
} t_posicion_buddy;

typedef struct Particion_Buddy
{
	int posicion;
	int tamano;
	int orden_fifo;
	int last_used;
	uint32_t data_size;
	int id_mensaje;
	t_tipo_mensaje id_cola;
} t_particion_buddy;

typedef struct Cache_Buddy_System{
	t_algoritmo_memoria tipo_cache;
	int cnt_order_fifo;
	int cnt_order_lru;
	int min_size_part;
	int used_space;
	int total_space;
	int cant_bitarrays;
	int max_size_msg;
	t_list *posiciones_buddy;
	t_list *buddy_table;
	void *buddy_repo;
} t_cache_buddy;


#endif /* SRC_BROKER_ESTRUCTURAS_H_ */
