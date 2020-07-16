/*
 * planificadorAMedianoPlazo.c
 *
 *  Created on: 7 jul. 2020
 *      Author: utnso
 */
#include "planificadorAMedianoPlazo.h"

void planificadorMedianoPlazo() {

	char* estadosEntrenadorStrings[9] = {
			"Moverse a pokemon", "Moverse a entrenador", "Atrapar",
			"Intercambiar", "Acabar intercambio", "Recibir respuesta ok",
			"Esperar caught","Estar en deadlock", "Estar en exit"
	};

	while (finalizarProceso == 0) {

		//AGREGAR UN SIGNLA POR CADA POKEOMON LIBRE QUE LLEGA EN LA RECEPCION DE MENSAJES DE POKEMONES, MAS
		//UN SIGNAL EN EL CASO DE FAIL. SIEMPRE Y CUANDO TENGAMOS POKEMONES LIBRES DE ESA ESPECIE.
		//AGREGAR EL SIGNAL AL RECIBIR EL POKEMON A UBICAR EN EL MAPA
		//VERIFICAR EL MAPA DE POKEMONES LIBRES, SI HAY COMIENZA A EJECUTAR

		//SEMAFORO QUE DEBEMOS MANDAR CADA VEZ QUE APARECE UN POKEMON EN EL MAPA, LA CANTIDAD DE POKEMONES
		//A AGREGAR SERAN LA CANTIDAD DE SIGNALS.
		sem_wait(&sem_hay_pokemones_mapa);

		//POR CONFIGURACION QUE ESTE ACTIVADO SIEMPRE
		sem_wait(&sem_planificador_mplazo);



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
			if (entrenadorAux->estado_entrenador == ACABO_INTERCAMBIO ||
					entrenadorAux->estado_entrenador == RECIBIO_RESPUESTA_OK) {

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

			finalizarProceso = 1;
			sem_post(&sem_planificador_cplazoReady);

		}
		sem_post(&sem_cola_exit);

		//MATAR A SI MISMO, verifico que finalizarProceso no sea 0. Si es, salteo la lógica para que salga
		//del while.

		if (finalizarProceso == 0) {

			//NO OLVIDARSE DE CORTAR LAS SUSCRIPCIONES A LAS COLAS DEL BROKER.
			//VERIFICAR CUANDO LLEGA EL CAUGTH SI YA ESTOY EN CONDICIONES DE CORTAR LA SUSCRIPCION
			//ESO QUIERE DECIR (LOS OBTENIDOS SEAN LOS MISMO QUE LOS GLOBALES), IDEM LOCALIZED Y APPEARED.

			//IR SACANDO DE UN ENTRENADOR DE LA COLA BLOCKED
			sem_wait(&sem_cola_blocked);
			cantidadElementosCola = queue_size(colaBlockedEntrenadores);

			char encontreUnoAPasar = 0;

			for (int i = 0; i < cantidadElementosCola; i++) {

				t_entrenador* entrenadorAux = (t_entrenador*) queue_pop(
						colaBlockedEntrenadores);

				if (encontreUnoAPasar == 1) {
					queue_push(colaBlockedEntrenadores, entrenadorAux);
				} else {

					//1.VERIFICAR SI ESTA EN DEADLOCK: SI OCURRE ESTO, SE DESCARTA Y SE COLACA NUEVAMENTE EN LA COLA
					//2. VERIFICAR SI ESTA EN ESPERANDO_RESPUESTA:  SI OCURRE ESTO, SE DESCARTA Y SE COLACA NUEVAMENTE EN LA COLA
					if (entrenadorAux->estado_entrenador == DEADLOCK
							|| entrenadorAux->estado_entrenador
									== ESPERAR_CAUGHT) {
						queue_push(colaBlockedEntrenadores, entrenadorAux);
					} else {
						encontreUnoAPasar = 1;
						//3.CUALQUIER OTRO CASO, LO PONGO EN LA COLA DE READY.
						sem_wait(&sem_cola_ready);
						queue_push(colaReadyEntrenadores, entrenadorAux);

						log_info(g_logger,"Entrenador %d se movio a la cola de Ready, porque va a %s", entrenadorAux->id, estadosEntrenadorStrings[entrenadorAux->estado_entrenador]);

						sem_post(&sem_cola_ready);

					}
				}

			}
			sem_post(&sem_cola_blocked);

			//variable local que me indique si ENCONTRE UNO LISTA PARA PASAR A READY
			//Si es FALSE, ahi recien recorre la lista de NEW.

			sem_wait(&sem_cola_new);
			if (encontreUnoAPasar == 0
					&& (queue_size(colaNewEntrenadores) != 0)) {

				t_entrenador* entrenadorAux = (t_entrenador*) queue_pop(
						colaNewEntrenadores);

				queue_push(colaReadyEntrenadores, entrenadorAux);
			}
			sem_post(&sem_cola_new);

			sem_wait(&sem_cola_new);
			int cantidadEnNew = queue_size(colaNewEntrenadores);
			sem_post(&sem_cola_new);

			log_info(g_logger, "Se va a correr el algoritmo de deteccion de deadlock");


			if (encontreUnoAPasar == 0
					&& (cantidadEnNew == 0)) {

			log_info(g_logger, "Se detecto deadlock");


				////TRATAR DEADLOCK
				sem_wait(&sem_cola_blocked);
				//Buscamos alguno que esté en deadlock. Asumimos el riesgo de que no haya ninguno en deadlock y que
				//capaz encuentre a todos en esperarCaught.
				t_entrenador* entrenadorAux = buscarPrimerEntrenadorEnDeadlock();
				sem_post(&sem_cola_blocked);

				sem_wait(&(entrenadorAux->mutex_entrenador));

				//Si está en deadlock entonces lo que tenemos que hacer es cambiarle el estado para que pueda
				//moverse a un entrenador
				entrenadorAux->estado_entrenador = MOVERSE_A_ENTRENADOR;

				//pasarlo a la cola de ready
				sem_wait(&sem_cola_ready);
				queue_push(colaReadyEntrenadores, entrenadorAux);
				sem_post(&sem_cola_ready);

				//y por último avisarle al planificador a corto plazo
				sem_post(&sem_planificador_cplazoReady);

				sem_post(&(entrenadorAux->mutex_entrenador));

				sem_post(&sem_hay_pokemones_mapa);



			}else{

				log_info(g_logger, "No hay deadlock");


			}


		}



	}

	sem_wait(&sem_terminar_todo);

	logearResultadoTeam();

	liberar_variables_globales();

}


