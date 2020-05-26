/*
 * servidor.c
 *
 *  Created on: 3 mar. 2019
 *      Author: utnso
 */

#include "servidor.h"

int main(void)
{
	iniciar_logger();
	iniciar_estructuras();

	iniciar_server_broker(IP, PUERTO, g_logger);

	eliminar_estructuras();
	log_destroy(g_logger);

	return EXIT_SUCCESS;
}
