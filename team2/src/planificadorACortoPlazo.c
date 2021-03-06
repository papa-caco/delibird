/*
 * planificadorACortoPlazo.c
 *
 *  Created on: 7 jul. 2020
 *      Author: utnso
 */

#include "planificadorACortoPlazo.h"

//PLANIFICADOR FIFO

void planificarFifo(){
	bool esLaPrimeraVez=true;

	while (finalizarProceso == 0) {

		//SEMAFORO QUE RECIBE DEL PLANIFICADOR DE MEDIANO PLAZO

		//int sem_val = 99;
		//sem_getvalue(&sem_planificador_cplazoReady, &sem_val);
		//printf("sem_plani_cplazoReady: %d\n", sem_val);

		sem_wait(&sem_planificador_cplazoReady);

	//	printf("SE ACTIVO EL PLANIFICADOR A CORTO PLAZO \n");

		//VERIFICAR SI DEBE MATARSE A SI MISMO (saltear o no su lógica)
		if (finalizarProceso == 0) {
			sem_wait(&sem_cola_ready);

			if ((!strcmp(g_config_team->algoritmo_planificion, "SJF-CD"))
					|| (!strcmp(g_config_team->algoritmo_planificion, "SJF-SD"))) {
				if(!esLaPrimeraVez){
					puts("entra");
					ordenar_cola_ready_estimacion();
				} else{
					esLaPrimeraVez=false;
				}

			}

			t_entrenador* entrenadorAEjecutar = (t_entrenador*) queue_pop(
					colaReadyEntrenadores);

			entroUnoAReady = queue_size(colaReadyEntrenadores);

			entrenadorEnEjecucion = entrenadorAEjecutar;

			sem_post(&sem_cola_ready);

			log_info(g_logger,"Entrenador %d se movio a la cola de EXEC, porque esta primero en la cola de Ready", entrenadorAEjecutar->id);
			/*log_trace(g_logger,"Entrenador %d|inst_act:%d|est_act:%.2f|ant_est:%.2f|real_est:%.2f|ejec_ant:%d",entrenadorAEjecutar->id,
					entrenadorAEjecutar->instruccion_actual,entrenadorAEjecutar->estimacion_actual,
					entrenadorAEjecutar->estimacion_anterior, entrenadorAEjecutar->estimacion_real,	entrenadorAEjecutar->ejec_anterior);*/
			//COMIENZA A EJECUTAR LA FUNCION CORRESPONDIENTE DEL ENTRENADOR
			sem_post(&(entrenadorAEjecutar->sem_entrenador));

			if (strcmp(g_config_team->algoritmo_planificion,"RR")== 0){

				if(entrenadorAEjecutar->estado_entrenador == INTERCAMBIAR){
					int parteEntera = 5/g_config_team->quantum;
					parteEntera++;
					cantidadCambiosDeContexto += parteEntera;
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
		//int sem_val2 = 99;
		//sem_getvalue(&sem_planificador_mplazo, &sem_val2);
		//printf("sem_plani_mplazo-post: %d\n", sem_val2);


	}

	sem_post(&sem_terminar_todo);

}


void ordenar_cola_ready_estimacion(){
	t_list* lista = list_create();
	int cantidadEnReady = queue_size(colaReadyEntrenadores);

	for(int i=0; i<cantidadEnReady; i++){
		t_entrenador* unEntrenador = queue_pop(colaReadyEntrenadores);
		list_add(lista, unEntrenador);
	}

    bool is_estimacion_menor(t_entrenador * entrenador1, t_entrenador * entrenador2){
        return ( (entrenador1->estimacion_real < entrenador2->estimacion_real) || (entrenador1->estimacion_real == entrenador2->estimacion_real) );
    }

    list_sort(lista, (void*)is_estimacion_menor);

    for (int i = 0; i < cantidadEnReady; i++) {
		t_entrenador* unEntrenador = list_get(lista, i);
		queue_push(colaReadyEntrenadores, unEntrenador);
	}

    list_clean(lista);
    list_destroy(lista);
}




