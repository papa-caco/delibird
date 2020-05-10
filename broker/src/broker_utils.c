/*
 ============================================================================
 Name        : broker_utils.c

 Author      : Los Que Aprueban.

 Created on	 : 9 may. 2020

 Version     :

 Description :
 ============================================================================
 */
#include "broker_utils.h"

t_list *lista_numerica(void)
{
	t_list *lista = list_create();
	for(int i = 0; i < 8; i++){
		int valor = rand() % 99;
		void *elem = malloc(sizeof(int));
		memcpy(elem, &valor, sizeof(int));
		list_add(lista,elem);
	}
	return lista;
}

void leer_config_broker(char *path) {
	g_config = config_create(path);
	g_config_broker = malloc(sizeof(t_config_broker));
	g_config_broker->ip_broker = config_get_string_value(g_config,"IP_BROKER");
	g_config_broker->puerto_broker = config_get_string_value(g_config,"PUERTO_BROKER");
	g_config_broker->tamano_memoria = config_get_int_value(g_config,"TAMANO_MEMORIA");
	g_config_broker->tamano_minimo_particion = config_get_int_value(g_config,"TAMANO_MINIMO_PARTICION");
	g_config_broker->algoritmo_memoria = algoritmo_memoria(config_get_string_value(g_config,"ALGORITMO_MEMORIA"));
	g_config_broker->algoritmo_particion_libre = algoritmo_part_libre(config_get_string_value(g_config,"ALGORITMO_PARTICION_LIBRE"));
	g_config_broker->algoritmo_reemplazo = algoritmo_reemplazo(config_get_string_value(g_config,"ALGORITMO_REEMPLAZO"));
	g_config_broker->freceuncia_compactacion = config_get_int_value(g_config,"FRECUENCIA_COMPACTACION");
	g_config_broker->ruta_log = config_get_string_value(g_config, "LOG_FILE");
}

t_algoritmo_memoria algoritmo_memoria(char *valor)
{
	t_algoritmo_memoria algoritmo;
	if(strcmp(valor,"PARTICIONES") == 0) {
		algoritmo = PARTICIONES;
	}
	else if (strcmp(valor,"BS") == 0) {
		algoritmo = BS;
	}
	else {
		algoritmo = 0;
	}
	return algoritmo;
}

t_algoritmo_part_libre algoritmo_part_libre(char *valor)
{
	t_algoritmo_part_libre algoritmo;
	if(strcmp(valor,"FF") == 0) {
		algoritmo = FF;
	}
	else if (strcmp(valor,"BF") == 0) {
		algoritmo = BF;
	}
	else {
		algoritmo = 0;
	}
	return algoritmo;
}

t_algoritmo_reemplazo algoritmo_reemplazo(char *valor)
{
	t_algoritmo_reemplazo algoritmo;
	if(strcmp(valor,"FIFO") == 0) {
		algoritmo = FIFO;
	}
	else if (strcmp(valor,"LRU") == 0) {
		algoritmo = LRU;
	}
	else {
		algoritmo = 0;
	}
	return algoritmo;
}

void iniciar_log_broker(void) {
	g_logger = log_create(g_config_broker->ruta_log, "BROKER", 1, LOG_LEVEL_INFO);
}
