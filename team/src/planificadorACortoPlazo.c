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

			/*while(entrenadorAEjecutar->estado_entrenador == MOVERSE_A_POKEMON && (list_size(pokemonesLibresEnElMapa) == 0)){

				queue_push(colaReadyEntrenadores, entrenadorAEjecutar);

				entrenadorAEjecutar = (t_entrenador*) queue_pop(
									colaReadyEntrenadores);


			}*/
			sem_post(&sem_cola_ready);

			log_info(g_logger,"Entrenador %d se movio a la cola de Exec, porque esta primero en la cola de Ready", entrenadorAEjecutar->id);

			//COMIENZA A EJECUTAR LA FUNCION CORRESPONDIENTE DEL ENTRENADOR
			sem_post(&(entrenadorAEjecutar->sem_entrenador));

			cantidadCambiosDeContexto++;

			//ESPERA EL SIGNAL DEL ENTRENADOR PARA QUE COMPLETE SU FUNCION
			sem_wait(&sem_planificador_cplazoEntrenador);

			//printf("ESTOY ANTES DEL VERIFICAR \n");
			//VERIFCO SI PUEDE PASAR A EXIT, DEADLOCK, O BLOCKED
			sem_wait(&(entrenadorAEjecutar->mutex_entrenador));
			verificarYCambiarEstadoEntrenador(entrenadorAEjecutar);
			sem_post(&(entrenadorAEjecutar->mutex_entrenador));
			//printf("ESTOY DESPUES DEL VERIFICAR \n");

			if (entrenadorAEjecutar->estado_entrenador == EXIT) { //CASO DESPUES DEL INTERCAMBIO
				//SI ESTA EN EXIT, LO MANDO A LA COLA CORRESPONDIENTE
				//Este semáforo ya no hace falta, el entreandor finaliza solo cuando cambia su estado a EXIT.
				sem_post(&(entrenadorAEjecutar->sem_entrenador));
				sem_wait(&sem_cola_exit);
				queue_push(colaExitEntrenadores, entrenadorAEjecutar);
				sem_post(&sem_cola_exit);

			} else {
				sem_wait(&sem_cola_blocked);
				queue_push(colaBlockedEntrenadores, entrenadorAEjecutar);
				sem_post(&sem_cola_blocked);
			}

			//LE MANDO EL SIGNAL AL PLANIFICADOR DE MEDIANO PLAZO PARA QUE VERIFIQUE LA COLA DE BLOCKED
			//O FINALICE TODO SI CORRESPONDE
			sem_post(&sem_planificador_mplazo);
		}

	}

	sem_post(&sem_terminar_todo);

}
