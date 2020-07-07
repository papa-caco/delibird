/*
 * planificadorACortoPlazo.c
 *
 *  Created on: 7 jul. 2020
 *      Author: utnso
 */

#include "planificadorACortoPlazo.h"

//PLANIFICADOR FIFO

void planificarFifo(){

	while(1){

			//SEMAFORO QUE RECIBE DEL PLANIFICADOR DE MEDIANO PLAZO
			sem_wait(&sem_planificador_cplazoReady);

			t_entrenador* entrenadorAEjecutar = (t_entrenador*) queue_pop(colaReadyEntrenadores);

			//COMIENZA A EJECUTAR LA FUNCION CORRESPONDIENTE DEL ENTRENADOR
			sem_post(&(entrenadorAEjecutar->sem_entrenador));

			//ESPERA EL SIGNAL DEL ENTRENADOR PARA QUE COMPLETE SU FUNCION
			sem_wait(&sem_planificador_cplazoEntrenador);

			//VERIFCO SI PUEDE PASAR A EXIT, DEADLOCK, O BLOCKED
			verificarYCambiarEstadoEntrenador(entrenadorAEjecutar);

			if(entrenadorAEjecutar->estado_entrenador == EXIT){

				//SI ESTA EN EXIT, LO MANDO A LA COLA CORRESPONDIENTE
				sem_post(&(entrenadorAEjecutar->sem_entrenador));
				queue_push(colaExitEntrenadores, entrenadorAEjecutar);

			}else
			{

				queue_push(colaBlockedEntrenadores, entrenadorAEjecutar);

			}

			//LE MANDO EL SIGNAL AL PLANIFICADOR DE MEDIANO PLAZO PARA QUE VERIFIQUE LA COLA DE BLOCKED
			sem_post(&sem_planificador_mplazo);

		}
}
