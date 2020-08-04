/*
 * entrenador.c
 *
 *  Created on: 4 jun. 2020
 *      Author: utnso
 */

#include "entrenador.h"

//------------------------------FUNCIONES BÁSICAS DEL ENTRENADOR-------------------------------------------

void comportamiento_entrenador(t_entrenador* entrenador) {

	int distancia;
	t_pokemon_entrenador* pokemon;
	t_pokemon_entrenador_reservado* pokemonReservado;
	t_posicion_entrenador* posicionEntrenadorAMoverse;
	t_entrenador* entrenador2;
	int quantum = g_config_team->quantum;
	char* planificadorAlgoritmo = g_config_team->algoritmo_planificion;
	//printf("PLANIFICADOR -%s- \n", planificadorAlgoritmo);
	char* fifo = "FIFO";
	char* rr = "RR";

	//La condición para cortar el hilo será que el entrenador esté en EXIT, y para que la función/hilo termine
	//va a tener que salir del while y finalizar la ejecución la función.
	while (entrenador->estado_entrenador != EXIT) {

		sem_wait(&(entrenador->sem_entrenador));

		switch (entrenador->estado_entrenador) {
		case MOVERSE_A_POKEMON:
			if (strcmp(planificadorAlgoritmo,fifo)== 0 || strcmp(planificadorAlgoritmo,"SJF-SD") == 0
					|| strcmp(planificadorAlgoritmo,"SJF-CD") == 0) {


				t_pokemon_entrenador_reservado* pokemonReservado = buscarPokemonReservado(entrenador->id);

				distancia = calcularDistancia(entrenador->posicion,
						pokemonReservado->posicion);

				for (int i = 0; i < distancia; i++) {
					 if(strcmp(planificadorAlgoritmo,"SJF-CD") == 0){
						 if(entrenador->hayQueDesalojar){
							 break;
						 }
					 }

					moverEntrenador(entrenador, pokemonReservado->posicion);

				}

				if(strcmp(planificadorAlgoritmo,"SJF-CD") == 0){
					distancia = calcularDistancia(entrenador->posicion,
							pokemonReservado->posicion);

					if (distancia == 0) {
						entrenador->estado_entrenador = ATRAPAR;
					} else{
						entrenador->estado_entrenador = SEGUIR_MOVIENDOSE;
					}

				}else{
					entrenador->estado_entrenador = ATRAPAR;
				}





				sem_post(&(sem_planificador_cplazoEntrenador));

			} else if (strcmp(planificadorAlgoritmo,rr)== 0) {
				////INICIO ROUND ROBIN
				pokemonReservado =
						buscarPokemonReservado(entrenador->id);

				distancia = calcularDistancia(entrenador->posicion,
						pokemonReservado->posicion);

				for (int i = 0; i < distancia; i++) {
					if (entrenador->quantumPorEjecutar == 0) {
						continue;
					}
					moverEntrenador(entrenador, pokemonReservado->posicion);

				}
				distancia = calcularDistancia(entrenador->posicion, pokemonReservado->posicion);

				if (distancia == 0) {

					entrenador->estado_entrenador = ATRAPAR;

				}else {

					entrenador->estado_entrenador = SEGUIR_MOVIENDOSE;
				}
				entrenador->quantumPorEjecutar = quantum;

				sem_post(&(sem_planificador_cplazoEntrenador));

			}

			break;

		case SEGUIR_MOVIENDOSE:

			if (strcmp(planificadorAlgoritmo, rr) == 0) {
				pokemonReservado = buscarPokemonReservado(entrenador->id);

				distancia = calcularDistancia(entrenador->posicion,
						pokemonReservado->posicion);

				for (int i = 0; i < distancia; i++) {
					if (entrenador->quantumPorEjecutar == 0) {
						continue;
					}
					moverEntrenador(entrenador, pokemonReservado->posicion);

				}
				distancia = calcularDistancia(entrenador->posicion,
						pokemonReservado->posicion);

				if (distancia == 0) {

					entrenador->estado_entrenador = ATRAPAR;

				}

				entrenador->quantumPorEjecutar = quantum;

				sem_post(&(sem_planificador_cplazoEntrenador));

			} else {

				pokemonReservado = buscarPokemonReservado(entrenador->id);

				distancia = calcularDistancia(entrenador->posicion,
						pokemonReservado->posicion);

				for (int i = 0; i < distancia; i++) {
					if (entrenador->hayQueDesalojar) {
						break;
					}

					moverEntrenador(entrenador, pokemonReservado->posicion);

				}
				distancia = calcularDistancia(entrenador->posicion,
						pokemonReservado->posicion);

				if (distancia == 0) {

					entrenador->estado_entrenador = ATRAPAR;

				}

				sem_post(&(sem_planificador_cplazoEntrenador));

			}




		break;
		case MOVERSE_A_ENTRENADOR:

			if (strcmp(planificadorAlgoritmo,fifo)== 0 || strcmp(planificadorAlgoritmo,"SJF-SD") == 0
					|| strcmp(planificadorAlgoritmo,"SJF-CD") == 0)  {

				//RECORDAR que el entrenador que se está moviendo ahora, debería de dejar de estar en la cola de blocked
				//y estar en exit.
				posicionEntrenadorAMoverse = buscarEntrenadorAMoverse(
						entrenador);

				distancia = calcularDistancia(entrenador->posicion,
						posicionEntrenadorAMoverse);

				for (int i = 0; i < distancia; i++) {

					moverEntrenador(entrenador, posicionEntrenadorAMoverse);

				}

				entrenador->estado_entrenador = INTERCAMBIAR;

				sem_post(&(sem_planificador_cplazoEntrenador));
			} else if (strcmp(planificadorAlgoritmo,rr)== 0) {

				//RECORDAR que el entrenador que se está moviendo ahora, debería de dejar de estar en la cola de blocked
				//y estar en exit.
				posicionEntrenadorAMoverse = buscarEntrenadorAMoverse(
						entrenador);

				distancia = calcularDistancia(entrenador->posicion,
						posicionEntrenadorAMoverse);

				for (int i = 0; i < distancia; i++) {
					if (entrenador->quantumPorEjecutar == 0) {
						continue;
					}
					moverEntrenador(entrenador, posicionEntrenadorAMoverse);
				}

				distancia = calcularDistancia(entrenador->posicion,
						posicionEntrenadorAMoverse);

				if (distancia == 0) {

					entrenador->estado_entrenador = INTERCAMBIAR;

				}

				entrenador->quantumPorEjecutar = quantum;

				sem_post(&(sem_planificador_cplazoEntrenador));


			break;
		case ATRAPAR:
			pokemonReservado = buscarPokemonReservado(entrenador->id);

			int resultadoEnvioMensaje = intentarAtraparPokemon(entrenador,
					pokemonReservado);

			//log_trace(g_logger,"Borrar-->>ESTOY EN ENTRENADOR DP DE INTENTAR ATRAPAR|id_msj_catch recibido:%d",resultadoEnvioMensaje); //TODO Borrar
			sleep(g_config_team->retardo_ciclo_cpu);

			ciclosCPU++;

			entrenador->ciclosCPU++;

			if ((!strcmp(g_config_team->algoritmo_planificion, "SJF-CD"))
						|| (!strcmp(g_config_team->algoritmo_planificion,
								"SJF-SD"))) {

					entrenador->instruccion_actual++;
					entrenador->estimacion_actual--;
					entrenador->ejec_anterior = 0;

				}

			if (resultadoEnvioMensaje == -1) {
				sem_wait(&entrenador->mutex_entrenador);
				entrenador->estado_entrenador = RECIBIO_RESPUESTA_OK;
				sem_post(&entrenador->mutex_entrenador);
			} else {
				sem_wait(&entrenador->mutex_entrenador);
				sem_wait(&sem_esperar_caught);
				sem_post(&entrenador->mutex_entrenador);
			}

			sem_wait(&entrenador->mutex_entrenador);
			if (entrenador->estado_entrenador == RECIBIO_RESPUESTA_OK && resultadoEnvioMensaje ==-1) {

				//printf("ENTRO AL IF DEL RESPUESTA POR DEFAULT \n");

				t_pokemon_entrenador* pokemonAAgregarConvertido = malloc(
						sizeof(t_pokemon_entrenador));
				pokemonAAgregarConvertido->cantidad =
						pokemonReservado->cantidad;
				pokemonAAgregarConvertido->pokemon = malloc(
						strlen(pokemonReservado->pokemon) + 1);
				memcpy(pokemonAAgregarConvertido->pokemon,
						pokemonReservado->pokemon,
						strlen(pokemonReservado->pokemon) + 1);
				pokemonAAgregarConvertido->posicion = malloc(
						sizeof(t_posicion_entrenador));
				pokemonAAgregarConvertido->posicion->pos_x =
						pokemonReservado->posicion->pos_x;
				pokemonAAgregarConvertido->posicion->pos_y =
						pokemonReservado->posicion->pos_y;

				//Borro de la lista al pokemon reservado
				sem_wait(&(sem_pokemonesReservados));
				int indice;
				for (int i = 0; i < list_size(pokemonesReservadosEnElMapa);
						i++) {

					t_pokemon_entrenador_reservado* aux =
							((t_pokemon_entrenador_reservado*) list_get(
									pokemonesReservadosEnElMapa, i));

					if (aux == pokemonReservado) {
						indice = i;
					}
				}
				pokemonReservado = list_remove(pokemonesReservadosEnElMapa,
						indice);
				sem_post(&(sem_pokemonesReservados));
				//-->> Les repetí la línea del log porque abajo le hacen free  al Pokemon y se imprimía mal
				log_info(g_logger,
						"Entrenador %d intenta atrapar al pokemon %s, en la posicion (%d,%d)",
						entrenador->id, pokemonReservado->pokemon,
						entrenador->posicion->pos_x,
						entrenador->posicion->pos_y);
				free(pokemonReservado->posicion);
				free(pokemonReservado->pokemon);
				free(pokemonReservado);

				//Agrego el Poke
				agregarPokemon(entrenador, pokemonAAgregarConvertido);

				//Muevo el pokemon a la lista global de atrapados
				sem_wait(&(sem_pokemonesGlobalesAtrapados));
				agregarPokemonAGlobalesAtrapados(pokemonAAgregarConvertido);
				sem_post(&(sem_pokemonesGlobalesAtrapados));

			}
			sem_post(&entrenador->mutex_entrenador);

			if (entrenador->estado_entrenador != RECIBIO_RESPUESTA_OK) {
				log_info(g_logger,
						"Entrenador %d intenta atrapar al pokemon %s, en la posicion (%d,%d)",
						entrenador->id, pokemonReservado->pokemon,
						entrenador->posicion->pos_x,
						entrenador->posicion->pos_y);
			} // -->> Acá le dejé el log con la condición del estado entrenador
			sem_post(&(sem_planificador_cplazoEntrenador));

			break;

		case INTERCAMBIAR:

			entrenador2 = buscarEntrenadorDelIntercambio(entrenador);


			intercambiarNormalPokemon(entrenador, entrenador2);

			entrenador->estado_entrenador = ACABO_INTERCAMBIO;

			entrenador2->estado_entrenador = ACABO_INTERCAMBIO;

			log_info(g_logger,
					"Los entrenadores %d y %d hicieron un intercambio",
					entrenador->id, entrenador2->id);

			sem_post(&(sem_planificador_cplazoEntrenador));

			break;


		case -1:
			pthread_exit(NULL);
		}

		//FALTA DETERMINAR EL CASO DEL FINALIZADO DEL ENTRENADOR

		}
	}
}



