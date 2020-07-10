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

	leer_config_broker(RUTA_CONFIG);

	iniciar_log_broker();

	iniciar_estructuras_broker();

	manejo_senial_externa();

	inicio_server_broker();

	return EXIT_SUCCESS;

}
