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

	iniciar_estructuras_broker();

	inicio_server_broker();

	return EXIT_SUCCESS;

}