t_pokemon_entrenador* buscarPokemonMasCercano(
		t_posicion_entrenador* posicion_Entrenador) {

	t_pokemon_entrenador* pokemonMasCercano;
	int distanciaMasCercana = 100000;
	int distanciaAux = 0;

	sem_wait(&sem_pokemonesLibresEnElMapa);

	for (int j = 0; j < list_size(pokemonesLibresEnElMapa); j++) {

		t_pokemon_entrenador* pokLibreAux = list_get(pokemonesLibresEnElMapa,
				j);

		if (necesitoPokemon(pokLibreAux->pokemon)) {

			distanciaAux = calcularDistancia(posicion_Entrenador,
					pokLibreAux->posicion);

			if (distanciaAux < distanciaMasCercana) {
				distanciaMasCercana = distanciaAux;
				pokemonMasCercano = pokLibreAux;
			}

		}
	}

	sem_post(&sem_pokemonesLibresEnElMapa);

	return pokemonMasCercano;

}

t_pokemon_entrenador* buscarPokemonMasCercanoRR(
		t_posicion_entrenador* posicion_Entrenador) {

	t_pokemon_entrenador* pokemonMasCercano;
	int distanciaMasCercana = 100000;
	int distanciaAux = 0;

	sem_wait(&sem_pokemonesReservados);

	for (int j = 0; j < list_size(pokemonesReservadosEnElMapa); j++) {

		t_pokemon_entrenador* pokLibreAux = list_get(pokemonesReservadosEnElMapa,
				j);

		if (necesitoPokemon(pokLibreAux->pokemon)) {

			distanciaAux = calcularDistancia(posicion_Entrenador,
					pokLibreAux->posicion);

			if (distanciaAux < distanciaMasCercana) {
				distanciaMasCercana = distanciaAux;
				pokemonMasCercano = pokLibreAux;
			}

		}
	}

	sem_post(&sem_pokemonesReservados);

	return pokemonMasCercano;

}

