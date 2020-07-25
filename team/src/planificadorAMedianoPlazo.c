/*
 * planificadorAMedianoPlazo.c
 *
 *  Created on: 7 jul. 2020
 *      Author: utnso
 */
#include "planificadorAMedianoPlazo.h"

void planificadorMedianoPlazo() {

	//printf("SE LEVANTO HILO PLANIFICADOR");

	char* estadosEntrenadorStrings[9] = { "Moverse a pokemon",
			"Moverse a entrenador", "Atrapar", "Intercambiar",
			"Acabar intercambio", "Recibir respuesta ok", "Esperar caught",
			"Estar en deadlock", "Estar en exit" };

	sem_wait(&sem_activacionPlanificadorMPlazo);

	int valor = 0;

	while (finalizarProceso == 0) {

		//AGREGAR UN SIGNLA POR CADA POKEOMON LIBRE QUE LLEGA EN LA RECEPCION DE MENSAJES DE POKEMONES, MAS
		//UN SIGNAL EN EL CASO DE FAIL. SIEMPRE Y CUANDO TENGAMOS POKEMONES LIBRES DE ESA ESPECIE.
		//AGREGAR EL SIGNAL AL RECIBIR EL POKEMON A UBICAR EN EL MAPA
		//VERIFICAR EL MAPA DE POKEMONES LIBRES, SI HAY COMIENZA A EJECUTAR

		//SEMAFORO QUE DEBEMOS MANDAR CADA VEZ QUE APARECE UN POKEMON EN EL MAPA, LA CANTIDAD DE POKEMONES
		//A AGREGAR SERAN LA CANTIDAD DE SIGNALS.

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
			if (entrenadorAux->estado_entrenador == ACABO_INTERCAMBIO
					|| entrenadorAux->estado_entrenador
							== RECIBIO_RESPUESTA_OK) {

				sem_wait(&(entrenadorAux->mutex_entrenador));
				verificarYCambiarEstadoEntrenador(entrenadorAux);
				sem_post(&(entrenadorAux->mutex_entrenador));

				//printf("SALIO DEL VERIFICAR ESTADO CON EL ESTADO: %s \n",
						//estadosEntrenadorStrings[entrenadorAux->estado_entrenador]);

				if (entrenadorAux->estado_entrenador == EXIT) {

					//SI ESTA EN EXIT, LO MANDO A LA COLA CORRESPONDIENTE
					sem_post(&(entrenadorAux->sem_entrenador));

					sem_wait(&sem_cola_exit);
					queue_push(colaExitEntrenadores, entrenadorAux);
					sem_post(&sem_cola_exit);

				} else {
					queue_push(colaBlockedEntrenadores, entrenadorAux);
				}

			} else {
				queue_push(colaBlockedEntrenadores, entrenadorAux);
			}
			sem_post(&sem_cola_blocked);
		}

		//PREGUNTAR SI TODOS LOS ENTRENADORES ESTAN EN EXIT, ENTONCES:
		//1.AVISAR AL PLANIFICADOR DEL CORTO PLAZO
		//2. MATARME A MI MISMO

		sem_wait(&sem_cola_exit);
		if (cantidadDeEntrenadores == queue_size(colaExitEntrenadores)) {
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

			//Hay que filtrar los que estan en deadlock y los que estan esperando caught.
			//Una vex filtrados, hay que preguntar si todos quieren moverse a pokemon y si es asi esperar a
			//que hayan pokemones libres que nos sirvan.
			//Si todos quieren moverse a pokemon, entonces esperen a que haya pokemones en el mapa que
			//nos sirvan.
			//Cuando hay pokemones, hay que pasar a ready al entrenador que menos distancia tenga para ejecutar
			//Si se encuentra uno a pasar se asigna 1 para saltear el resto de busquedas.
			if (cantidadElementosCola != 0
					&& todosQuierenMoverseAPokemon(colaBlockedEntrenadores)) {

				t_entrenador* entrenadorAux;

				//Espero que haya pokemones
				sem_wait(&sem_hay_pokemones_mapa);

				//Busco al primer entrenador que tiene el estado "Moverse a pokemon"
				if (strcmp(g_config_team->algoritmo_planificion, "FIFO") == 0) {
					entrenadorAux = buscarEntrenadorMasConvenienteEnCola(
							colaBlockedEntrenadores);
					encontreUnoAPasar = 1;
				} else if (strcmp(g_config_team->algoritmo_planificion, "RR")
						== 0) {

					entrenadorAux = buscarEntrenadorMasConvenienteRR(
							colaBlockedEntrenadores);
					encontreUnoAPasar = 1;

				}

				sem_wait(&sem_cola_ready);
				queue_push(colaReadyEntrenadores, entrenadorAux);

				log_info(g_logger,
						"Entrenador %d se movio a la cola de Ready, porque va a %s",
						entrenadorAux->id,
						estadosEntrenadorStrings[entrenadorAux->estado_entrenador]);

				sem_post(&sem_cola_ready);

				sem_post(&sem_planificador_cplazoReady);

			}

			if (encontreUnoAPasar == 0) {
				for (int i = 0; i < cantidadElementosCola; i++) {

					//Si entra aca quiere decir que al menos uno hay en blocked que quiere hacer algo != MoverseAPokeon
					//Entonces no hay que esperar nada ni calcular cercanias, metemos a ejecutar a ese.

					t_entrenador* entrenadorAux = (t_entrenador*) queue_pop(
							colaBlockedEntrenadores);

					if (encontreUnoAPasar == 1) {
						queue_push(colaBlockedEntrenadores, entrenadorAux);
					} else {

						//1.VERIFICAR SI ESTA EN DEADLOCK: SI OCURRE ESTO, SE DESCARTA Y SE COLACA NUEVAMENTE EN LA COLA
						//2. VERIFICAR SI ESTA EN ESPERANDO_RESPUESTA:  SI OCURRE ESTO, SE DESCARTA Y SE COLACA NUEVAMENTE EN LA COLA
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//Aca en este if capaz habria que tambien filtrar a que no puedan ejecutar los que quieren moverseAPokemon
						if (entrenadorAux->estado_entrenador == DEADLOCK
								|| entrenadorAux->estado_entrenador
										== ESPERAR_CAUGHT
								|| entrenadorAux->estado_entrenador
										== MOVERSE_A_POKEMON) {
							queue_push(colaBlockedEntrenadores, entrenadorAux);
						} else {
							encontreUnoAPasar = 1;
							//3.CUALQUIER OTRO CASO, LO PONGO EN LA COLA DE READY.
							sem_wait(&sem_cola_ready);
							queue_push(colaReadyEntrenadores, entrenadorAux);

							log_info(g_logger,
									"Entrenador %d se movio a la cola de Ready, porque va a %s",
									entrenadorAux->id,
									estadosEntrenadorStrings[entrenadorAux->estado_entrenador]);

							sem_post(&sem_cola_ready);

							sem_post(&sem_planificador_cplazoReady);

						}
					}

				}
			}

			sem_post(&sem_cola_blocked);

			//variable local que me indique si ENCONTRE UNO LISTA PARA PASAR A READY
			//Si es FALSE, ahi recien recorre la lista de NEW.

			log_info(g_logger,
					"Se va a correr el algoritmo de deteccion de deadlock");

			if (encontreUnoAPasar == 0) {

				log_info(g_logger, "Se detecto deadlock");

				////TRATAR DEADLOCK
				sem_wait(&sem_cola_blocked);
				//Buscamos alguno que esté en deadlock. Asumimos el riesgo de que no haya ninguno en deadlock y que
				//capaz encuentre a todos en esperarCaught.
				t_entrenador* entrenadorAux =
						buscarPrimerEntrenadorEnDeadlock();
				sem_post(&sem_cola_blocked);

				sem_wait(&(entrenadorAux->mutex_entrenador));

				//Si está en deadlock entonces lo que tenemos que hacer es cambiarle el estado para que pueda
				//moverse a un entrenador
				entrenadorAux->estado_entrenador = MOVERSE_A_ENTRENADOR;

				log_info(g_logger,
						"Entrenador %d se movio a la cola de Ready, porque va a %s",
						entrenadorAux->id,
						estadosEntrenadorStrings[entrenadorAux->estado_entrenador]);

				//pasarlo a la cola de ready
				sem_wait(&sem_cola_ready);
				queue_push(colaReadyEntrenadores, entrenadorAux);
				sem_post(&sem_cola_ready);

				//y por último avisarle al planificador a corto plazo
				sem_post(&sem_planificador_cplazoReady);

				sem_post(&(entrenadorAux->mutex_entrenador));

				//sem_post(&sem_hay_pokemones_mapa);

			} else {

				log_info(g_logger, "No hay deadlock");

			}

		}

	}

	sem_wait(&sem_terminar_todo);

	logearResultadoTeam();

	liberar_variables_globales();

}