//Los semaforos los tiene afuera
t_entrenador* buscarPrimerEntrenadorEnDeadlock(){

	t_entrenador* entrenadorRetorno = NULL;
	t_entrenador* entrenadorAux = NULL;

	for(int i=0; i<queue_size(colaBlockedEntrenadores); i++){

		entrenadorAux = (t_entrenador*) queue_pop(colaBlockedEntrenadores);

		sem_wait(&(entrenadorAux->mutex_entrenador));
		if((entrenadorAux->estado_entrenador == DEADLOCK) && (entrenadorRetorno!= NULL)){

			sem_post(&(entrenadorAux->mutex_entrenador));
			entrenadorRetorno = entrenadorAux;

		}
		queue_push(colaBlockedEntrenadores, entrenadorAux);
	}
	return entrenadorRetorno;
}

void liberar_variables_globales(){

		int cantidadEntrenadoresExit = queue_size(colaExitEntrenadores);

		for(int i=0; i< cantidadEntrenadoresExit; i++){

			t_entrenador* entrenador = (t_entrenador*) queue_pop(colaExitEntrenadores);

			free(entrenador->posicion);
			liberar_lista_de_pokemones(entrenador->objetivoEntrenador);
			liberar_lista_de_pokemones(entrenador->pokemonesObtenidos);
			sem_destroy(&entrenador->mutex_entrenador);
			sem_destroy(&entrenador->sem_entrenador);
			free(entrenador);
		}



	    queue_destroy(colaReadyEntrenadores);

	    queue_destroy(colaBlockedEntrenadores);

	    queue_destroy(colaExitEntrenadores);

		liberar_lista_de_pokemones(pokemonesLibresEnElMapa);

		liberar_lista(pokemonesReservadosEnElMapa);

		liberar_lista_de_pokemones(pokemonesAtrapadosGlobal);


		//--------------SEMAFOROS LISTAS DE POKEMONES------------------------------

		sem_destroy(&sem_pokemonesGlobalesAtrapados);

		sem_destroy(&sem_pokemonesReservados);

		sem_destroy(&sem_pokemonesLibresEnElMapa);

		sem_destroy(&sem_pokemonesObjetivoGlobal);



		//---------------SEMAFOROS COLAS DE ENTRENADORES---------------------------

		sem_destroy(&sem_cola_blocked);

		sem_destroy(&sem_cola_new);

		sem_destroy(&sem_cola_ready);

		sem_destroy(&sem_cola_exit);


		//--------------SEMAFOROS PLANIFICADORES-----------------------------------

		sem_destroy(&sem_planificador_cplazoReady);

		sem_destroy(&sem_planificador_cplazoEntrenador);

		sem_destroy(&sem_planificador_mplazo);

		sem_destroy(&sem_hay_pokemones_mapa);

		sem_destroy(&sem_terminar_todo);


}

void logearResultadoTeam(){

	log_info(g_logger, "CANTIDAD CICLOS CPU TOTALES %d", ciclosCPU);

	log_info(g_logger, "CANTIDAD CAMBIOS DE CONTEXTO %d", cantidadCambiosDeContexto);

	int cantidadEntrenadoresExit = queue_size(colaExitEntrenadores);

			for(int i=0; i< cantidadEntrenadoresExit; i++){

				t_entrenador* entrenador = (t_entrenador*) queue_pop(colaExitEntrenadores);

				log_info(g_logger, "CANTIDAD CICLOS CPU ENTRENADOR %d : %d", entrenador->id, entrenador->ciclosCPU);

				queue_push(colaExitEntrenadores, entrenador);
			}


}