t_entrenador* buscarEntrenadorDelIntercambio(t_entrenador* entrenador) {

	t_entrenador* entrenadorMasCercano;
	t_entrenador* entrenadorAux;
	int distanciaAux = 0;

	sem_wait(&(sem_cola_blocked));

	for (int i = 0; i < queue_size(colaBlockedEntrenadores); i++) {

		entrenadorAux = (t_entrenador*) queue_pop(colaBlockedEntrenadores);

		distanciaAux = calcularDistancia(entrenador->posicion,
				entrenadorAux->posicion);

		if (distanciaAux == 0 && entrenadorAux->estado_entrenador == DEADLOCK
				&& puedoIntercambiar(entrenador, entrenadorAux)) {

			entrenadorMasCercano = entrenadorAux;

		}

		queue_push(colaBlockedEntrenadores, entrenadorAux);

	}

	sem_post(&(sem_cola_blocked));

	return entrenadorMasCercano;

}

t_posicion_entrenador* buscarEntrenadorAMoverse(t_entrenador* entrenador) {

	t_posicion_entrenador* posicionAMoverse = NULL;
	t_entrenador* entrenadorAux;
	char yaEncontre = 0;

	sem_wait(&(sem_cola_blocked));

	for (int i = 0; i < queue_size(colaBlockedEntrenadores); i++) {

		entrenadorAux = (t_entrenador*) queue_pop(colaBlockedEntrenadores);

		if (entrenadorAux->estado_entrenador == DEADLOCK
				&& puedoIntercambiar(entrenador, entrenadorAux) && yaEncontre == 0) {
			yaEncontre=1;
			posicionAMoverse = entrenadorAux->posicion;

		}

		queue_push(colaBlockedEntrenadores, entrenadorAux);

	}

	sem_post(&(sem_cola_blocked));
	//}

	//sem_post(&(sem_cola_blocked));

	return posicionAMoverse;

}