//Los semaforos los tiene afuera
t_entrenador* buscarPrimerEntrenadorEnDeadlock() {

	t_entrenador* entrenadorRetorno = NULL;
	t_entrenador* entrenadorAux = NULL;
	int cant_blocked_entrenadores = queue_size(colaBlockedEntrenadores);

	for (int i = 0; i < cant_blocked_entrenadores; i++) {

		entrenadorAux = (t_entrenador*) queue_pop(colaBlockedEntrenadores);

		sem_wait(&(entrenadorAux->mutex_entrenador));
		if ((entrenadorAux->estado_entrenador == DEADLOCK)
				&& (entrenadorRetorno == NULL)) {

			sem_post(&(entrenadorAux->mutex_entrenador));
			entrenadorRetorno = entrenadorAux;

		} else {
			queue_push(colaBlockedEntrenadores, entrenadorAux);
			sem_post(&(entrenadorAux->mutex_entrenador));
		}

	}
	return entrenadorRetorno;
}

void liberar_variables_globales() {

	int cantidadEntrenadoresExit = queue_size(colaExitEntrenadores);

	for (int i = 0; i < cantidadEntrenadoresExit; i++) {

		t_entrenador* entrenador = (t_entrenador*) queue_pop(
				colaExitEntrenadores);

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

	if (pokemonesLibresEnElMapa->elements_count == 0) {
		list_destroy(pokemonesLibresEnElMapa);
	} else {
		liberar_lista_de_pokemones(pokemonesLibresEnElMapa);
	}

	list_destroy(pokemonesReservadosEnElMapa);

	list_clean(pokemonesAtrapadosGlobal);

	list_destroy(pokemonesAtrapadosGlobal);

	list_clean(objetivoGlobalEntrenadores);

	list_destroy(objetivoGlobalEntrenadores);

	liberar_lista(idCorrelativosCatch);

	list_clean(idCorrelativosGet);

	list_destroy(idCorrelativosGet);

	liberar_lista(pokemonesLlegadosDelBroker);

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

	sem_destroy(&sem_activacionPlanificadorMPlazo);

	sem_destroy(&sem_terminar_todo);

//sem_destroy(&mutex_listaPokemonesLlegadosDelBroker);

	pthread_mutex_destroy(&mutex_listaPokemonesLlegadosDelBroker);

	sem_destroy(&mutex_idCorrelativosGet);

	sem_destroy(&mutex_ciclosCPU);

	sem_destroy(&mutex_idCorrelativos);

	sem_destroy(&mutex_entrenador);

	sem_destroy(&sem_esperar_caught);

	printf("######################################\n");
	printf("FINALIZA EL TEAM \n");
	printf("######################################\n");
}

void logearResultadoTeam() {

	log_info(g_logger, "CANTIDAD CICLOS CPU TOTALES %d", ciclosCPU);

	log_info(g_logger, "CANTIDAD CAMBIOS DE CONTEXTO %d",
			cantidadCambiosDeContexto);

	int cantidadEntrenadoresExit = queue_size(colaExitEntrenadores);

	for (int i = 0; i < cantidadEntrenadoresExit; i++) {

		t_entrenador* entrenador = (t_entrenador*) queue_pop(
				colaExitEntrenadores);

		log_info(g_logger, "CANTIDAD CICLOS CPU ENTRENADOR %d : %d",
				entrenador->id, entrenador->ciclosCPU);

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

	return valorDeRetorno;
}

//Esta funcion necesita que los semaforos mutex de la cola de entrenadores este afuera.
//Lo que si hay que ver es si necesita los semaforos mutex de la lista de pokemones
/*t_entrenador* buscarEntrenadorMasConvenienteEnCola(t_queue* colaEntrenadores){

 int distanciaMasCorta = 100000;
 int entrenadoresEnCola = queue_size(colaEntrenadores);
 t_entrenador* entrenadorConveniente;
 int idEntrenadorConveniente = 100;

 //Primero hago un for de la cola de entrenadores. Lo que hago es agarrar cada entrenador y calculo por cada uno
 //la distancia mas corta que tiene con los pokemones y esa distancia me la guardo.
 //Una vez que tengo la distancia mas corta, vuelvo a hacer un for dentro de otro for y calculo de nuevo para cada
 //entrenador la distancia que tiene con cada pokemon, y la comparo con la distancia mas corta. Cuando encuentro al
 //entrenador que tiene esa distancia corta con algun pokemon, entonces me lo guardo, y al resto los vuelvo a pushear
 //a la cola
 for(int i=0; i < entrenadoresEnCola; i++){
 t_entrenador* entrenadorAux = queue_pop(colaEntrenadores);

 for(int j=0; j < list_size(pokemonesLibresEnElMapa); j++){

 t_pokemon_entrenador* pokemonAux = list_get(pokemonesLibresEnElMapa, j);
 int distanciaAux = calcularDistancia(entrenadorAux->posicion, pokemonAux->posicion);

 if(distanciaAux < distanciaMasCorta){

 distanciaMasCorta = distanciaAux;

 }

 }

 queue_push(colaEntrenadores, entrenadorAux);

 }

 for(int i=0; i < entrenadoresEnCola; i++){

 t_entrenador* entrenadorAux = queue_pop(colaEntrenadores);

 for(int j=0; j < list_size(pokemonesLibresEnElMapa); j++){

 t_pokemon_entrenador* pokemonAux = list_get(pokemonesLibresEnElMapa, j);
 int distanciaAux = calcularDistancia(entrenadorAux->posicion, pokemonAux->posicion);

 if(distanciaMasCorta == distanciaAux){
 idEntrenadorConveniente = entrenadorAux->id;
 }
 }

 if(idEntrenadorConveniente == entrenadorAux->id){
 entrenadorConveniente = entrenadorAux;
 } else{
 queue_push(colaEntrenadores, entrenadorAux);
 }

 }


 return entrenadorConveniente;
 }*/

t_entrenador* buscarEntrenadorMasConvenienteEnCola(t_queue* colaEntrenadores) {

	int distanciaMasCorta = 100000;
	t_entrenador* unEntrenador;
	t_entrenador* entrenadorConveniente = NULL;
	int entrenadoresEnCola = queue_size(colaEntrenadores);

	for (int i = 0; i < entrenadoresEnCola; i++) {

		unEntrenador = queue_pop(colaEntrenadores);
		if (unEntrenador->estado_entrenador != MOVERSE_A_POKEMON) {
			queue_push(colaEntrenadores, unEntrenador);
		} else {
			t_pokemon_entrenador* pokemonCercano = buscarPokemonMasCercano(
					unEntrenador->posicion);
			puts(pokemonCercano->pokemon);
			int distanciaEntreAmbos = calcularDistancia(unEntrenador->posicion,
					pokemonCercano->posicion);

			//printf("EL ENTRENADOR %d tiene una distancia de %d",
				//	unEntrenador->id, distanciaEntreAmbos);

			if (distanciaEntreAmbos < distanciaMasCorta) {
				distanciaMasCorta = distanciaEntreAmbos;
			}

			queue_push(colaEntrenadores, unEntrenador);

		}

	}

	char yaLoEncontre = 0;

	for (int i = 0; i < entrenadoresEnCola; i++) {

		unEntrenador = queue_pop(colaEntrenadores);
		if (unEntrenador->estado_entrenador != MOVERSE_A_POKEMON) {
			queue_push(colaEntrenadores, unEntrenador);
		} else {
			t_pokemon_entrenador* pokemonCercano = buscarPokemonMasCercano(
					unEntrenador->posicion);
			int distanciaEntreAmbos = calcularDistancia(unEntrenador->posicion,
					pokemonCercano->posicion);

			if (distanciaEntreAmbos == distanciaMasCorta) {

				if (yaLoEncontre == 0) {

					entrenadorConveniente = unEntrenador;
					yaLoEncontre = 1;
				} else {
					queue_push(colaEntrenadores, unEntrenador);
				}

			} else {
				queue_push(colaEntrenadores, unEntrenador);
			}
		}

	}

	return entrenadorConveniente;
}

t_entrenador* buscarEntrenadorMasConvenienteRR(t_queue* colaEntrenadores) {

	int distanciaMasCorta = 100000;
	t_entrenador* unEntrenador;
	t_entrenador* entrenadorConveniente = NULL;
	int entrenadoresEnCola = queue_size(colaEntrenadores);
	int idElegido = 0;

	for (int i = 0; i < entrenadoresEnCola; i++) {

		unEntrenador = queue_pop(colaEntrenadores);
		if (unEntrenador->estado_entrenador != MOVERSE_A_POKEMON) {
			queue_push(colaEntrenadores, unEntrenador);
		} else {

			t_pokemon_entrenador_reservado* pokRes = buscarPokemonReservado(
					unEntrenador->id);

			if (pokRes == NULL) {
				t_pokemon_entrenador* pokemonCercano = buscarPokemonMasCercano(
						unEntrenador->posicion);
				int distanciaEntreAmbos = calcularDistancia(
						unEntrenador->posicion, pokemonCercano->posicion);

				if (distanciaEntreAmbos < distanciaMasCorta) {
					distanciaMasCorta = distanciaEntreAmbos;
					idElegido = unEntrenador->id;
				}

				queue_push(colaEntrenadores, unEntrenador);

			} else {

				int distanciaEntreAmbos = calcularDistancia(
						unEntrenador->posicion, pokRes->posicion);
				if (distanciaEntreAmbos < distanciaMasCorta) {
					distanciaMasCorta = distanciaEntreAmbos;
					idElegido = unEntrenador->id;
				}
				queue_push(colaEntrenadores, unEntrenador);
			}
		}

	}

	for (int i = 0; i < entrenadoresEnCola; i++) {

		unEntrenador = queue_pop(colaEntrenadores);
		if (unEntrenador->estado_entrenador != MOVERSE_A_POKEMON) {
			queue_push(colaEntrenadores, unEntrenador);
		} else {
			if (unEntrenador->id == idElegido) {
				entrenadorConveniente = unEntrenador;
			} else {
				queue_push(colaEntrenadores, unEntrenador);
			}
		}

	}

	return entrenadorConveniente;
}
