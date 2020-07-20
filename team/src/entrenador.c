/*
 * entrenador.c
 *
 *  Created on: 4 jun. 2020
 *      Author: utnso
 */


#include "entrenador.h"

//------------------------------FUNCIONES BÁSICAS DEL ENTRENADOR-------------------------------------------


void comportamiento_entrenador(t_entrenador* entrenador){

	int distancia;
	t_pokemon_entrenador* pokemon;
	t_pokemon_entrenador_reservado* pokemonReservado;
	t_posicion_entrenador* posicionEntrenadorAMoverse;
	t_entrenador* entrenador2;

	//La condición para cortar el hilo será que el entrenador esté en EXIT, y para que la función/hilo termine
	//va a tener que salir del while y finalizar la ejecución la función.
	while(entrenador->estado_entrenador != EXIT){

		sem_wait(&(entrenador->sem_entrenador));


		switch (entrenador->estado_entrenador) {
			case MOVERSE_A_POKEMON:
				pokemon = buscarPokemonMasCercano(entrenador->posicion);
				//Ya hay semaforos adentro
				t_pokemon_entrenador_reservado* pokemonReservado = moverPokemonAReservados(pokemon, entrenador->id);

				distancia = calcularDistancia(entrenador->posicion, pokemonReservado->posicion);

				printf("LA DISTANCIA ES %d \n", distancia);

				printf("POSICION DEL ENTRENADOR %d %d \n", entrenador->posicion->pos_x, entrenador->posicion->pos_y);

				printf("POSICION DEL POKEMON %d %d \n", pokemonReservado->posicion->pos_x, pokemonReservado->posicion->pos_y);

					for(int i=0; i<distancia; i++){
						moverEntrenador(entrenador, pokemonReservado->posicion);

					}

					entrenador->estado_entrenador = ATRAPAR;



					sem_post(&(sem_planificador_cplazoEntrenador));


					//// SIGNAL A PLANIFICADOR?????????

				break;
		case MOVERSE_A_ENTRENADOR:

			//RECORDAR que el entrenador que se está moviendo ahora, debería de dejar de estar en la cola de blocked
			//y estar en exit.
			posicionEntrenadorAMoverse = buscarEntrenadorAMoverse(entrenador);

			distancia = calcularDistancia(entrenador->posicion, posicionEntrenadorAMoverse);

			for(int i=0; i<distancia; i++){

			moverEntrenador(entrenador, posicionEntrenadorAMoverse);

			}

			entrenador->estado_entrenador = INTERCAMBIAR;

			sem_post(&(sem_planificador_cplazoEntrenador));


							break;
			case ATRAPAR:

				printf("ENTRO AL CASE DE ATRAPAR \n");
				pokemonReservado = buscarPokemonReservado(entrenador->id);


				//int resultadoEnvioMensaje = intentarAtraparPokemon(entrenador, pokemonReservado);

				sleep(g_config_team->retardo_ciclo_cpu);

				ciclosCPU++;

				entrenador->ciclosCPU++;

			/*if (resultadoEnvioMensaje == -1) {
				sem_wait(entrenador->mutex_entrenador);
				entrenador->estado_entrenador = RECIBIO_RESPUESTA_OK;
				sem_post(entrenador->mutex_entrenador);
			} else {
				sem_wait(entrenador->mutex_entrenador);
				entrenador->estado_entrenador = ESPERAR_CAUGHT;
				sem_post(entrenador->mutex_entrenador);
			}*/

				//entrenador->estado_entrenador = ESPERAR_CAUGHT;
				entrenador->estado_entrenador = RECIBIO_RESPUESTA_OK;

	//----------EL SIGUIENTE CACHO DE CODIGO ES SOLO PARA PROBAR ESTO DE "RECIBIO_RESPUESTA_OK"
				//CUANDO SE ARREGLE EL MENSAJE DEFAULT ESTO SE TIENE QUE BORRAR Y SE HACE DESDE LA RECECPION
				//DEL MENSAJE

				t_pokemon_entrenador* pokemonAAgregarConvertido = malloc(sizeof(t_pokemon_entrenador));
				pokemonAAgregarConvertido->cantidad = pokemonReservado->cantidad;
				pokemonAAgregarConvertido->pokemon = malloc(strlen(pokemonReservado->pokemon)+1);
				memcpy(pokemonAAgregarConvertido->pokemon, pokemonReservado->pokemon, strlen(pokemonReservado->pokemon)+1);
				pokemonAAgregarConvertido->posicion = malloc(sizeof(t_posicion_entrenador));
				pokemonAAgregarConvertido->posicion->pos_x = pokemonReservado->posicion->pos_x;
				pokemonAAgregarConvertido->posicion->pos_y = pokemonReservado->posicion->pos_y;

				//Borro de la lista al pokemon reservado
				sem_wait(&(sem_pokemonesReservados));
				int indice;
				for (int i = 0; i < list_size(pokemonesReservadosEnElMapa);i++) {

					t_pokemon_entrenador_reservado* aux =((t_pokemon_entrenador_reservado*) list_get(pokemonesReservadosEnElMapa, i));

					if (aux == pokemonReservado) {
						indice = i;
					}
				}
				pokemonReservado = list_remove(pokemonesReservadosEnElMapa, indice);
				sem_post(&(sem_pokemonesReservados));

				free(pokemonReservado->posicion);
				//free(pokemonReservadoAAgregar->pokemon);
				free(pokemonReservado);

				//Agrego el Poke
				sem_wait(&(entrenador->mutex_entrenador));
				agregarPokemon(entrenador, pokemonAAgregarConvertido);
				sem_post(&(entrenador->mutex_entrenador));

				//Muevo el pokemon a la lista global de atrapados
				sem_wait(&(sem_pokemonesGlobalesAtrapados));
				agregarPokemonAGlobalesAtrapados(pokemonAAgregarConvertido);
				sem_post(&(sem_pokemonesGlobalesAtrapados));

//----------------------------HASTA ACA HAY QUE BORRAR---------------------------


				log_info(g_logger,"Entrenador %d intenta atrapar al pokemon %s, en la posicion (%d,%d)", entrenador->id, pokemonReservado->pokemon, entrenador->posicion->pos_x, entrenador->posicion->pos_y);


				sem_post(&(sem_planificador_cplazoEntrenador));

				break;

			case INTERCAMBIAR:

				entrenador2 = buscarEntrenadorDelIntercambio(entrenador);

				intercambiarPokemon(entrenador, entrenador2);

				entrenador->estado_entrenador = ACABO_INTERCAMBIO;

				entrenador2->estado_entrenador = ACABO_INTERCAMBIO;

				log_info(g_logger, "Los entrenadores %d y %d hicieron un intercambio", entrenador->id, entrenador2->id);

				sem_post(&(sem_planificador_cplazoEntrenador));

				break;

			//case EXIT:
				//pthread_exit(NULL);
				//VERIFICAR SI SE TERMINA EL HILO DE ESTA FORMA.

				case -1:
					pthread_exit(NULL);
				}
		//FALTA DETERMINAR EL CASO DEL FINALIZADO DEL ENTRENADOR

	}


}

