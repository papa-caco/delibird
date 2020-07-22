/*
 * planificadorAMedianoPlazo.c
 *
 *  Created on: 7 jul. 2020
 *      Author: utnso
 */
#include "planificadorAMedianoPlazo.h"

void planificadorMedianoPlazo() {

	printf("SE LEVANTO HILO PLANIFICADOR");


	char* estadosEntrenadorStrings[9] = {
			"Moverse a pokemon", "Moverse a entrenador", "Atrapar",
			"Intercambiar", "Acabar intercambio", "Recibir respuesta ok",
			"Esperar caught","Estar en deadlock", "Estar en exit"
	};

	sem_wait(&sem_activacionPlanificadorMPlazo);

	while (finalizarProceso == 0) {


		//AGREGAR UN SIGNLA POR CADA POKEOMON LIBRE QUE LLEGA EN LA RECEPCION DE MENSAJES DE POKEMONES, MAS
		//UN SIGNAL EN EL CASO DE FAIL. SIEMPRE Y CUANDO TENGAMOS POKEMONES LIBRES DE ESA ESPECIE.
		//AGREGAR EL SIGNAL AL RECIBIR EL POKEMON A UBICAR EN EL MAPA
		//VERIFICAR EL MAPA DE POKEMONES LIBRES, SI HAY COMIENZA A EJECUTAR


		//SEMAFORO QUE DEBEMOS MANDAR CADA VEZ QUE APARECE UN POKEMON EN EL MAPA, LA CANTIDAD DE POKEMONES
		//A AGREGAR SERAN LA CANTIDAD DE SIGNALS.


		//POR CONFIGURACION QUE ESTE ACTIVADO SIEMPRE
		sem_wait(&sem_planificador_mplazo);
		printf("Ya esta corriendo el planificador a mediano plazo \n");


		sem_wait(&sem_cola_blocked);
		int cantidadElementosCola = queue_size(colaBlockedEntrenadores);
		printf("Ya se calculo la cantidad que hay en blocked y son %d \n", cantidadElementosCola);
		sem_post(&sem_cola_blocked);


		for (int i = 0; i < cantidadElementosCola; i++) {

			sem_wait(&sem_cola_blocked);
			printf("Estoy buscando a los entrenadores que pueden pasar a exit \n");
			t_entrenador* entrenadorAux = (t_entrenador*) queue_pop(
					colaBlockedEntrenadores);

			//VERIFICAR SI EL ENTRENADOR PUEDE PASAR A EXIT O SI TIENE DEADLOCK, O SI DEBE BUSCAR MAS POKEMONES (CAMBIAR ESTADO)





			//RECORRER TODA LA COLA DE BLOCKED, VERIFICANDO LOS QUE ESTEN EN ESTADO ACABO_INTERCAMBIAR,
			//QUE NOS QUEDARON COLGADOS DEL INTERCAMBIO, Y FIJARSE SI DEBEN PASAR A ESTADO EXIT Y A LA COLA EXIT.
			if (entrenadorAux->estado_entrenador == ACABO_INTERCAMBIO ||
					entrenadorAux->estado_entrenador == RECIBIO_RESPUESTA_OK) {
				printf("Va a verificar al entrenador %d \n", entrenadorAux->id);

				sem_wait(&(entrenadorAux->mutex_entrenador));
				printf("Va a verificar al entrenador %d \n", entrenadorAux->id);
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
			printf("######################################\n");
			printf("CERRANDO TEAM \n");
			printf("######################################\n");

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


			//Si todos quieren moverse a pokemon, entonces esperen a que haya pokemones en el mapa que
			//nos sirvan. Si se encuentra uno a pasar se asigna 1 para saltear el resto de busquedas.
			if(cantidadElementosCola != 0 && todosQuierenMoverseAPokemon(colaBlockedEntrenadores)){

				//Espero que haya pokemones
				printf("TODOS QUIEREN MOVERSE A UN POKEMON Y HAY QUE ESPERAR A QUE EN EL MAPA HAYAN POKEMONES QUE NOS SIRVAN \n");
				sem_wait(&sem_hay_pokemones_mapa);

				//Busco al primer entrenador que tiene el estado "Moverse a pokemon"
				for (int i = 0; i < cantidadElementosCola; i++){
					t_entrenador* entrenadorAux = (t_entrenador*) queue_pop(
							colaBlockedEntrenadores);

					if (encontreUnoAPasar == 1 || entrenadorAux->estado_entrenador != MOVERSE_A_POKEMON) {

						queue_push(colaBlockedEntrenadores, entrenadorAux);

					}
					else{
						encontreUnoAPasar = 1;

						sem_wait(&sem_cola_ready);
						queue_push(colaReadyEntrenadores, entrenadorAux);

						log_info(g_logger, "Entrenador %d se movio a la cola de Ready, porque va a %s", entrenadorAux->id, estadosEntrenadorStrings[entrenadorAux->estado_entrenador]);

						sem_post(&sem_cola_ready);

						sem_post(&sem_planificador_cplazoReady);
					}
				}
			}

			for (int i = 0; i < cantidadElementosCola; i++) {

				//Si entra aca quiere decir que al menos uno hay en blocked que quiere hacer algo.
				//Hay que filtrar los que estan en deadlock y los que estan esperando caught.
				//Una vex filtrados, hay que preguntar si todos quieren moverse a pokemon y si es asi esperar a
				//que hayan pokemones libres que nos sirvan.
				//AHora bien si hay al menos uno que no quiere moverse a pokemon entonces no hay que esperar nada
				// y metemos a ejecutar a ese.

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

						sem_post(&sem_planificador_cplazoReady);

					}
				}

			}
			sem_post(&sem_cola_blocked);

			//variable local que me indique si ENCONTRE UNO LISTA PARA PASAR A READY
			//Si es FALSE, ahi recien recorre la lista de NEW.

			sem_wait(&sem_cola_new);
			if (encontreUnoAPasar == 0
					&& (queue_size(colaNewEntrenadores) != 0)) {

				//Si llego aca es porque blocked esta vacio, y en new hay alguien, por lo cual lo unico que va a
				//querer hacer es moverse y primero me tengo que fijar si hay pokemones que me sirven en el mapa
				printf("TODOS ESTAN EN NEW Y HAY QUE ESPERAR A QUE EN EL MAPA HAYAN POKEMONES QUE NOS SIRVAN \n");
				sem_wait(&sem_hay_pokemones_mapa);

				t_entrenador* entrenadorAux = (t_entrenador*) queue_pop(
						colaNewEntrenadores);

				queue_push(colaReadyEntrenadores, entrenadorAux);

				sem_post(&sem_planificador_cplazoReady);

				encontreUnoAPasar =1;
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

				log_info(g_logger,"Entrenador %d se movio a la cola de Ready, porque va a %s", entrenadorAux->id, estadosEntrenadorStrings[entrenadorAux->estado_entrenador]);

				//pasarlo a la cola de ready
				sem_wait(&sem_cola_ready);
				queue_push(colaReadyEntrenadores, entrenadorAux);
				sem_post(&sem_cola_ready);

				//y por último avisarle al planificador a corto plazo
				sem_post(&sem_planificador_cplazoReady);

				sem_post(&(entrenadorAux->mutex_entrenador));

				//sem_post(&sem_hay_pokemones_mapa);



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
		if((entrenadorAux->estado_entrenador == DEADLOCK) && (entrenadorRetorno == NULL)){

			sem_post(&(entrenadorAux->mutex_entrenador));
			entrenadorRetorno = entrenadorAux;

		} else{
			queue_push(colaBlockedEntrenadores, entrenadorAux);
			sem_post(&(entrenadorAux->mutex_entrenador));
		}

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

		//printf("Ya libere los entrenadores \n");

	    queue_destroy(colaReadyEntrenadores);

	    queue_destroy(colaBlockedEntrenadores);

	    queue_destroy(colaExitEntrenadores);

	    printf("Ya libere la cola de entrenadores \n");

		if(pokemonesLibresEnElMapa->elements_count == 0){
			list_destroy(pokemonesLibresEnElMapa);
		} else{
			liberar_lista_de_pokemones(pokemonesLibresEnElMapa);
		}

		list_destroy(pokemonesReservadosEnElMapa);

		list_clean(pokemonesAtrapadosGlobal);

		list_destroy(pokemonesAtrapadosGlobal);

		list_clean(objetivoGlobalEntrenadores);

		list_destroy(objetivoGlobalEntrenadores);

		//printf("Ya libere las listas de entrenadores \n");

		liberar_lista(idCorrelativosCatch);

		list_destroy(idCorrelativosGet);

		liberar_lista(pokemonesLlegadosDelBroker);

		//printf("Ya libere las listas auxiliares que reciben mensajes \n");


		//--------------SEMAFOROS LISTAS DE POKEMONES------------------------------

		sem_destroy(&sem_pokemonesGlobalesAtrapados);

		sem_destroy(&sem_pokemonesReservados);

		sem_destroy(&sem_pokemonesLibresEnElMapa);

		sem_destroy(&sem_pokemonesObjetivoGlobal);

		//printf("Ya libere los semaforos de listas de pokes \n");

		//---------------SEMAFOROS COLAS DE ENTRENADORES---------------------------

		sem_destroy(&sem_cola_blocked);

		sem_destroy(&sem_cola_new);

		sem_destroy(&sem_cola_ready);

		sem_destroy(&sem_cola_exit);

		//printf("Ya libere los semaforos de colas \n");


		//--------------SEMAFOROS PLANIFICADORES-----------------------------------

		sem_destroy(&sem_planificador_cplazoReady);

		sem_destroy(&sem_planificador_cplazoEntrenador);

		sem_destroy(&sem_planificador_mplazo);

		sem_destroy(&sem_hay_pokemones_mapa);

		sem_destroy(&sem_activacionPlanificadorMPlazo);

		sem_destroy(&sem_terminar_todo);

		//sem_destroy(&mutex_listaPokemonesLlegadosDelBroker);

		pthread_mutex_destroy(&mutex_listaPokemonesLlegadosDelBroker);

		sem_destroy(&mutex_idCorrelativosGet);

		sem_destroy(&mutex_ciclosCPU);

		sem_destroy(&mutex_idCorrelativos);

		sem_destroy(&mutex_entrenador);

		//printf("Ya libere los semaforos varios. Finaliza team \n");

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

//Filtrando a los que estan en blocked o esperando respuesta caught, se fija si todos quieren moverse a pokemon
char todosQuierenMoverseAPokemon(t_queue* colaDeEntrenadores) {

	char valorDeRetorno = 0;
	int entrenadoresEnBlocked = queue_size(colaDeEntrenadores);
	int cantidadQueQuierenMoverse = 0;
	int cantidadDeFiltrados = 0;

	for (int i = 0; i < entrenadoresEnBlocked; i++) {

		t_entrenador* unEntrenador = queue_pop(colaDeEntrenadores);

		if (unEntrenador->estado_entrenador == DEADLOCK
				|| unEntrenador->estado_entrenador == ESPERAR_CAUGHT) {

			cantidadDeFiltrados++;

		} else if (unEntrenador->estado_entrenador == MOVERSE_A_POKEMON) {

			cantidadQueQuierenMoverse++;

		}
		queue_push(colaDeEntrenadores, unEntrenador);
	}
	if (cantidadQueQuierenMoverse != 0) {
		valorDeRetorno = (entrenadoresEnBlocked - cantidadDeFiltrados)
				== cantidadQueQuierenMoverse;
	}

	//printf("El valorDeRetorno es %d \n", valorDeRetorno);
	return valorDeRetorno;
}
