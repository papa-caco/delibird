/*
 ============================================================================
 Name        : broker.c

 Author      : Los Que Aprueban.

 Created on	 : 9 may. 2020

 Version     :

 Description :
 ============================================================================
 */
#include "broker.h"

int main(void) {

	leer_config_broker("/home/utnso/config/broker.config");

	iniciar_log_broker();

	log_info(g_logger,"Algoritmo Memoria: %s = %d",
			config_get_string_value(g_config,"ALGORITMO_MEMORIA"), g_config_broker->algoritmo_memoria);

	t_list *lista = lista_numerica();

	log_info(g_logger,"Cant Elem: %d", lista->elements_count);

	char *cadena = concat_coord_x_y(lista);

	log_info(g_logger,"%s", cadena);

	return 0;

}