t_pokemon_entrenador* buscarPokemonMasCercano(t_posicion_entrenador* posicion_Entrenador){

	t_pokemon_entrenador* pokemonMasCercano;
	int distanciaMasCercana = 100000;
	int distanciaAux = 0;


	sem_wait(&sem_pokemonesLibresEnElMapa);


	for(int j = 0; j< list_size(pokemonesLibresEnElMapa); j++){

		t_pokemon_entrenador* pokLibreAux = list_get(pokemonesLibresEnElMapa, j);

		printf("POKEMON EN LIBRE ES %s y su POSICION ES %d %d \n", pokLibreAux->pokemon, pokLibreAux->posicion->pos_x, pokLibreAux->posicion->pos_y);

		if(necesitoPokemon(pokLibreAux->pokemon)){

			distanciaAux = calcularDistancia(posicion_Entrenador, pokLibreAux->posicion );

			if(distanciaAux < distanciaMasCercana){
			distanciaMasCercana = distanciaAux;
			pokemonMasCercano = pokLibreAux;
				}

		}
	}

	sem_post(&sem_pokemonesLibresEnElMapa);


	return pokemonMasCercano;

}

t_entrenador* buscarEntrenadorDelIntercambio(t_entrenador* entrenador){

	t_entrenador* entrenadorMasCercano;
	t_entrenador* entrenadorAux;
	int distanciaAux = 0;

	sem_wait(&(sem_cola_blocked));

	for (int i = 0; i < queue_size(colaBlockedEntrenadores); i++) {



		entrenadorAux = (t_entrenador*) queue_pop(colaBlockedEntrenadores);


		distanciaAux = calcularDistancia(entrenador->posicion, entrenadorAux->posicion);

		if(distanciaAux == 0 && entrenadorAux->estado_entrenador == DEADLOCK
				&& puedeIntercambiarPokemon(entrenador, entrenadorAux)){

			entrenadorMasCercano = entrenadorAux;

		}


		queue_push(colaBlockedEntrenadores, entrenadorAux);

		}

	sem_post(&(sem_cola_blocked));


	return entrenadorMasCercano;

}

