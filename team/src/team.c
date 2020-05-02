/*
 * servidor.c
 *
 *  Created on: 3 mar. 2019
 *      Author: utnso
 */

#include "team.h"

int main(void)
{
	//iniciar_team();

	t_list * listita = extraer_posiciones_entrenadores();

	for (int procesados = 0; procesados < 3; procesados++) {

		t_posicion_entrenador* posicion = list_get(listita, procesados);

		printf("-------------------El contenido de la posicion DENTRO DE LA LISTA es %d \n",
				posicion ->pos_x);
		printf("-------------------El contenido de la posicion DENTRO DE LA LISTA es %d \n",
						posicion ->pos_y);

	}

	list_destroy(listita);

	//log_destroy(g_logger);

	return EXIT_SUCCESS;
}
