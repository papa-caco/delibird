/*
 * servidor.c
 *
 *  Created on: 3 mar. 2019
 *      Author: utnso
 */

#include "team.h"

int main(void)
{
	iniciar_logger();

	iniciar_servidor();

	log_destroy(g_logger);

	return EXIT_SUCCESS;
}