t_pokemon_entrenador_reservado* moverPokemonAReservados(t_pokemon_entrenador* pokemonAMover, int idReservador) {

	t_pokemon_entrenador* pokemonAux;
	t_pokemon_entrenador_reservado* pokemonAAgregar = malloc(sizeof(t_pokemon_entrenador_reservado));
	pokemonAAgregar->cantidad = 1;
	pokemonAAgregar->pokemon = malloc(strlen(pokemonAMover->pokemon) + 1);
	memcpy(pokemonAAgregar->pokemon, pokemonAMover->pokemon,strlen(pokemonAMover->pokemon) + 1);
	pokemonAAgregar->posicion = malloc(sizeof(t_posicion_entrenador));
	pokemonAAgregar->posicion->pos_x = pokemonAMover->posicion->pos_x;
	pokemonAAgregar->posicion->pos_y = pokemonAMover->posicion->pos_y;
	pokemonAAgregar->id_entrenadorReserva = idReservador;
	sem_wait(&sem_pokemonesReservados);
	list_add(pokemonesReservadosEnElMapa, pokemonAAgregar);
	sem_post(&sem_pokemonesReservados);
	eliminar_pokemon_libre_mapa(pokemonAMover->orden);
	return pokemonAAgregar;
}

int cant_pokemones_reservados_entrenador(int id_entrenador)
{
	bool mismo_id_entrenador(void *poke){
		t_pokemon_entrenador_reservado *reservado = (t_pokemon_entrenador_reservado*) poke;
		return reservado->id_entrenadorReserva == id_entrenador;
	}
	int cant_reservados_entrenador = 0;
	if (pokemonesReservadosEnElMapa->elements_count > 0) {
		t_list *reservados_entrenador = list_filter(pokemonesReservadosEnElMapa, mismo_id_entrenador);
		cant_reservados_entrenador = reservados_entrenador->elements_count;
		list_destroy(reservados_entrenador);
	}
	return cant_reservados_entrenador;
}

bool admite_reservar_pokemon(t_entrenador *entrenador)
{
	int cant_objetivos = entrenador->objetivoEntrenador->elements_count;
	int cant_obtenidos = entrenador->pokemonesObtenidos->elements_count;
	int cant_reservados = cant_pokemones_reservados_entrenador(entrenador->id);
	return  cant_objetivos > (cant_reservados + cant_obtenidos);
}

void eliminar_pokemon_libre_mapa(int orden_pokemon)
{
	bool mismo_orden(void *poke) {
		t_pokemon_entrenador *pokemon = (t_pokemon_entrenador*) poke;
		return pokemon->orden == orden_pokemon;
	}
	sem_wait(&sem_pokemonesLibresEnElMapa);
	t_pokemon_entrenador *pokemon_a_eliminar = list_remove_by_condition(pokemonesLibresEnElMapa,mismo_orden);
	free(pokemon_a_eliminar->pokemon);
	free(pokemon_a_eliminar->posicion);
	free(pokemon_a_eliminar);
	sem_post(&sem_pokemonesLibresEnElMapa);
}

/////MOVER ENTRENADOR///////////////////

void moverEntrenador(t_entrenador* entrenador,
		t_posicion_entrenador* posicionAMoverse) {

	//AGREGAR SEMAFORO MUTEX DEL ENTRENADOR EN PARTICULAR  (ADENTRO O AFUERA?????)

	if (entrenador->posicion->pos_x != posicionAMoverse->pos_x) {
		if (entrenador->posicion->pos_x > posicionAMoverse->pos_x) {
			entrenador->posicion->pos_x -= 1;
		} else if (entrenador->posicion->pos_x < posicionAMoverse->pos_x) {
			entrenador->posicion->pos_x += 1;
		}
	} else {
		if (entrenador->posicion->pos_y > posicionAMoverse->pos_y) {
			entrenador->posicion->pos_y -= 1;
		} else if (entrenador->posicion->pos_y < posicionAMoverse->pos_y) {
			entrenador->posicion->pos_y += 1;
		}
	}

	sleep(g_config_team->retardo_ciclo_cpu);
	sem_wait(&mutex_ciclosCPU);
	ciclosCPU++;
	sem_post(&mutex_ciclosCPU);

	entrenador->ciclosCPU++;

	if ((!strcmp(g_config_team->algoritmo_planificion, "SJF-CD"))
						|| (!strcmp(g_config_team->algoritmo_planificion, "SJF-SD"))) {

		entrenador->instruccion_actual++;
		entrenador->estimacion_actual--;
		entrenador->ejec_anterior = 0;


		sem_wait(&sem_cola_ready);
		int cantidadEnReadyActual = queue_size(colaReadyEntrenadores);
		if(entroUnoAReady < cantidadEnReadyActual){

			for(int i = 0; i < cantidadEnReadyActual; i++){
				t_entrenador* otroEntrenador = queue_pop(colaReadyEntrenadores);
				if(entrenador->estimacion_actual > otroEntrenador->estimacion_real){
					entrenador->hayQueDesalojar = true;
				}
				queue_push(colaReadyEntrenadores, otroEntrenador);
			}

		}
		sem_post(&sem_cola_ready);

	}



	entrenador->quantumPorEjecutar--;

	log_info(g_logger, "Entrenador %d se movio a la posicion (%d,%d) \n",
			entrenador->id, entrenador->posicion->pos_x,
			entrenador->posicion->pos_y);

}

int calcularDistancia(t_posicion_entrenador* posicionActual,
		t_posicion_entrenador* posicionAMoverse) {
	int distanciaTotal = 0;
	if (posicionActual->pos_x > posicionAMoverse->pos_x) {
		distanciaTotal += posicionActual->pos_x - posicionAMoverse->pos_x;
	} else {
		distanciaTotal += posicionAMoverse->pos_x - posicionActual->pos_x;
	}

	if (posicionActual->pos_y > posicionAMoverse->pos_y) {
		distanciaTotal += posicionActual->pos_y - posicionAMoverse->pos_y;
	} else {
		distanciaTotal += posicionAMoverse->pos_y - posicionActual->pos_y;
	}

	return distanciaTotal;

}

