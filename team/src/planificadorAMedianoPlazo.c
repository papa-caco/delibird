/*
 * planificadorAMedianoPlazo.c
 *
 *  Created on: 7 jul. 2020
 *      Author: utnso
 */
#include "planificadorAMedianoPlazo.h"

void planificadorMedianoPlazo() {
	while (1) {

		//AGREGAR UN SIGNLA POR CADA POKEOMON LIBRE QUE LLEGA EN LA RECEPCION DE MENSAJES DE POKEMONES, MAS
		//UN SIGNAL EN EL CASO DE FAIL. SIEMPRE Y CUANDO TENGAMOS POKEMONES LIBRES DE ESA ESPECIE.
		//AGREGAR EL SIGNAL AL RECIBIR EL POKEMON A UBICAR EN EL MAPA
		//VERIFICAR EL MAPA DE POKEMONES LIBRES, SI HAY COMIENZA A EJECUTAR

		//SEMAFORO QUE DEBEMOS MANDAR CADA VEZ QUE APARECE UN POKEMON EN EL MAPA, LA CANTIDAD DE POKEMONES
		//A AGREGAR SERAN LA CANTIDAD DE SIGNALS.
		sem_wait(&sem_hay_pokemones_mapa);

		//POR CONFIGURACION QUE ESTE ACTIVADO SIEMPRE
		sem_wait(&sem_planificador_mplazo);

		char encontreUnoAPasar = 0;

		sem_wait(&sem_cola_blocked);
		int cantidadElementosCola = queue_size(colaBlockedEntrenadores);
		sem_post(&sem_cola_blocked);

		for (int i = 0; i < cantidadElementosCola; i++) {

			sem_wait(&sem_cola_blocked);
			t_entrenador* entrenadorAux = (t_entrenador*) queue_pop(
					colaBlockedEntrenadores);

			//VERIFICAR SI EL ENTRENADOR PUEDE PASAR A EXIT O SI TIENE DEADLOCK, O SI DEBE BUSCAR MAS POKEMONES (CAMBIAR ESTADO)





			//RECORRER TODA LA COLA DE BLOCKED, VERIFICANDO LOS QUE ESTEN EN ESTADO ACABO_INTERCAMBIAR,
			//QUE NOS QUEDARON COLGADOS DEL INTERCAMBIO, Y FIJARSE SI DEBEN PASAR A ESTADO EXIT Y A LA COLA EXIT.
			if (entrenadorAux->estado_entrenador == ACABO_INTERCAMBIO || entrenadorAux->estado_entrenador == RECIBIO_RESPUESTA_OK) {

				sem_wait(&(entrenadorAux->mutex_entrenador));
				verificarYCambiarEstadoEntrenador(entrenadorAux);
				sem_post(&(entrenadorAux->mutex_entrenador));

				if (entrenadorAux->estado_entrenador == EXIT) {

					//SI ESTA EN EXIT, LO MANDO A LA COLA CORRESPONDIENTE
					sem_post(&(entrenadorAux->sem_entrenador));

					sem_wait(&sem_cola_exit);
					queue_push(colaExitEntrenadores, entrenadorAux);
					sem_post(&sem_cola_exit);

				} else{
					queue_push(colaBlockedEntrenadores, entrenadorAux);
				}

			}else
			{
				queue_push(colaBlockedEntrenadores, entrenadorAux);
			}
			sem_post(&sem_cola_blocked);
		}


		//PREGUNTAR SI TODOS LOS ENTRENADORES ESTAN EN EXIT, ENTONCES:
		//1.AVISAR AL PLANIFICADOR DEL CORTO PLAZO
		//2. MATARME A MI MISMO

		sem_wait(&sem_cola_exit);
		if(cantidadDeEntrenadores == queue_size(colaExitEntrenadores)){

			sem_post(&sem_planificador_cplazoReady);

			//MATAR A SI MISMO
		}
		sem_post(&sem_cola_exit);

		//NO OLVIDARSE DE CORTAR LAS SUSCRIPCIONES A LAS COLAS DEL BROKER.
		//VERIFICAR CUANDO LLEGA EL CAUGTH SI YA ESTOY EN CONDICIONES DE CORTAR LA SUSCRIPCION
		//ESO QUIERE DECIR (LOS OBTENIDOS SEAN LOS MISMO QUE LOS GLOBALES), IDEM LOCALIZED Y APPEARED.


		//IR SACANDO DE UN ENTRENADOR DE LA COLA BLOCKED
		sem_wait(&sem_cola_blocked);
		cantidadElementosCola = queue_size(colaBlockedEntrenadores);

		for (int i = 0; i < cantidadElementosCola; i++) {

		t_entrenador* entrenadorAux = (t_entrenador*) queue_pop(colaBlockedEntrenadores);

		if(encontreUnoAPasar == 1){
			queue_push(colaBlockedEntrenadores, entrenadorAux);
		}else{

			//1.VERIFICAR SI ESTA EN DEADLOCK: SI OCURRE ESTO, SE DESCARTA Y SE COLACA NUEVAMENTE EN LA COLA
			//2. VERIFICAR SI ESTA EN ESPERANDO_RESPUESTA:  SI OCURRE ESTO, SE DESCARTA Y SE COLACA NUEVAMENTE EN LA COLA
			if(entrenadorAux->estado_entrenador == DEADLOCK || entrenadorAux->estado_entrenador == ESPERAR_CAUGHT){
						queue_push(colaBlockedEntrenadores, entrenadorAux);
					}else {
						encontreUnoAPasar = 1;
						//3.CUALQUIER OTRO CASO, LO PONGO EN LA COLA DE READY.
						sem_wait(&sem_cola_ready);
						queue_push(colaReadyEntrenadores, entrenadorAux);
						sem_post(&sem_cola_ready);
					}
		}



		}
		sem_post(&sem_cola_blocked);


		//variable local que me indique si ENCONTRE UNO LISTA PARA PASAR A READY
		//Si es FALSE, ahi recien recorre la lista de NEW.

		sem_wait(&sem_cola_new);
		if(encontreUnoAPasar == 0 && (queue_size(colaNewEntrenadores)!= 0)){

			t_entrenador* entrenadorAux = (t_entrenador*) queue_pop(colaNewEntrenadores);

			queue_push(colaReadyEntrenadores, entrenadorAux);
		}
		sem_post(&sem_cola_new);


		sem_wait(&sem_cola_new);
		if(encontreUnoAPasar == 0 && (queue_size(colaNewEntrenadores)== 0)){

					////TRATAR DEADLOCK
		}
		sem_post(&sem_cola_new);

	}
}