t_posicion_entrenador* buscarEntrenadorAMoverse(t_entrenador* entrenador){

	t_posicion_entrenador* posicionAMoverse;
	t_entrenador* entrenadorAux;

	sem_wait(&(sem_cola_blocked));

	for (int i = 0; i < queue_size(colaBlockedEntrenadores); i++) {



		entrenadorAux = (t_entrenador*) queue_pop(colaBlockedEntrenadores);


		if(entrenadorAux->estado_entrenador == DEADLOCK
				&& puedeIntercambiarPokemon(entrenador, entrenadorAux)){

			posicionAMoverse = entrenadorAux->posicion;

		}


		queue_push(colaBlockedEntrenadores, entrenadorAux);

		}

	sem_post(&(sem_cola_blocked));


	return posicionAMoverse;

}

t_pokemon_entrenador_reservado* moverPokemonAReservados(t_pokemon_entrenador* pokemonAMover,
		int idReservador) {

	t_pokemon_entrenador* pokemonAux;

	printf("POSICION DEL POKEMON A MOVER A RESERVADOS ---%d %d ---\n ", pokemonAMover->posicion->pos_x, pokemonAMover->posicion->pos_y);

	t_pokemon_entrenador_reservado* pokemonAAgregar = malloc(
			sizeof(t_pokemon_entrenador_reservado));
	pokemonAAgregar->cantidad = 1;
	pokemonAAgregar->pokemon = malloc(strlen(pokemonAMover->pokemon)+1);
	memcpy(pokemonAAgregar->pokemon, pokemonAMover->pokemon, strlen(pokemonAMover->pokemon)+1);
	pokemonAAgregar->posicion = malloc(sizeof(t_posicion_entrenador));
	pokemonAAgregar->posicion->pos_x = pokemonAMover->posicion->pos_x;
	pokemonAAgregar->posicion->pos_y = pokemonAMover->posicion->pos_y;
	pokemonAAgregar->id_entrenadorReserva = idReservador;



	sem_wait(&sem_pokemonesReservados);
	list_add(pokemonesReservadosEnElMapa, pokemonAAgregar);
	sem_post(&sem_pokemonesReservados);

	printf("POSICION DEL POKEMON A AGREGADO A RESERVADOS ---%d %d ---\n ", pokemonAAgregar->posicion->pos_x, pokemonAAgregar->posicion->pos_y);


	int indice = 0;

	sem_wait(&sem_pokemonesLibresEnElMapa);
	for (int i = 0; i < list_size(pokemonesLibresEnElMapa); i++) {

		pokemonAux = ((t_pokemon_entrenador*) list_get(
				pokemonesLibresEnElMapa, i));

		if (pokemonAux == pokemonAMover) {
			indice = i;
		}
	}

	if (pokemonAMover->cantidad == 1) {
		pokemonAux = list_remove(pokemonesLibresEnElMapa, indice);
		free(pokemonAux->posicion);
		free(pokemonAux);

	} else {

		pokemonAMover->cantidad--;
	}
	sem_post(&sem_pokemonesLibresEnElMapa);

	printf("POSICION DEL POKEMON A AGREGADO A RESERVADOS DESPUES DEL FREE---%d %d ---\n ", pokemonAAgregar->posicion->pos_x, pokemonAAgregar->posicion->pos_y);


	return pokemonAAgregar;
}