//////ATRAPAR UN POKEMON//////////////////

int intentarAtraparPokemon(t_entrenador* entrenador,
		t_pokemon_entrenador_reservado* pokemon) {

	int thread_status = enviar_catch_pokemon_broker(entrenador->posicion->pos_x,
				entrenador->posicion->pos_y, pokemon->pokemon, g_logger,
				entrenador->id);
	sem_wait(&mutex_catch);
	int prueba = rta_catch;
	return prueba;

}

//////INTERCAMBIAR UN POKEMON/////////////

void intercambiarIdealPokemon(t_entrenador* entrenador1,
		t_entrenador* entrenador2) {

	t_list* pokemonesInnecesariosDT1 = pokemonesInnecesarios(entrenador1);
	t_list* pokemonesInnecesariosDT2 = pokemonesInnecesarios(entrenador2);
	t_list* pokemonesPendientesDT1 = pokemonesPendientes(entrenador1);
	t_list* pokemonesPendientesDT2 = pokemonesPendientes(entrenador2);

	//NO HAY QUE LIBERAR ESTOS CUATRO!
	t_pokemon_entrenador* pokemonInnecesario1 = malloc(
			sizeof(t_pokemon_entrenador));
	//t_pokemon_entrenador* pokemonPendiente2 = malloc(sizeof(t_pokemon_entrenador));
	t_pokemon_entrenador* pokemonInnecesario2 = malloc(
			sizeof(t_pokemon_entrenador));
	//t_pokemon_entrenador* pokemonPendiente1 = malloc(sizeof(t_pokemon_entrenador));

	//Evalúo que le sirva al entrenador 2
	for (int i = 0; i < list_size(pokemonesInnecesariosDT1); i++) {

		t_pokemon_entrenador* pokInnecesarioAux =
				((t_pokemon_entrenador*) list_get(pokemonesInnecesariosDT1, i));
		pokemonInnecesario1->cantidad = pokInnecesarioAux->cantidad;
		pokemonInnecesario1->pokemon = pokInnecesarioAux->pokemon;

		for (int j = 0; j < list_size(pokemonesPendientesDT2); j++) {
			t_pokemon_entrenador* pokemonPendiente2 =
					((t_pokemon_entrenador*) list_get(pokemonesPendientesDT2, j));

			if (strcmp(pokemonInnecesario1->pokemon, pokemonPendiente2->pokemon)
					== 0) {

				agregarPokemon(entrenador2, pokemonInnecesario1);
				quitarPokemon(entrenador1, pokemonInnecesario1);

			}

		}
	}

	//Evalúo que le sirva al entrenador 1
	for (int i = 0; i < list_size(pokemonesInnecesariosDT2); i++) {

		t_pokemon_entrenador* pokInnecesarioAux =
				((t_pokemon_entrenador*) list_get(pokemonesInnecesariosDT2, i));
		pokemonInnecesario2->cantidad = pokInnecesarioAux->cantidad;
		pokemonInnecesario2->pokemon = pokInnecesarioAux->pokemon;

		for (int j = 0; j < list_size(pokemonesPendientesDT1); j++) {
			t_pokemon_entrenador* pokemonPendiente1 =
					((t_pokemon_entrenador*) list_get(pokemonesPendientesDT1, j));

			if (strcmp(pokemonInnecesario2->pokemon, pokemonPendiente1->pokemon)
					== 0) {

				agregarPokemon(entrenador1, pokemonInnecesario2);
				quitarPokemon(entrenador2, pokemonInnecesario2);

			}

		}
	}
	//VER SI TENEMOS QUE HACERLO AFUERA POR LA PLANIFICACION RR DE QUANTUM
	sleep((g_config_team->retardo_ciclo_cpu) * 5);
	sem_wait(&mutex_ciclosCPU);
	ciclosCPU += 5;
	sem_post(&mutex_ciclosCPU);

	entrenador1->ciclosCPU += 5;
	entrenador2->ciclosCPU += 5;

	liberar_lista_de_pokemones(pokemonesInnecesariosDT1);
	liberar_lista_de_pokemones(pokemonesInnecesariosDT2);
	liberar_lista_de_pokemones(pokemonesPendientesDT1);
	liberar_lista_de_pokemones(pokemonesPendientesDT2);

}

