/*
 * planificadorAMedianoPlazo.c
 *
 *  Created on: 7 jul. 2020
 *      Author: utnso
 */
#include "planificadorAMedianoPlazo.h"

void planificadorMedianoPlazo() {

	//printf("SE LEVANTO HILO PLANIFICADOR");

	char* estadosEntrenadorStrings[10] = { "MOVERSE A POKEMON",
			"MOVERSE A ENTRENADOR", "ATRAPAR", "INTERCAMBIAR",
			"ACABAR INTERCAMBIO", "RECIBIR RESPUESTA OK", "ESPERAR CAUGHT",
			"ESPERAR DEADLOCK", "ESTAR EN EXIT", "SEGUIR MOVIENDOSE" };

	sem_wait(&sem_activacionPlanificadorMPlazo);

	int valor = 0;
	int hacerSleep = 1;

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

		/*if(hacerSleep == 1){
		 hacerSleep = 0;
		 sleep(1);
		 }*/

		if (finalizarProceso == 0) {

			//IR SACANDO DE UN ENTRENADOR DE LA COLA BLOCKED
			sem_wait(&sem_cola_blocked);
			cantidadElementosCola = queue_size(colaBlockedEntrenadores);
			int cantidadDeEntrenadoresAux = 0;

			for (int i = 0; i < cantidadElementosCola; i++) {
				t_entrenador* entrenadorAux = queue_pop(
						colaBlockedEntrenadores);
				if (entrenadorAux->estado_entrenador == MOVERSE_A_POKEMON) {
					cantidadDeEntrenadoresAux++;
				}
				queue_push(colaBlockedEntrenadores, entrenadorAux);
			}

			char encontreUnoAPasar = 0;

			//Hay que filtrar los que estan en deadlock y los que estan esperando caught.
			//Una vex filtrados, hay que preguntar si todos quieren moverse a pokemon y si es asi esperar a
			//que hayan pokemones libres que nos sirvan.
			//Si todos quieren moverse a pokemon, entonces esperen a que haya pokemones en el mapa que
			//nos sirvan.
			//Cuando hay pokemones, hay que pasar a ready al entrenador que menos distancia tenga para ejecutar
			//Si se encuentra uno a pasar se asigna 1 para saltear el resto de busquedas.
			if (cantidadDeEntrenadoresAux != 0
					&& todosQuierenMoverseAPokemon(colaBlockedEntrenadores) == 1
					&& todosEstanBloqueados() == 0) {

				t_entrenador* entrenadorAux;

				//Espero que haya pokemones
				sem_wait(&sem_hay_pokemones_mapa);

				//Busco al primer entrenador que tiene el estado "Moverse a pokemon"

				sem_wait(&sem_pokemonesLibresEnElMapa);
				int cantidadDePokemonesLibresAuxiliar = list_size(
						pokemonesLibresEnElMapa);
				sem_post(&sem_pokemonesLibresEnElMapa);
				//Empiezo a llenar la cola de Ready
				while (cantidadDeEntrenadoresAux != 0
						&& cantidadDePokemonesLibresAuxiliar != 0) {

					//entrenadorAux = buscarEntrenadorMasConvenienteEnCola(colaBlockedEntrenadores);

					//Esto tambien es nuevo
					entrenadorAux = buscarEntrenadorMasConvenienteEnColaRancio(
							colaBlockedEntrenadores);
					//Hasta aca

					if ((!strcmp(g_config_team->algoritmo_planificion, "SJF-CD"))
							|| (!strcmp(g_config_team->algoritmo_planificion,
									"SJF-SD"))) {
						if (!entrenadorAux->hayQueDesalojar) {
							estimar_entrenador(entrenadorAux);
//							printf("La estimacion actual del entrenador %d es %4.2f \n", entrenadorAux->id, entrenadorAux->estimacion_actual);
//							printf("La estimacion real del entrenador %d es %4.2f \n", entrenadorAux->id, entrenadorAux->estimacion_real);
//							printf("La estimacion anterior del entrenador %d es %4.2f \n", entrenadorAux->id, entrenadorAux->estimacion_anterior);
//							printf("La instruccion actual del entrenador %d es %d \n", entrenadorAux->id, entrenadorAux->instruccion_actual);
						} else {
							entrenadorAux->hayQueDesalojar = false;
						}

					}

					encontreUnoAPasar++;

					//t_pokemon_entrenador* pokemonAReservar = buscarPokemonMasCercano(entrenadorAux->posicion);

					sem_wait(&sem_cola_ready);
					queue_push(colaReadyEntrenadores, entrenadorAux);

					log_info(g_logger,
							"Entrenador %d se movio a la cola de READY, porque va a %s",
							entrenadorAux->id,
							estadosEntrenadorStrings[entrenadorAux->estado_entrenador]);

					sem_post(&sem_cola_ready);

					cantidadDeEntrenadoresAux--;
					cantidadDePokemonesLibresAuxiliar--;

				}
				//Mando un post al planificador a corto plazo por cada entrenador que pase a ready
				if (esLAPrimeraVez == 1) {
					for (int i = 0; i < encontreUnoAPasar + 1; i++) {
						sem_post(&sem_planificador_cplazoReady);
					}

				} else {
					for (int i = 0; i < encontreUnoAPasar; i++) {
						sem_post(&sem_planificador_cplazoReady);
					}
				}

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
						if (entrenadorAux->estado_entrenador == DEADLOCK
								|| entrenadorAux->estado_entrenador
										== ESPERAR_CAUGHT
								|| entrenadorAux->estado_entrenador
										== MOVERSE_A_POKEMON) {
							queue_push(colaBlockedEntrenadores, entrenadorAux);
						} else {
							encontreUnoAPasar = 1;

							if ((!strcmp(g_config_team->algoritmo_planificion,
									"SJF-CD"))
									|| (!strcmp(
											g_config_team->algoritmo_planificion,
											"SJF-SD"))) {
								if (!entrenadorAux->hayQueDesalojar) {
									estimar_entrenador(entrenadorAux);
									//printf("La estimacion actual del entrenador %d es %4.2f \n", entrenadorAux->id, entrenadorAux->estimacion_actual);
//									printf("La estimacion real del entrenador %d es %4.2f \n", entrenadorAux->id, entrenadorAux->estimacion_real);
									//printf("La estimacion anterior del entrenador %d es %4.2f \n", entrenadorAux->id, entrenadorAux->estimacion_anterior);
									//printf("La instruccion actual del entrenador %d es %d \n", entrenadorAux->id, entrenadorAux->instruccion_actual);
								} else {
									entrenadorAux->hayQueDesalojar = false;
								}
							}
							//3.CUALQUIER OTRO CASO, LO PONGO EN LA COLA DE READY.
							sem_wait(&sem_cola_ready);
							queue_push(colaReadyEntrenadores, entrenadorAux);

							log_info(g_logger,
									"Entrenador %d se movio a la cola de READY, porque va a %s",
									entrenadorAux->id,
									estadosEntrenadorStrings[entrenadorAux->estado_entrenador]);

							sem_post(&sem_cola_ready);

							if (esLAPrimeraVez == 1) {

								sem_post(&sem_planificador_mplazo);
								int* valorSem = malloc(sizeof(int));
								sem_getvalue(&sem_planificador_mplazo,
										valorSem);

							} else {
								sem_post(&sem_planificador_cplazoReady);
							}

						}
					}

				}
			}

			sem_post(&sem_cola_blocked);

			//variable local que me indique si ENCONTRE UNO LISTA PARA PASAR A READY
			//Si es FALSE, ahi recien recorre la lista de NEW.

			log_info(g_logger,
					"Se va a correr el algoritmo de deteccion de deadlock");

			sem_wait(&sem_cola_exit);
			sem_wait(&sem_cola_blocked);

			int cantidadDeEntrenadoresEnExit = queue_size(colaExitEntrenadores);
			int cantidadDeEntrenadoresEnBlocked = queue_size(
					colaBlockedEntrenadores);

			if (todosEstanBloqueados() != 0
					&& cantidadDeEntrenadoresEnBlocked
							== (cantidadDeEntrenadores
									- cantidadDeEntrenadoresEnExit)) {
				sem_post(&sem_cola_exit);
				sem_post(&sem_cola_blocked);

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
				sem_post(&sem_cola_exit);
				sem_post(&sem_cola_blocked);

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
	puts("");
	finalizar_suscripciones_team(SIGUSR1);
	sleep(1);
	printf("\n######################################\n");
	printf("TEAM DESCONECTADO DEL BROKER \n");
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

	char valorDeRetorno = 1;
	int entrenadoresEnBlocked = queue_size(colaDeEntrenadores);
	int cantidadQueQuierenMoverse = 0;
	int cantidadDeFiltrados = 0;

	for (int i = 0; i < entrenadoresEnBlocked; i++) {

		t_entrenador* unEntrenador = queue_pop(colaDeEntrenadores);

		/*if (unEntrenador->estado_entrenador == DEADLOCK
		 || unEntrenador->estado_entrenador == ESPERAR_CAUGHT) {

		 cantidadDeFiltrados++;

		 } else if (unEntrenador->estado_entrenador == MOVERSE_A_POKEMON) {

		 cantidadQueQuierenMoverse++;

		 }
		 queue_push(colaDeEntrenadores, unEntrenador);*/
		if (unEntrenador->estado_entrenador != DEADLOCK
				&& unEntrenador->estado_entrenador != ESPERAR_CAUGHT
				&& unEntrenador->estado_entrenador != MOVERSE_A_POKEMON) {

			valorDeRetorno = 0;

		}
		queue_push(colaDeEntrenadores, unEntrenador);

	}

	return valorDeRetorno;
}

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
			//puts(pokemonCercano->pokemon);
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