/////MOVER ENTRENADOR///////////////////

void moverEntrenador(t_entrenador* entrenador, t_posicion_entrenador* posicionAMoverse){

	//AGREGAR SEMAFORO MUTEX DEL ENTRENADOR EN PARTICULAR  (ADENTRO O AFUERA?????)

	if(entrenador->posicion->pos_x != posicionAMoverse->pos_x){
		if(entrenador -> posicion -> pos_x > posicionAMoverse->pos_x ){
				entrenador->posicion->pos_x -= 1;
			}else if(entrenador->posicion->pos_x < posicionAMoverse->pos_x){
				entrenador->posicion->pos_x += 1;
			}
	}else {
		if(entrenador -> posicion -> pos_y > posicionAMoverse->pos_y ){
				entrenador->posicion->pos_y -= 1;
			}else if(entrenador->posicion->pos_y < posicionAMoverse->pos_y){
				entrenador->posicion->pos_y += 1;
			}
	}


	sleep(g_config_team->retardo_ciclo_cpu);
	sem_wait(&mutex_ciclosCPU);
	ciclosCPU++;
	sem_post(&mutex_ciclosCPU);

	entrenador->ciclosCPU++;


	log_info(g_logger,"Entrenador %d se movio a la posicion (%d,%d) \n", entrenador->id, entrenador->posicion->pos_x, entrenador->posicion->pos_y);


}

int calcularDistancia(t_posicion_entrenador* posicionActual, t_posicion_entrenador* posicionAMoverse){
	 int distanciaTotal = 0;
	    if (posicionActual->pos_x > posicionAMoverse->pos_x){
	        distanciaTotal += posicionActual->pos_x - posicionAMoverse->pos_x;
	    } else {
	        distanciaTotal += posicionAMoverse->pos_x -posicionActual->pos_x;
	    }

	    if(posicionActual->pos_y > posicionAMoverse->pos_y){
	        distanciaTotal += posicionActual->pos_y - posicionAMoverse->pos_y;
	    } else {
	        distanciaTotal += posicionAMoverse->pos_y - posicionActual->pos_y ;
	    }

	    return distanciaTotal;

}

//////ATRAPAR UN POKEMON//////////////////

int intentarAtraparPokemon(t_entrenador* entrenador, t_pokemon_entrenador_reservado* pokemon){

	int prueba = enviar_catch_pokemon_broker(entrenador->posicion->pos_x, entrenador->posicion->pos_y, pokemon->pokemon, g_logger, entrenador->id);
	printf("PROBANDO LO DEL CATCH, EL ULTIMO PASO ES: %d \n", prueba);
	return prueba;

}

//////INTERCAMBIAR UN POKEMON/////////////