void intercambiarNormalPokemon(t_entrenador* entrenador1,
		t_entrenador* entrenador2) {

	t_list* pokemonesInnecesariosDT1 = pokemonesInnecesarios(entrenador1);
	t_list* pokemonesInnecesariosDT2 = pokemonesInnecesarios(entrenador2);
	t_list* pokemonesPendientesDT1 = pokemonesPendientes(entrenador1);

	//NO HAY QUE LIBERAR ESTOS CUATRO!
	t_pokemon_entrenador* pokemonInnecesario1 = malloc(
			sizeof(t_pokemon_entrenador));
	//t_pokemon_entrenador* pokemonPendiente2 = malloc(sizeof(t_pokemon_entrenador));
	t_pokemon_entrenador* pokemonInnecesario2 = malloc(
			sizeof(t_pokemon_entrenador));
	//t_pokemon_entrenador* pokemonPendiente1 = malloc(sizeof(t_pokemon_entrenador));

	//Busco uno que le sirva al entrenador 1, de los innecesarios del entrenador 2
	for (int i = 0; i < list_size(pokemonesInnecesariosDT2); i++) {

		t_pokemon_entrenador* pokInnecesarioAux =
				((t_pokemon_entrenador*) list_get(pokemonesInnecesariosDT2, i));
		pokemonInnecesario2->cantidad = pokInnecesarioAux->cantidad;
		pokemonInnecesario2->pokemon = pokInnecesarioAux->pokemon;

		for (int j = 0; j < list_size(pokemonesPendientesDT1); j++) {
			t_pokemon_entrenador* pokemonPendiente1 =
					((t_pokemon_entrenador*) list_get(pokemonesPendientesDT1, j));

			if (strcmp(pokemonInnecesario2->pokemon, pokemonPendiente1->pokemon)
					== 0) {

				printf(
						"Al Entrenador %d se le quita el pokemon %s y se lo pasa al Entrenador %d \n",
						entrenador2->id, pokemonInnecesario2->pokemon,
						entrenador1->id);

				agregarPokemon(entrenador1, pokemonInnecesario2);
				quitarPokemon(entrenador2, pokemonInnecesario2);

			}

		}
	}

	//Busco uno que al entrenador 1 no le sirva
	for (int i = 0; i < list_size(entrenador1->pokemonesObtenidos); i++) {
		t_pokemon_entrenador* pokObtenidoAux = list_get(
				entrenador1->pokemonesObtenidos, i);
		t_pokemon_entrenador* pokObjetivoAux = list_buscar(
				entrenador1->objetivoEntrenador, pokObtenidoAux->pokemon);
		if (pokObjetivoAux == NULL) {
			pokemonInnecesario1->cantidad = pokObtenidoAux->cantidad;
			pokemonInnecesario1->pokemon = malloc(
					strlen(pokObtenidoAux->pokemon) + 1);
			memcpy(pokemonInnecesario1->pokemon, pokObtenidoAux->pokemon,
					strlen(pokObtenidoAux->pokemon) + 1);
			continue;
		} else if (pokObjetivoAux->cantidad < pokObtenidoAux->cantidad) {
			pokemonInnecesario1->cantidad = pokObtenidoAux->cantidad;
			pokemonInnecesario1->pokemon = malloc(
					strlen(pokObtenidoAux->pokemon) + 1);
			memcpy(pokemonInnecesario1->pokemon, pokObtenidoAux->pokemon,
					strlen(pokObtenidoAux->pokemon) + 1);
			continue;
		}
	}

	printf(
			"Al Entrenador %d se le quita el pokemon %s y se lo pasa al Entrenador %d \n",
			entrenador1->id, pokemonInnecesario1->pokemon, entrenador2->id);

	agregarPokemon(entrenador2, pokemonInnecesario1);
	quitarPokemon(entrenador1, pokemonInnecesario1);

	//VER SI TENEMOS QUE HACERLO AFUERA POR LA PLANIFICACION RR DE QUANTUM
	sleep((g_config_team->retardo_ciclo_cpu) * 5);
	sem_wait(&mutex_ciclosCPU);
	ciclosCPU += 5;
	sem_post(&mutex_ciclosCPU);

	entrenador1->ciclosCPU += 5;

	if ((!strcmp(g_config_team->algoritmo_planificion, "SJF-CD"))
			|| (!strcmp(g_config_team->algoritmo_planificion, "SJF-SD"))) {

		entrenador1->instruccion_actual += 5;
		entrenador1->estimacion_actual -= 5;
		entrenador1->ejec_anterior = 0;

	}

	liberar_lista_de_pokemones(pokemonesInnecesariosDT1);
	liberar_lista_de_pokemones(pokemonesInnecesariosDT2);
	liberar_lista_de_pokemones(pokemonesPendientesDT1);

}

void agregarPokemon(t_entrenador* entrenador, t_pokemon_entrenador* pokemon) {

	char loEncontro = 0;

	for (int i = 0; i < list_size(entrenador->pokemonesObtenidos); i++) {
		t_pokemon_entrenador* pokemonABuscar =
				((t_pokemon_entrenador*) list_get(
						entrenador->pokemonesObtenidos, i));
		if (strcmp(pokemonABuscar->pokemon, pokemon->pokemon) == 0) {
			pokemonABuscar->cantidad++;
			loEncontro = 1;
		}
	}

	if (loEncontro == 0) {
		t_pokemon_entrenador* pokemonAAgregar = malloc(
				sizeof(t_pokemon_entrenador));
		pokemonAAgregar->cantidad = 1;
		pokemonAAgregar->pokemon = malloc(strlen(pokemon->pokemon) + 1);
		memcpy(pokemonAAgregar->pokemon, pokemon->pokemon,
				strlen(pokemon->pokemon) + 1);
		pokemonAAgregar->posicion = malloc(sizeof(t_posicion_entrenador*));
		pokemonAAgregar->posicion->pos_x = 0;
		pokemonAAgregar->posicion->pos_y = 0;

		list_add(entrenador->pokemonesObtenidos, pokemonAAgregar);
	}
}