//Se busca al entrenador cuya distancia al primer pokemon de la lista de pokemonesLibres sea menor.
t_entrenador* buscarEntrenadorMasConvenienteEnColaRancio(
		t_queue* colaEntrenadores) {

	int distanciaMasCorta = 100000;
	t_entrenador* unEntrenador;
	int entrenadoresEnCola = queue_size(colaEntrenadores);
	t_list* entrenadoresDisponibles = list_create();

//	printf("FIltrando entrenadores disponibles \n");
	for (int i = 0; i < entrenadoresEnCola; i++) {

		unEntrenador = queue_pop(colaEntrenadores);
		if (unEntrenador->estado_entrenador == MOVERSE_A_POKEMON) {
			list_add(entrenadoresDisponibles, unEntrenador);
		} else {
			queue_push(colaEntrenadores, unEntrenador);
		}
	}

	sem_wait(&sem_pokemonesLibresEnElMapa);
	t_pokemon_entrenador* pokemonAAsignar = list_get(pokemonesLibresEnElMapa,
			0);
	sem_post(&sem_pokemonesLibresEnElMapa);

//	printf("Buscano al entrenador mas cercano \n");
	int indice = 100;
	sem_wait(&sem_pokemonesLibresEnElMapa);
	sem_post(&sem_pokemonesLibresEnElMapa);
	for (int i = 0; i < list_size(entrenadoresDisponibles); i++) {
		unEntrenador = list_get(entrenadoresDisponibles, i);

		//puts(pokemonCercano->pokemon);
		int distanciaEntreAmbos = calcularDistancia(unEntrenador->posicion,
				pokemonAAsignar->posicion);

		//printf("EL ENTRENADOR %d tiene una distancia de %d",
		//	unEntrenador->id, distanciaEntreAmbos);

		if (distanciaEntreAmbos < distanciaMasCorta) {
			distanciaMasCorta = distanciaEntreAmbos;
			indice = i;
		}
	}

	if (indice != 100) {
		unEntrenador = list_remove(entrenadoresDisponibles, indice);
	} else {	}

//	printf("Devolviendolos a blocked \n");

	while (list_size(entrenadoresDisponibles) != 0) {
		t_entrenador* otroEntrenador = list_remove(entrenadoresDisponibles, 0);
		queue_push(colaEntrenadores, otroEntrenador);
	}
	list_destroy(entrenadoresDisponibles);

//	printf("Moviendo pokemon a reservados \n");

	t_pokemon_entrenador_reservado* pokemonReservado = moverPokemonAReservados(
			pokemonAAsignar, unEntrenador->id);

//	printf("Por devolver al entrenador \n");

	return unEntrenador;
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

char todosEstanBloqueados() {
	char valorDeRetorno = 1;

	int cantidadEnBlocked = queue_size(colaBlockedEntrenadores);

	for (int i = 0; i < cantidadEnBlocked; i++) {
		t_entrenador* unEntrenador = queue_pop(colaBlockedEntrenadores);
		if (unEntrenador->estado_entrenador != DEADLOCK) {
			valorDeRetorno = 0;
		}
		queue_push(colaBlockedEntrenadores, unEntrenador);
	}

	return valorDeRetorno;
}