void intercambiarPokemon(t_entrenador* entrenador1, t_entrenador* entrenador2) {

	t_list* pokemonesInnecesariosDT1 = pokemonesInnecesarios(entrenador1);
	t_list* pokemonesInnecesariosDT2 = pokemonesInnecesarios(entrenador2);
	t_list* pokemonesPendientesDT1 = pokemonesPendientes(entrenador1);
	t_list* pokemonesPendientesDT2 = pokemonesPendientes(entrenador2);

	//NO HAY QUE LIBERAR ESTOS CUATRO!
	t_pokemon_entrenador* pokemonInnecesario1 = malloc(sizeof(t_pokemon_entrenador));
	//t_pokemon_entrenador* pokemonPendiente2 = malloc(sizeof(t_pokemon_entrenador));
	t_pokemon_entrenador* pokemonInnecesario2 = malloc(sizeof(t_pokemon_entrenador));
	//t_pokemon_entrenador* pokemonPendiente1 = malloc(sizeof(t_pokemon_entrenador));

	//Evalúo que le sirva al entrenador 2
	for (int i = 0; i < list_size(pokemonesInnecesariosDT1); i++) {

		t_pokemon_entrenador* pokInnecesarioAux = ((t_pokemon_entrenador*) list_get(pokemonesInnecesariosDT1, i));
		pokemonInnecesario1->cantidad=pokInnecesarioAux->cantidad;
		pokemonInnecesario1->pokemon=pokInnecesarioAux->pokemon;


		for (int j = 0; j < list_size(pokemonesPendientesDT2); j++) {
			t_pokemon_entrenador* pokemonPendiente2 =
					((t_pokemon_entrenador*) list_get(pokemonesPendientesDT2, j));

			if (strcmp(pokemonInnecesario1->pokemon, pokemonPendiente2->pokemon) == 0) {

				agregarPokemon(entrenador2, pokemonInnecesario1);
				quitarPokemon(entrenador1, pokemonInnecesario1);

			}

		}
	}

	//Evalúo que le sirva al entrenador 1
	for (int i = 0; i < list_size(pokemonesInnecesariosDT2); i++) {

		t_pokemon_entrenador* pokInnecesarioAux = ((t_pokemon_entrenador*) list_get(pokemonesInnecesariosDT2, i));
		pokemonInnecesario2->cantidad=pokInnecesarioAux->cantidad;
		pokemonInnecesario2->pokemon=pokInnecesarioAux->pokemon;


		for (int j = 0; j < list_size(pokemonesPendientesDT1); j++) {
			t_pokemon_entrenador* pokemonPendiente1 =
					((t_pokemon_entrenador*) list_get(pokemonesPendientesDT1, j));

			if (strcmp(pokemonInnecesario2->pokemon, pokemonPendiente1->pokemon) == 0) {

				agregarPokemon(entrenador1, pokemonInnecesario2);
				quitarPokemon(entrenador2, pokemonInnecesario2);

			}

		}
	}
	//VER SI TENEMOS QUE HACERLO AFUERA POR LA PLANIFICACION RR DE QUANTUM
		sleep((g_config_team->retardo_ciclo_cpu)*5);
		sem_wait(&mutex_ciclosCPU);
		ciclosCPU+=5;
		sem_post(&mutex_ciclosCPU);

		entrenador1->ciclosCPU+=5;
		entrenador2->ciclosCPU+=5;

	liberar_lista_de_pokemones(pokemonesInnecesariosDT1);
	liberar_lista_de_pokemones(pokemonesInnecesariosDT2);
	liberar_lista_de_pokemones(pokemonesPendientesDT1);
	liberar_lista_de_pokemones(pokemonesPendientesDT2);

}

void agregarPokemon(t_entrenador* entrenador, t_pokemon_entrenador* pokemon){

	char loEncontro = 0;

	for(int i=0; i < list_size(entrenador->pokemonesObtenidos); i++){
		t_pokemon_entrenador* pokemonABuscar =
						((t_pokemon_entrenador*) list_get(entrenador->pokemonesObtenidos, i));
		if(strcmp(pokemonABuscar->pokemon, pokemon->pokemon)==0){
			pokemonABuscar->cantidad++;
			loEncontro=1;
		}
	}

	if(loEncontro==0){
		t_pokemon_entrenador* pokemonAAgregar = malloc(sizeof(t_pokemon_entrenador));
		pokemonAAgregar->cantidad=1;
		pokemonAAgregar->pokemon=pokemon->pokemon;
		list_add(entrenador->pokemonesObtenidos, pokemonAAgregar);
	}
}

