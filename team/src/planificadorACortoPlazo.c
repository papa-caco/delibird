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

			if ((!strcmp(g_config_team->algoritmo_planificion, "SJF-CD"))
					|| (!strcmp(g_config_team->algoritmo_planificion, "SJF-SD"))) {
				ordenar_cola_ready_estimacion();
			}

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



/*if((!strcmp(algoritmo_planificacion, "SJF-SD"))) {
	ciclos_de_cpu++;
	entrenador->ciclos_de_cpu++;
	sleep(retardo_ciclo_cpu);
	//log_info(team_logger, "El entrenador %d ejecutó 1 ciclo de cpu", entrenador->id);

	entrenador->instruccion_actual++;
	entrenador->estimacion_actual--;
	entrenador->ejec_anterior = 0;
}

if(!strcmp(algoritmo_planificacion, "SJF-CD")) {
	ciclos_de_cpu++;
	entrenador->ciclos_de_cpu++;
	sleep(retardo_ciclo_cpu);
	cambios_de_contexto++;
	//log_info(team_logger, "El entrenador %d ejecutó 1 ciclo de cpu", entrenador->id);
	entrenador->instruccion_actual++;
	entrenador->estimacion_actual--;
	//log_info(team_logger, "Mi estimacion actual es %f", entrenador->estimacion_actual);
	entrenador->ejec_anterior = 0;

	if(desalojo_en_ejecucion) {
		entrenador_en_ejecucion = NULL;
		pthread_mutex_lock(&lista_listos_mutex);
		list_add(lista_listos, entrenador);
		pthread_mutex_unlock(&lista_listos_mutex);
		log_info(team_logger, "El entrenador de id %d fue desalojado y paso a Ready", entrenador->id);
		log_info(team_logger_oficial, "El entrenador de id %d fue desalojado y pasó a la cola de listos", entrenador->id);

		sem_post(&orden_para_planificar);
		log_info(team_logger_oficial, "El entrenador %d pasó a exec", entrenador->id);
	}
}*/