void quitarPokemon(t_entrenador* entrenador, t_pokemon_entrenador* pokemon) {
	char hayQueEliminarPokemon = 0;
	int indiceLista = 0;

	for (int i = 0; i < list_size(entrenador->pokemonesObtenidos); i++) {
		t_pokemon_entrenador* pokemonABuscar =
				((t_pokemon_entrenador*) list_get(
						entrenador->pokemonesObtenidos, i));
		if (strcmp(pokemonABuscar->pokemon, pokemon->pokemon) == 0) {
			if (pokemonABuscar->cantidad > 1) {
				pokemonABuscar->cantidad--;
			} else {
				hayQueEliminarPokemon = 1;
				indiceLista = i;
			}
			continue;

		}
	}

	if (hayQueEliminarPokemon == 1) {

		t_pokemon_entrenador* pokemonAEliminar =
				(t_pokemon_entrenador*) list_remove(
						entrenador->pokemonesObtenidos, indiceLista);

		free(pokemonAEliminar->pokemon); //Se deja comentado porque por ahora no malloqueamos los char*
		free(pokemonAEliminar->posicion);
		free(pokemonAEliminar);
	}

}

///FALTA LIBERAR LAS 4 LISTAS
char elIntercambioEsIdeal(t_entrenador* entrenador1, t_entrenador* entrenador2) {

	t_list* pokemonesInnecesariosDT1 = pokemonesInnecesarios(entrenador1);
	t_list* pokemonesInnecesariosDT2 = pokemonesInnecesarios(entrenador2);
	t_list* pokemonesPendientesDT1 = pokemonesPendientes(entrenador1);
	t_list* pokemonesPendientesDT2 = pokemonesPendientes(entrenador2);

	char leSirveAlDT1 = 0;
	char leSirveAlDT2 = 0;

	//Evalúo que le sirva al entrenador 2
	for (int i = 0; i < list_size(pokemonesInnecesariosDT1); i++) {

		char* pokemonInnecesario = ((t_pokemon_entrenador*) list_get(
				pokemonesInnecesariosDT1, i))->pokemon;

		for (int j = 0; j < list_size(pokemonesPendientesDT2); j++) {
			char* pokemonPendiente = ((t_pokemon_entrenador*) list_get(
					pokemonesPendientesDT2, j))->pokemon;

			if (strcmp(pokemonInnecesario, pokemonPendiente) == 0) {

				leSirveAlDT2 = 1;

			}

		}
	}

	//Evalúo que le sirva al entrenador 1
	for (int i = 0; i < list_size(pokemonesInnecesariosDT2); i++) {

		char* pokemonInnecesario = ((t_pokemon_entrenador*) list_get(
				pokemonesInnecesariosDT2, i))->pokemon;

		for (int j = 0; j < list_size(pokemonesPendientesDT1); j++) {
			char* pokemonPendiente = ((t_pokemon_entrenador*) list_get(
					pokemonesPendientesDT1, j))->pokemon;

			if (strcmp(pokemonInnecesario, pokemonPendiente) == 0) {

				leSirveAlDT1 = 1;

			}

		}
	}

	liberar_lista_de_pokemones(pokemonesInnecesariosDT1);
	liberar_lista_de_pokemones(pokemonesInnecesariosDT2);
	liberar_lista_de_pokemones(pokemonesPendientesDT1);
	liberar_lista_de_pokemones(pokemonesPendientesDT2);

	//Para poder intercambiar, a los dos les debe interesar el intercambio
	return leSirveAlDT1 && leSirveAlDT2;

}

char puedoIntercambiar(t_entrenador* entrenadorPorEjecutar,
		t_entrenador* otroEntrenador) {

	t_list* pokemonesPendientesEntrenadorPorEjecutar = pokemonesPendientes(
			entrenadorPorEjecutar);
	t_list* pokemonesInnecesariosOtroEntrenador = pokemonesInnecesarios(
			otroEntrenador);

	char leSirveAlDTPorEjecutar = 0;

	//Evalúo que le sirva al entrenador por ejecutar
	for (int i = 0; i < list_size(pokemonesInnecesariosOtroEntrenador); i++) {

		char* pokemonInnecesario = ((t_pokemon_entrenador*) list_get(
				pokemonesInnecesariosOtroEntrenador, i))->pokemon;

		for (int j = 0; j < list_size(pokemonesPendientesEntrenadorPorEjecutar);
				j++) {
			char* pokemonPendiente = ((t_pokemon_entrenador*) list_get(
					pokemonesPendientesEntrenadorPorEjecutar, j))->pokemon;

			if (strcmp(pokemonInnecesario, pokemonPendiente) == 0) {

				leSirveAlDTPorEjecutar = 1;

			}

		}
	}
	return leSirveAlDTPorEjecutar;
}

