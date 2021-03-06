/*
 * servidor.c
 *
 *  Created on: 3 mar. 2019
 *      Author: utnso
 */

#include "team.h"

int main(void) {


	iniciar_team();

	iniciar_variables_globales();


	sem_wait(&sem_mutex_msjs);
	iniciar_entrenadores_and_objetivoGlobal();

	int id_recibido;

	sem_post(&sem_mutex_msjs);

	pthread_create(&threadPlanificadorCP, NULL, (void*) planificarFifo, NULL);

	pthread_detach(threadPlanificadorCP);

	pthread_create(&threadPlanificadorMP, NULL, (void*) planificadorMedianoPlazo, NULL);

	pthread_detach(threadPlanificadorMP);

	iniciar_suscripciones_broker();

	enviar_msjs_get_objetivos();

	lanzar_reconexion_broker(g_logger);

	//AL finalizar el programase deben liberar las listas de las colas de estados y la de objetivoGlobal
	//liberar_lista(objetivoGlobalEntrenadores);

	manejo_senial_externa_tm();

	sleep(1);

	inicio_server_team();

	//log_destroy(g_logger);
	//LAS VARIABLES "objetivoGlobal" y "new" TIENEN QUE SER VARIABLES GLOBALES, es decir que vamos a tener que sacar
	//su hardcodeo de las funciones "iniciar_entrenadores_and_objetivoGLobal" y "cargar_objetivoGLobal"


	return EXIT_SUCCESS;
}