void quitarPokemon(t_entrenador* entrenador, t_pokemon_entrenador* pokemon) {
	printf("Entro a quitarPOkemon \n");
	char hayQueEliminarPokemon = 0;
	int indicePokemon;

	for (int i = 0; i < list_size(entrenador->pokemonesObtenidos); i++) {
		t_pokemon_entrenador* pokemonABuscar =
				((t_pokemon_entrenador*) list_get(
						entrenador->pokemonesObtenidos, i));
		if (strcmp(pokemonABuscar->pokemon, pokemon->pokemon) == 0) {
			if(pokemonABuscar->cantidad > 1){
				pokemonABuscar->cantidad--;
			}
			else{
				hayQueEliminarPokemon = 1;
				indicePokemon=i;
			}


		}
	}

	if (hayQueEliminarPokemon == 1) {
		t_pokemon_entrenador* pokemonAEliminar=list_remove(entrenador->pokemonesObtenidos, indicePokemon);
		//free(pokemonAEliminar->pokemon); Se deja comentado porque por ahora no malloqueamos los char*
		free(pokemonAEliminar);
	}
}

///FALTA LIBERAR LAS 4 LISTAS
char puedeIntercambiarPokemon(t_entrenador* entrenador1,
		t_entrenador* entrenador2) {

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




t_list* pokemonesInnecesarios(t_entrenador* entrenador){

	t_list* pokemonesInnesarios = list_create();

	for(int i=0; i< list_size(entrenador->pokemonesObtenidos); i++){
		char esObjetivo= 0;
		int cantidadObjetivo=0;

		t_pokemon_entrenador* pokemonObtenido= ((t_pokemon_entrenador*)list_get(entrenador->pokemonesObtenidos, i));

		for(int j=0; j< list_size(entrenador->objetivoEntrenador); j++){
			t_pokemon_entrenador* pokemonObjetivo= ((t_pokemon_entrenador*)list_get(entrenador->objetivoEntrenador, j));

			if(strcmp(pokemonObtenido->pokemon,pokemonObjetivo->pokemon)== 0 &&
					(pokemonObtenido->cantidad> pokemonObjetivo->cantidad)){

				cantidadObjetivo= pokemonObtenido->cantidad - pokemonObjetivo->cantidad;
				esObjetivo=1;
			}


		}

		if(!esObjetivo){

			t_pokemon_entrenador* pokemonInnesario = malloc(sizeof(t_pokemon_entrenador));
			pokemonInnesario->cantidad = ((t_pokemon_entrenador*)list_get(entrenador->pokemonesObtenidos, i))->cantidad;
			pokemonInnesario->pokemon = ((t_pokemon_entrenador*)list_get(entrenador->pokemonesObtenidos, i))->pokemon;
			pokemonInnesario->posicion = malloc(sizeof(t_posicion_entrenador));
			list_add(pokemonesInnesarios, pokemonInnesario);

		}else if(cantidadObjetivo>0){
			t_pokemon_entrenador* pokemonInnesario = malloc(sizeof(t_pokemon_entrenador));
			pokemonInnesario->cantidad = cantidadObjetivo;
			pokemonInnesario->pokemon = ((t_pokemon_entrenador*)list_get(entrenador->pokemonesObtenidos, i))->pokemon;
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
				}
				else{
					cantidadObjetivo = -1;
				}


			}

		}

		if (esPendiente == 0 && cantidadObjetivo != -1) {

			t_pokemon_entrenador* pokemonPendiente = malloc(
					sizeof(t_pokemon_entrenador));
			pokemonPendiente->cantidad = ((t_pokemon_entrenador*) list_get(
					entrenador->objetivoEntrenador, i))->cantidad;
			pokemonPendiente->pokemon = ((t_pokemon_entrenador*) list_get(
					entrenador->objetivoEntrenador, i))->pokemon;
			pokemonPendiente->posicion = malloc(sizeof(t_posicion_entrenador));
			list_add(pokemonesPendientes, pokemonPendiente);

		} else if (cantidadObjetivo > 0) {
			t_pokemon_entrenador* pokemonPendiente = malloc(
					sizeof(t_pokemon_entrenador));
			pokemonPendiente->cantidad = cantidadObjetivo;
			pokemonPendiente->pokemon = ((t_pokemon_entrenador*) list_get(
					entrenador->objetivoEntrenador, i))->pokemon;
			pokemonPendiente->posicion = malloc(sizeof(t_posicion_entrenador));
			list_add(pokemonesPendientes, pokemonPendiente);
		}

	}
	return pokemonesPendientes;


}