t_list* pokemonesInnecesarios(t_entrenador* entrenador) {

	t_list* pokemonesInnesarios = list_create();

	for (int i = 0; i < list_size(entrenador->pokemonesObtenidos); i++) {
		char esObjetivo = 0;
		int cantidadObjetivo = 0;

		t_pokemon_entrenador* pokemonObtenido =
				((t_pokemon_entrenador*) list_get(
						entrenador->pokemonesObtenidos, i));

		for (int j = 0; j < list_size(entrenador->objetivoEntrenador); j++) {
			t_pokemon_entrenador* pokemonObjetivo =
					((t_pokemon_entrenador*) list_get(
							entrenador->objetivoEntrenador, j));

			if (strcmp(pokemonObtenido->pokemon, pokemonObjetivo->pokemon) == 0
					&& (pokemonObtenido->cantidad > pokemonObjetivo->cantidad)) {

				cantidadObjetivo = pokemonObtenido->cantidad
						- pokemonObjetivo->cantidad;
				esObjetivo = 1;
			}

		}

		if (!esObjetivo) {

			t_pokemon_entrenador* pokemonInnesario = malloc(
					sizeof(t_pokemon_entrenador));
			pokemonInnesario->cantidad = ((t_pokemon_entrenador*) list_get(
					entrenador->pokemonesObtenidos, i))->cantidad;
			pokemonInnesario->pokemon =
					malloc(
							strlen(
									((t_pokemon_entrenador*) list_get(
											entrenador->pokemonesObtenidos, i))->pokemon)
									+ 1);
			memcpy(pokemonInnesario->pokemon,
					((t_pokemon_entrenador*) list_get(
							entrenador->pokemonesObtenidos, i))->pokemon,
					strlen(
							((t_pokemon_entrenador*) list_get(
									entrenador->pokemonesObtenidos, i))->pokemon)
							+ 1);
			pokemonInnesario->posicion = malloc(sizeof(t_posicion_entrenador));
			list_add(pokemonesInnesarios, pokemonInnesario);

		} else if (cantidadObjetivo > 0) {
			t_pokemon_entrenador* pokemonInnesario = malloc(
					sizeof(t_pokemon_entrenador));
			pokemonInnesario->cantidad = cantidadObjetivo;
			pokemonInnesario->pokemon =
					malloc(
							strlen(
									((t_pokemon_entrenador*) list_get(
											entrenador->pokemonesObtenidos, i))->pokemon)
									+ 1);
			memcpy(pokemonInnesario->pokemon,
					((t_pokemon_entrenador*) list_get(
							entrenador->pokemonesObtenidos, i))->pokemon,
					strlen(
							((t_pokemon_entrenador*) list_get(
									entrenador->pokemonesObtenidos, i))->pokemon)
							+ 1);
			pokemonInnesario->posicion = malloc(sizeof(t_posicion_entrenador));
			list_add(pokemonesInnesarios, pokemonInnesario);
		}

	}
	return pokemonesInnesarios;
}

t_list* pokemonesPendientes(t_entrenador* entrenador) {

	t_list* pokemonesPendientes = list_create();

	for (int i = 0; i < list_size(entrenador->objetivoEntrenador); i++) {
		char esPendiente = 0;
		int cantidadObjetivo = 0;
		t_pokemon_entrenador* pokemonObjetivo =
				((t_pokemon_entrenador*) list_get(
						entrenador->objetivoEntrenador, i));

		for (int j = 0; j < list_size(entrenador->pokemonesObtenidos); j++) {
			t_pokemon_entrenador* pokemonObtenido =
					((t_pokemon_entrenador*) list_get(
							entrenador->pokemonesObtenidos, j));

			if (strcmp(pokemonObtenido->pokemon, pokemonObjetivo->pokemon)
					== 0) {

				if (pokemonObtenido->cantidad < pokemonObjetivo->cantidad) {
					cantidadObjetivo = pokemonObjetivo->cantidad
							- pokemonObtenido->cantidad;
					esPendiente = 1;
				} else {
					cantidadObjetivo = -1;
				}

			}

		}

		if (esPendiente == 0 && cantidadObjetivo != -1) {

			t_pokemon_entrenador* pokemonPendiente = malloc(
					sizeof(t_pokemon_entrenador));
			pokemonPendiente->cantidad = ((t_pokemon_entrenador*) list_get(
					entrenador->objetivoEntrenador, i))->cantidad;
			pokemonPendiente->pokemon =
					malloc(
							strlen(
									((t_pokemon_entrenador*) list_get(
											entrenador->objetivoEntrenador, i))->pokemon)
									+ 1);
			memcpy(pokemonPendiente->pokemon,
					((t_pokemon_entrenador*) list_get(
							entrenador->objetivoEntrenador, i))->pokemon,
					strlen(
							((t_pokemon_entrenador*) list_get(
									entrenador->objetivoEntrenador, i))->pokemon)
							+ 1);
			pokemonPendiente->posicion = malloc(sizeof(t_posicion_entrenador));
			list_add(pokemonesPendientes, pokemonPendiente);

		} else if (cantidadObjetivo > 0) {
			t_pokemon_entrenador* pokemonPendiente = malloc(
					sizeof(t_pokemon_entrenador));
			pokemonPendiente->cantidad = cantidadObjetivo;
			pokemonPendiente->pokemon =
					malloc(
							strlen(
									((t_pokemon_entrenador*) list_get(
											entrenador->objetivoEntrenador, i))->pokemon)
									+ 1);
			memcpy(pokemonPendiente->pokemon,
					((t_pokemon_entrenador*) list_get(
							entrenador->objetivoEntrenador, i))->pokemon,
					strlen(
							((t_pokemon_entrenador*) list_get(
									entrenador->objetivoEntrenador, i))->pokemon)
							+ 1);
			pokemonPendiente->posicion = malloc(sizeof(t_posicion_entrenador));
			list_add(pokemonesPendientes, pokemonPendiente);
		}

	}
	return pokemonesPendientes;

}
