/*
 * planificadorACortoPlazo.c
 *
 *  Created on: 7 jul. 2020
 *      Author: utnso
 */

#include "planificadorACortoPlazo.h"

//PLANIFICADOR FIFO

void planificarFifo(){

	while (finalizarProceso == 0) {

		//SEMAFORO QUE RECIBE DEL PLANIFICADOR DE MEDIANO PLAZO
		sem_wait(&sem_planificador_cplazoReady);

	//	printf("SE ACTIVO EL PLANIFICADOR A CORTO PLAZO \n");

		//VERIFICAR SI DEBE MATARSE A SI MISMO (saltear o no su lógica)
		if (finalizarProceso == 0) {
			sem_wait(&sem_cola_ready);
			t_entrenador* entrenadorAEjecutar = (t_entrenador*) queue_pop(
					colaReadyEntrenadores);
			entrenadorEnEjecucion = entrenadorAEjecutar;

			sem_post(&sem_cola_ready);

			log_info(g_logger,"Entrenador %d se movio a la cola de EXEC, porque esta primero en la cola de Ready", entrenadorAEjecutar->id);

			//COMIENZA A EJECUTAR LA FUNCION CORRESPONDIENTE DEL ENTRENADOR
			sem_post(&(entrenadorAEjecutar->sem_entrenador));

			if (strcmp(g_config_team->algoritmo_planificion,"RR")== 0){

				if(entrenadorAEjecutar->estado_entrenador == INTERCAMBIAR){
					int parteEntera = 5/g_config_team->quantum;
					parteEntera++;
					cantidadCambiosDeContexto += parteEntera;
					printf("5555555555555555555555555555555555555555555555555555 \n");
					printf("HUBO INTERCAMBIO Y SE SUMARON %d CAMBIOS DE CONTEXTO \n", parteEntera);
					printf("5555555555555555555555555555555555555555555555555555 \n");
				} else{
					cantidadCambiosDeContexto++;
				}

			}else {
				cantidadCambiosDeContexto++;
			}



			//ESPERA EL SIGNAL DEL ENTRENADOR PARA QUE COMPLETE SU FUNCION
			sem_wait(&sem_planificador_cplazoEntrenador);


			sem_wait(&sem_cola_blocked);
			queue_push(colaBlockedEntrenadores, entrenadorAEjecutar);
			sem_post(&sem_cola_blocked);

			//LE MANDO EL SIGNAL AL PLANIFICADOR DE MEDIANO PLAZO PARA QUE VERIFIQUE LA COLA DE BLOCKED
			//O FINALICE TODO SI CORRESPONDE
			sem_post(&sem_planificador_mplazo);
		}


	}

	sem_post(&sem_terminar_todo);

}
