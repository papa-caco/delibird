/*
 * utilsTeam.c
 *
 *  Created on: 3 mar. 2019
 *      Author: Los Que Aprueban
 */
#include "utilsTeam.h"
#include "entrenador.h"
//Lo que cambié fue que esto importe a su .h, y su .h importe al teamInitializer

void inicio_server_team(void) {
	char *ip = g_config_team->ip_team;
	char *puerto = g_config_team->puerto_team;
	puts("");
	iniciar_servidor(ip, puerto, g_logger);
}

void iniciar_suscripciones_broker(void) {
	puts("Estableciendo conexiones con el BROKER -- Iniciando Suscripciones");
	puts("");
	int status_appeared = iniciar_suscripcion_cola(APPEARED_POKEMON, g_logger);
	int status_localized = iniciar_suscripcion_cola(LOCALIZED_POKEMON,
			g_logger);
	int status_caught = iniciar_suscripcion_cola(CAUGHT_POKEMON, g_logger);
}

int iniciar_suscripcion_cola(t_tipo_mensaje cola_suscripta, t_log *logger) {
	pthread_t tid;
	t_tipo_mensaje *cola = malloc(sizeof(t_tipo_mensaje));
	memcpy(cola, &cola_suscripta, sizeof(t_tipo_mensaje));
	sem_wait(&sem_mutex_msjs);
	int tid_status = pthread_create(&tid, NULL, (void*) inicio_suscripcion,
			(void*) cola);
	if (tid_status != 0) {
		log_error(logger, "Thread create returned %d | %s", tid_status,
				strerror(tid_status));
	} else {
		pthread_detach(tid);
	}
	return tid_status;
}

// ------ USAR ESTA FUNCION PARA ENVIAR MENSAJES GET_POKEMON AL BROKER ----------//
int enviar_get_pokemon_broker(char *pokemon, t_log *logger) {
	pthread_t tid;
	t_msg_get_broker *msj_get = malloc(sizeof(t_msg_get_broker));
	msj_get->size_pokemon = strlen(pokemon) + 1;
	msj_get->pokemon = malloc(sizeof(msj_get->size_pokemon));
	memcpy(msj_get->pokemon, pokemon, msj_get->size_pokemon);
	sem_wait(&sem_mutex_msjs);
	int thread_status = pthread_create(&tid, NULL,
			(void*) connect_broker_y_enviar_mensaje_get, (void*) msj_get);
	if (thread_status != 0) {
		log_error(logger, "Thread create returned %d | %s", thread_status,
				strerror(thread_status));
	} else {
		pthread_detach(tid);
	}
	return thread_status;
}

int connect_broker_y_enviar_mensaje_get(t_msg_get_broker *msg_get) {
	char *ip = g_config_team->ip_broker;
	char *puerto = g_config_team->puerto_broker;
	char *proceso = "BROKER";
	char *name_cola = nombre_cola(GET_POKEMON);
	int cliente_fd = crear_conexion(ip, puerto, g_logger, proceso, name_cola);
	enviar_msj_get_broker(cliente_fd, g_logger, msg_get);
	int id_mensaje = -1;
	if (rcv_codigo_operacion(cliente_fd) == ID_MENSAJE) {
		id_mensaje = rcv_id_mensaje(cliente_fd, g_logger);
		//TODO Guardar ID_MENSAJE que envia el BROKER de GET_POKEMON
	}
	sem_post(&sem_mutex_msjs);
	//pthread_exit(&tid_send_get);
	sem_wait(&mutex_idCorrelativosGet);
	int *id_msj = malloc(sizeof(int));
	memcpy(id_msj, &id_mensaje, sizeof(int));
	void *id = (int*) id_msj;
	list_add(idCorrelativosGet, id);
	sem_post(&mutex_idCorrelativosGet);
	eliminar_msg_get_broker(msg_get);
	close(cliente_fd);
	return id_mensaje;
}

int enviar_catch_pokemon_broker(int pos_x, int pos_y, char* pokemon, t_log *logger, int id_entrenador)
{ // ------ USAR ESTA FUNCION PARA ENVIAR MENSAJES CATCH_POKEMON AL BROKER ----------//
	t_msg_catch_team_broker *msg_catch = malloc(sizeof(t_msg_catch_team_broker));
	msg_catch->pos_x = pos_x;
	msg_catch->pos_y = pos_y;
	msg_catch->id_entrenador = id_entrenador;
	msg_catch->size_pokemon = strlen(pokemon) + 1;
	msg_catch->pokemon = malloc(msg_catch->size_pokemon);
	memcpy(msg_catch->pokemon, pokemon, msg_catch->size_pokemon);
	pthread_t tid;
	int thread_status = pthread_create(&tid, NULL,
			(void*) connect_broker_y_enviar_mensaje_catch, (void*) msg_catch);
	if (thread_status != 0) {
		log_error(logger, "Thread create returned %d | %s", thread_status,
		strerror(thread_status));
	} else {
		pthread_detach(tid);
	}
	return thread_status;
}

int connect_broker_y_enviar_mensaje_catch(t_msg_catch_team_broker *msg_catch_team)
{
	char *ip = g_config_team->ip_broker;
	char *puerto = g_config_team->puerto_broker;
	char *proceso = "BROKER";
	char *name_cola = nombre_cola(CATCH_POKEMON);
	int id_mensaje = -1;
	t_msg_catch_broker* msg_catch = malloc(sizeof(t_msg_catch_broker));
	msg_catch->coordenada = malloc(sizeof(t_coordenada));
	msg_catch->coordenada->pos_x = msg_catch_team->pos_x;
	msg_catch->coordenada->pos_y = msg_catch_team->pos_y;
	msg_catch->size_pokemon = msg_catch_team->size_pokemon;
	msg_catch->pokemon = malloc(msg_catch->size_pokemon);
	memcpy(msg_catch->pokemon, msg_catch_team->pokemon, msg_catch->size_pokemon);
	t_id_Correlativo_and_Entrenador* ids = malloc(sizeof(t_id_Correlativo_and_Entrenador));
	ids->id_Entrenador = msg_catch_team->id_entrenador;
	sem_wait(&sem_mutex_msjs);
	int cliente_fd = crear_conexion(ip, puerto, g_logger, proceso, name_cola);
	if (cliente_fd > 0) {
		sem_wait(&mutex_idCorrelativos);
		enviar_msj_catch_broker(cliente_fd, g_logger, msg_catch);
		op_code code_op = rcv_codigo_operacion(cliente_fd);
		if (code_op == ID_MENSAJE) {
			id_mensaje = rcv_id_mensaje(cliente_fd, g_logger);
		} else if (code_op == MSG_ERROR) {
			void* a_recibir = recibir_buffer(cliente_fd, &id_mensaje);
			log_warning(g_logger, "(RECEIVING: |%s)", a_recibir);
			free(a_recibir);
		}
	}
	sem_post(&sem_mutex_msjs);
	close(cliente_fd);
	eliminar_msg_catch_broker(msg_catch);
	free(msg_catch_team->pokemon);
	free(msg_catch_team);
	//sem_wait(&mutex_idCorrelativos);
	ids->id_Correlativo = id_mensaje;
	list_add(idCorrelativosCatch, ids);
	if (id_mensaje != -1) {
			rta_catch = id_mensaje;
	}
	sem_post(&mutex_idCorrelativos);


	return id_mensaje;
}

void inicio_suscripcion(t_tipo_mensaje *cola) {
	char *ip = g_config_team->ip_broker;
	char *puerto = g_config_team->puerto_broker;
	char *proceso = "BROKER";
	char *name_cola = nombre_cola(*cola);
	int cliente_fd = crear_conexion(ip, puerto, g_logger, proceso, name_cola);
	sem_post(&sem_mutex_msjs);
	if (cliente_fd >= 0) {
		status_conn_broker = true;
		t_handsake_suscript *handshake = malloc(sizeof(t_handsake_suscript));
		handshake->id_suscriptor = g_config_team->id_suscriptor;
		handshake->id_recibido = 0;
		handshake->cola_id = *cola;
		handshake->msjs_recibidos = 0;
		enviar_msj_handshake_suscriptor(cliente_fd, g_logger, handshake);
		op_code cod_oper_mensaje = 0;
		uint32_t contador_msjs = 0;
		int flag_salida = 1;
		//En este bucle se queda recibiendo los mensajes que va enviando el BROKER al suscriptor
		while (flag_salida) {
			uint32_t id_recibido;
			cod_oper_mensaje = rcv_codigo_operacion(cliente_fd);
			if (!codigo_operacion_valido(cod_oper_mensaje)) { // Posible desconexión del BROKER
				log_error(g_logger, "(Se perdió Conexión con BROKER|%s)",
						nombre_cola(handshake->cola_id));
				status_conn_broker = false;
				flag_salida = 0;
			} else if (cod_oper_mensaje != SUSCRIP_END) {
				id_recibido = rcv_msjs_broker_publish(cod_oper_mensaje,
						cliente_fd, g_logger);
				contador_msjs += 1;
				handshake->msjs_recibidos = contador_msjs;
				handshake->id_recibido = id_recibido;
				//Actualizo el ID_RECIBIDO porque en el próximo envío confirmo recepción del mensaje anterior
				enviar_msj_handshake_suscriptor(cliente_fd, g_logger,
						handshake);
			} else if (rcv_msg_suscrip_end(cliente_fd)
					!= g_config_team->id_suscriptor) {
				pthread_exit((int*) EXIT_FAILURE);
			} else {
				flag_salida = 0;
			}
		}
		int contador_global = contador_msjs_cola(*cola);
		log_debug(g_logger,
				"(TEAM END_SUSCRIPTION: |RECVD_MSGs:%d|TOTAL_MSGS:%d)",
				contador_msjs, contador_global);
		free(handshake);
		close(cliente_fd);
	} else {
		status_conn_broker = false;
	}

}

uint32_t rcv_msjs_broker_publish(op_code codigo_operacion, int socket_cliente,
		t_log *logger) {
	uint32_t id_recibido = 0, id_correlativo = 0;
	void *msg;
	switch (codigo_operacion) {
	case COLA_VACIA:;
		id_recibido = rcv_msj_cola_vacia(socket_cliente, logger);
		if (id_recibido != g_config_team->id_suscriptor) {
			log_error(logger, "MSG_ERROR");
		}
		break;
	case APPEARED_TEAM:;
		t_msg_appeared_team *msg_appeared = (t_msg_appeared_team*) msg;
		msg_appeared = rcv_msj_appeared_team(socket_cliente, logger);
		g_cnt_msjs_appeared++;
		id_recibido = msg_appeared->id_mensaje;
		recibir_msg_appeared_pokemon(msg_appeared, logger);
		break;
	case CAUGHT_TEAM:;
		t_msg_caught_team *msg_caught = (t_msg_caught_team*) msg;
		msg_caught = rcv_msj_caught_team(socket_cliente, logger);
		g_cnt_msjs_caught++;
		id_recibido = msg_caught->id_mensaje;
		recibir_msg_caught_pokemon(msg_caught, logger);
		break;
	case LOCALIZED_TEAM:;
		t_msg_localized_team *msg_localized = (t_msg_localized_team*) msg;
		msg_localized = rcv_msj_localized_team(socket_cliente, logger);
		g_cnt_msjs_localized++;
		id_recibido = msg_localized->id_mensaje;
		recibir_msg_localized_pokemon(msg_localized, logger);
		break;
	case 0:
		pthread_exit(NULL);
	case -1:
		pthread_exit(NULL);
	}
	return id_recibido;
}

int recibir_msg_appeared_pokemon(t_msg_appeared_team *msg_appeared, t_log *logger)
{
	pthread_t tid;
	int thread_status = pthread_create(&tid, NULL, (void*) procesar_msg_appeared, (void*) msg_appeared);
	if (thread_status != 0) {
		log_error(logger, "Thread create returned %d | %s", thread_status, strerror(thread_status));
	} else {
		pthread_detach(tid);
	}
	return thread_status;
}

int recibir_msg_caught_pokemon(t_msg_caught_team *msg_caught, t_log *logger)
{
	pthread_t tid;
	int thread_status = pthread_create(&tid, NULL, (void*) procesar_msg_caught, (void*) msg_caught);
	if (thread_status != 0) {
		log_error(logger, "Thread create returned %d | %s", thread_status, strerror(thread_status));
	} else {
		pthread_detach(tid);
	}
	return thread_status;
}

int recibir_msg_localized_pokemon(t_msg_localized_team *msg_localized, t_log *logger)
{
	pthread_t tid;
	int thread_status = pthread_create(&tid, NULL, (void*) procesar_msg_localized, (void*) msg_localized);
	if (thread_status != 0) {
		log_error(logger, "Thread create returned %d | %s", thread_status, strerror(thread_status));
	} else {
		pthread_detach(tid);
	}
	return thread_status;
}

void procesar_msg_appeared(t_msg_appeared_team *msg_appeared)
{
	char *pokemon = malloc(msg_appeared->size_pokemon);
	memcpy(pokemon, msg_appeared->pokemon, msg_appeared->size_pokemon);
	t_posicion_entrenador *posicion = malloc(sizeof(t_posicion_entrenador));
	posicion->pos_x = msg_appeared->coord->pos_x;
	posicion->pos_y = msg_appeared->coord->pos_y;
	eliminar_msg_appeared_team(msg_appeared);
	//printf("\nRecibí msg_appeared ->> pokemon: %s | X=%d | Y=%d\n\n",
	// TODO Borrar //	pokemon, posicion->pos_x, posicion->pos_y);
	//meSirvePokemon ya tiene semaforos dentro
	//if (meSirvePokemon(msg_appeared->pokemon)) {
	//Las dos líneas anteriores las comento por las dudas de que en realidad al final sí haya que realizar ese tipo de
	//validació
	//necesitoPokemon ya tiene los semáforos adentro
	if (necesitoPokemon(pokemon) != 0) {
		char* mensajeAppeared = malloc(strlen(pokemon) + 1);
		memcpy(mensajeAppeared, pokemon, strlen(pokemon) + 1);
		pthread_mutex_lock(&mutex_listaPokemonesLlegadosDelBroker);
		list_add(pokemonesLlegadosDelBroker, mensajeAppeared);
		pthread_mutex_unlock(&mutex_listaPokemonesLlegadosDelBroker);
		t_pokemon_entrenador* pokemonAAgregarAlMapa = malloc(sizeof(t_pokemon_entrenador));

		pokemonAAgregarAlMapa->cantidad = 1;
		pokemonAAgregarAlMapa->pokemon = malloc(strlen(pokemon) + 1);
		memcpy(pokemonAAgregarAlMapa->pokemon, pokemon, strlen(pokemon) + 1);
		pokemonAAgregarAlMapa->posicion = malloc(sizeof(t_posicion_entrenador));
		pokemonAAgregarAlMapa->posicion->pos_x = posicion->pos_x;
		pokemonAAgregarAlMapa->posicion->pos_y = posicion->pos_y;

		char verificaSiEsNecesarioAtraparlo = 0;
		char yaLoAgregueAlMapa = 0;

		sem_wait(&sem_pokemonesLibresEnElMapa);
		if (list_size(pokemonesLibresEnElMapa) == 0) {
			list_add(pokemonesLibresEnElMapa, pokemonAAgregarAlMapa);
			yaLoAgregueAlMapa = 1;
			sem_post(&sem_hay_pokemones_mapa);
		} else {
			//ESTO LO HAGO PORQUE SI NO SE QUEDA HACIENDO UN DOBLE WAIT DEL SEMAFORO DE POKEMONESLIBRESENELMAPA
			verificaSiEsNecesarioAtraparlo = 1;
		}
		sem_post(&sem_pokemonesLibresEnElMapa);

		if (verificaSiEsNecesarioAtraparlo == 1) {
			if (necesitoIrAAtraparlo(pokemonAAgregarAlMapa->pokemon)) {
				sem_wait(&sem_pokemonesLibresEnElMapa);
				list_add(pokemonesLibresEnElMapa, pokemonAAgregarAlMapa);
				sem_post(&sem_pokemonesLibresEnElMapa);
				yaLoAgregueAlMapa = 1;
				sem_post(&sem_hay_pokemones_mapa);
			}
		}
		if (yaLoAgregueAlMapa == 0) {
			sem_wait(&sem_pokemonesLibresEnElMapa);
			list_add(pokemonesLibresEnElMapa, pokemonAAgregarAlMapa);
			sem_post(&sem_pokemonesLibresEnElMapa);
		}
		if (cantidadDeEntrenadores == queue_size(colaBlockedEntrenadores)) {
			sem_post(&sem_activacionPlanificadorMPlazo);
		}
		////ANALIZAR SI TENEMOS QUE ACTIVAR EL PLANIFICADOR MEDIANO PLAZO PARA IR A ATRAPAR A UN POKEMON
	}
	// Las funciones comentadas de abajo estuvieron para prueba, no tienen lógica
	// Se pusieron para probar el reenvío de CATCH_POKEMON y GET_POKEMON. FUNCIONAN las DOS.
	/*if (id_recibido > 20 && id_recibido < 30) {
	 enviar_catch_de_appeared(msg_appeared);
	 }
	 else if (id_recibido > 30) {
	 enviar_get_de_appeared(msg_appeared);
	 }*/
}

void procesar_msg_caught(t_msg_caught_team *msg_caught)
{
	int id_correlativo = msg_caught->id_correlativo;
	t_result_caught resultado = msg_caught->resultado;
	free(msg_caught);
	//printf("\nRecibí msg_caught ->> id_correlativo:%d ->> Resultado:%d\n\n"
	//		,id_correlativo, resultado);  // TODO Borrar
	//FIltramos los ids que nos corresponden por id correlativo
	t_id_Correlativo_and_Entrenador* idAux;
	char meSirve = 0;
	sem_wait(&mutex_idCorrelativos);
	int cant_ids = list_size(idCorrelativosCatch);
	for (int i = 0; i < cant_ids; i++) {
		idAux = (t_id_Correlativo_and_Entrenador*) list_get(idCorrelativosCatch, i);
		if (idAux->id_Correlativo == id_correlativo) {
			//printf("5555555555555555555555555555555555555555555555555555555555555555555555555555555 \n");
			//printf("Encontro el id correlativo %d en la lista de idsCorrelativosCatch \n", id_recibido);
			//printf("5555555555555555555555555555555555555555555555555555555555555555555555555555555 \n");
			meSirve = 1;
		}
	}
	sem_post(&mutex_idCorrelativos);
	if (meSirve == 1) {
		//Busco al entrenador que hizo la reserva
		sem_wait(&(sem_cola_blocked));
		t_entrenador* entrenadorReservador = buscarEntrenadorDeLaReserva(idAux->id_Entrenador);
		sem_post(&(sem_cola_blocked));
		//printf("ID ENTRENADOR %d \n", entrenadorReservador->id);
		//Busco el pokemon que corresponde al entrenador, es decir, el que él reservó
		sem_wait(&(sem_pokemonesReservados));
		t_pokemon_entrenador_reservado* pokemonReservadoAAgregar =
				buscarPokemonReservado(idAux->id_Entrenador);
		sem_post(&(sem_pokemonesReservados));
		if (resultado == OK) {
			//AGREGAR POKEMON A LA LISTA DE ENTRENADORES DEL POKEMON
			//Transformo el pokemonReservado al tipo pokemon_entrenador
			t_pokemon_entrenador* pokemonAAgregarConvertido = malloc(
					sizeof(t_pokemon_entrenador));
			pokemonAAgregarConvertido->cantidad =
					pokemonReservadoAAgregar->cantidad;
			pokemonAAgregarConvertido->pokemon = malloc(
					strlen(pokemonReservadoAAgregar->pokemon) + 1);
			memcpy(pokemonAAgregarConvertido->pokemon,
					pokemonReservadoAAgregar->pokemon,
					strlen(pokemonReservadoAAgregar->pokemon) + 1);
			pokemonAAgregarConvertido->posicion = malloc(
					sizeof(t_posicion_entrenador));
			pokemonAAgregarConvertido->posicion->pos_x =
					pokemonReservadoAAgregar->posicion->pos_x;
			pokemonAAgregarConvertido->posicion->pos_y =
					pokemonReservadoAAgregar->posicion->pos_y;
			//Borro de la lista al pokemon reservado
			sem_wait(&(sem_pokemonesReservados));
			int indice;
			int cant_reservados = list_size(pokemonesReservadosEnElMapa);
			for (int i = 0; i < cant_reservados; i++) {

				t_pokemon_entrenador_reservado* aux =
						((t_pokemon_entrenador_reservado*) list_get(
								pokemonesReservadosEnElMapa, i));

				if (aux == pokemonReservadoAAgregar) {
					indice = i;
				}
			}
			pokemonReservadoAAgregar = list_remove(pokemonesReservadosEnElMapa,
					indice);
			sem_post(&(sem_pokemonesReservados));

			free(pokemonReservadoAAgregar->posicion);
			free(pokemonReservadoAAgregar->pokemon);
			free(pokemonReservadoAAgregar);
			//Agrego el Poke
			if (entrenadorReservador != NULL) {
				//	sem_wait(&(entrenadorReservador->mutex_entrenador));
				agregarPokemon(entrenadorReservador, pokemonAAgregarConvertido);
				//	sem_post(&(entrenadorReservador->mutex_entrenador));
			}
			//printf("PASE AGREGAR POKEMON \n");
			//Muevo el pokemon a la lista global de atrapados
			sem_wait(&(sem_pokemonesGlobalesAtrapados));
			agregarPokemonAGlobalesAtrapados(pokemonAAgregarConvertido);
			sem_post(&(sem_pokemonesGlobalesAtrapados));

			//CAMBIAR ESTADO A RECIBIO OK.
			if (entrenadorReservador != NULL) {
				//sem_wait(&(entrenadorReservador->mutex_entrenador));
				entrenadorReservador->estado_entrenador = RECIBIO_RESPUESTA_OK;
				//sem_post(&(entrenadorReservador->mutex_entrenador));
			}
			//SEMAFORO MUTEX AL ENTRENADOR

		} else {

			//printf("ESTOY DENRO DEL ELSE DE LA RESPUESTA \n");
			//Si entra acá quiere decir que la respuesta fue FAIL y no pudo atrapar, entonces sigue pudiendo atrapar poke
			//PERO OJO! Falta eliminar al pokemon de los reservados porque por algo falló.
			//Borro de la lista al pokemon reservado
			sem_wait(&(sem_pokemonesReservados));
			int indice;
			int cant_pokes = pokemonesReservadosEnElMapa->elements_count;
			for (int i = 0; i < cant_pokes; i++) {

				t_pokemon_entrenador_reservado* aux =
						((t_pokemon_entrenador_reservado*) list_get(
								pokemonesReservadosEnElMapa, i));

				if (mismo_pokemon_entrenador_reservado(aux,
						pokemonReservadoAAgregar)) {
					indice = i;
				}
			}
			pokemonReservadoAAgregar = list_remove(pokemonesReservadosEnElMapa,
					indice);
			sem_post(&(sem_pokemonesReservados));

			if (list_buscar(pokemonesLibresEnElMapa,
					pokemonReservadoAAgregar->pokemon) != NULL) {
				sem_post(&sem_hay_pokemones_mapa);
			}
			free(pokemonReservadoAAgregar->posicion);

			free(pokemonReservadoAAgregar->pokemon);

			free(pokemonReservadoAAgregar);

			//sem_wait(&(entrenadorReservador->mutex_entrenador));
			entrenadorReservador->estado_entrenador = MOVERSE_A_POKEMON;
			//sem_post(&(entrenadorReservador->mutex_entrenador));
		}
//		printf(
//				"YA RECIBIO LA RESPUESTA CAUGHT Y EL ESTADO DEL ENTRENADOR ES; %d \n",
//				entrenadorReservador->estado_entrenador);

	}
	sem_post(&sem_esperar_caught);
}

void procesar_msg_localized(t_msg_localized_team *msg_localized)
{
	//printf("\nRecibí msg_localized ->> pokemon: %s con %d posiciones\n\n",
	//TODO Borrar		msg_localized->pokemon, msg_localized->posiciones->cant_posic);
	int size_pokemon = msg_localized->size_pokemon;
	int id_correlativo = msg_localized->id_correlativo;
	char *pokemon = malloc(size_pokemon);
	memcpy(pokemon, msg_localized->pokemon, size_pokemon);
	t_posiciones_localized *posiciones_localized = malloc(sizeof(t_posiciones_localized));
	posiciones_localized->cant_posic = msg_localized->posiciones->cant_posic;
	posiciones_localized->coordenadas = list_create();
	list_add_all(posiciones_localized->coordenadas, msg_localized->posiciones->coordenadas);
	list_clean(msg_localized->posiciones->coordenadas);
	eliminar_msg_localized_team(msg_localized);
	char meSirveGet = 0;
	int *idAuxGet;
	char* nombreAux;
	char yaMeLlego = 0;
	//FILTRO POR ID CORRELATIVO
	sem_wait(&mutex_idCorrelativosGet);
	int cant_ids_correlativos = idCorrelativosGet->elements_count;
	for (int i = 0; i < cant_ids_correlativos; i++) {
		idAuxGet = (int*) list_get(idCorrelativosGet, i);
		if (*idAuxGet == id_correlativo) {
			meSirveGet = 1;
		}
	}
	sem_post(&mutex_idCorrelativosGet);
	if (meSirveGet == 1) {
		//SI ES CORRELATIVO, ME FIJO QUE NO ME HAYA LLEGADO UN APPEARED O LOCALIZED DE LA MISMA ESPECIE ANTERIORMENTE
		for (int i = 0; i < list_size(pokemonesLlegadosDelBroker); i++) {
			nombreAux = (char*) list_get(pokemonesLlegadosDelBroker, i);
			if (strcmp(nombreAux, pokemon) == 0) {
				yaMeLlego = 1;
			}
		}
		if (yaMeLlego == 0) {
			//LOS SEMAFOROS ESTAN INCLUIDOS EN LA FUNCION
			agregarPokemonesDelLocalized(pokemon, posiciones_localized);
			char* mensajeLocalized = malloc(size_pokemon);
			memcpy(mensajeLocalized, pokemon, size_pokemon);
			pthread_mutex_lock(&mutex_listaPokemonesLlegadosDelBroker);
			list_add(pokemonesLlegadosDelBroker, mensajeLocalized);
			pthread_mutex_unlock(&mutex_listaPokemonesLlegadosDelBroker);
		}
		/////ANALIZAR SI TENEMOS QUE ACTIVAR EL PLANIFICADOR MEDIANO PLAZO PARA IR A ATRAPAR A UN POKEMON
	}
	//eliminar_msg_localized_team(msg_localized);
}


bool mismo_pokemon_entrenador_reservado(t_pokemon_entrenador_reservado *poke1,
		t_pokemon_entrenador_reservado *poke2) {
	bool resultado;
	int diferencia = strcmp(poke1->pokemon, poke2->pokemon);
	resultado = diferencia == 0
			&& poke1->posicion->pos_x == poke2->posicion->pos_x
			&& poke1->posicion->pos_y == poke2->posicion->pos_y
			&& poke1->cantidad == poke2->cantidad
			&& poke1->id_entrenadorReserva == poke2->id_entrenadorReserva;
	return resultado;
}

void atender_gameboy_gc(int *cliente_fd) {
	op_code cod_op;
	if (recv(*cliente_fd, &cod_op, sizeof(op_code), MSG_WAITALL) == -1) {
		cod_op = -1;
	}
	if (cod_op == APPEARED_TEAM) {
		process_msjs_gameboy(cod_op, *cliente_fd, g_logger);
	} else {
		log_error(g_logger, "RECIBI MSJ EQUIVOCADO DEL GAMEBOY");
	}
}

// RECIBE MENSAJES QUE ENVIA el GAMEBOY: APPEARED_POKEMON
void process_msjs_gameboy(op_code cod_op, int cliente_fd, t_log *logger) {
	void* msg;
	switch (cod_op) {
	case APPEARED_TEAM:
		;
		t_msg_appeared_team *msg_appeared = (t_msg_appeared_team*) msg;
		msg_appeared = rcv_msj_appeared_team(cliente_fd, logger);
		g_cnt_msjs_appeared++;
		//TODO Hacer lo que corresponda con el msg_appeared

		if (necesitoPokemon(msg_appeared->pokemon) != 0) {

			char* mensajeAppeared = malloc(strlen(msg_appeared->pokemon) + 1);
			memcpy(mensajeAppeared, msg_appeared->pokemon,
					strlen(msg_appeared->pokemon) + 1);

			pthread_mutex_lock(&mutex_listaPokemonesLlegadosDelBroker);
			list_add(pokemonesLlegadosDelBroker, mensajeAppeared);
			pthread_mutex_unlock(&mutex_listaPokemonesLlegadosDelBroker);

			t_pokemon_entrenador* pokemonAAgregarAlMapa = malloc(
					sizeof(t_pokemon_entrenador));
			pokemonAAgregarAlMapa->cantidad = 1;
			pokemonAAgregarAlMapa->pokemon = malloc(
					strlen(msg_appeared->pokemon) + 1);
			memcpy(pokemonAAgregarAlMapa->pokemon, msg_appeared->pokemon,
					strlen(msg_appeared->pokemon) + 1);
			pokemonAAgregarAlMapa->posicion = malloc(
					sizeof(t_posicion_entrenador));
			pokemonAAgregarAlMapa->posicion->pos_x = msg_appeared->coord->pos_x;
			pokemonAAgregarAlMapa->posicion->pos_y = msg_appeared->coord->pos_y;

			//El codigo orginal de lo que empieza a continuacion hasta antes de que verifique la long de cola
			//era este:
			/*if (necesitoIrAAtraparlo(pokemonAAgregarAlMapa->pokemon)) {
			 sem_post(&sem_hay_pokemones_mapa);
			 }
			 sem_wait(&sem_pokemonesLibresEnElMapa);
			 list_add(pokemonesLibresEnElMapa, pokemonAAgregarAlMapa);
			 sem_post(&sem_pokemonesLibresEnElMapa);*/
			char verificaSiEsNecesarioAtraparlo = 0;
			char yaLoAgregueAlMapa = 0;
			sem_wait(&sem_pokemonesLibresEnElMapa);
			if (list_size(pokemonesLibresEnElMapa) == 0) {
				list_add(pokemonesLibresEnElMapa, pokemonAAgregarAlMapa);
				yaLoAgregueAlMapa = 1;
				sem_post(&sem_hay_pokemones_mapa);
			} else {
				//ESTO LO HAGO PORQUE SI NO SE QUEDA HACIENDO UN DOBLE WAIT DEL SEMAFORO DE POKEMONESLIBRESENELMAPA
				verificaSiEsNecesarioAtraparlo = 1;
			}
			sem_post(&sem_pokemonesLibresEnElMapa);
			if (verificaSiEsNecesarioAtraparlo == 1) {
				if (necesitoIrAAtraparlo(pokemonAAgregarAlMapa->pokemon)) {
					sem_wait(&sem_pokemonesLibresEnElMapa);
					list_add(pokemonesLibresEnElMapa, pokemonAAgregarAlMapa);
					sem_post(&sem_pokemonesLibresEnElMapa);
					yaLoAgregueAlMapa = 1;
					sem_post(&sem_hay_pokemones_mapa);
				}
			}
			if (yaLoAgregueAlMapa == 0) {
				sem_wait(&sem_pokemonesLibresEnElMapa);
				list_add(pokemonesLibresEnElMapa, pokemonAAgregarAlMapa);
				sem_post(&sem_pokemonesLibresEnElMapa);
			}
			if (cantidadDeEntrenadores == queue_size(colaBlockedEntrenadores)) {

				sem_post(&sem_activacionPlanificadorMPlazo);
			}
		}
		enviar_msg_confirmed(cliente_fd, logger);
		eliminar_msg_appeared_team(msg_appeared);
		break;
	}
}

//Función de prueba - Envía un mensaje CATCH_POKEMON para probar la función.
void enviar_catch_de_appeared(t_msg_appeared_team *msg_appeared) {
	int pos_x = msg_appeared->coord->pos_x;
	int pos_y = msg_appeared->coord->pos_y;
	enviar_catch_pokemon_broker(pos_x, pos_y, msg_appeared->pokemon, g_logger,
			1);
}

//Función de prueba - Envía un mensaje GET_POKEMON para probar la función.
void enviar_get_de_appeared(t_msg_appeared_team *msg_appeared) {
	enviar_get_pokemon_broker(msg_appeared->pokemon, g_logger);
}

void enviar_msjs_get_objetivos(void) {
	sem_wait(&sem_mutex_msjs);
	puts("Pokemones Objetivo Global:");
	puts("");
	list_iterate(objetivoGlobalEntrenadores, (void*) print_pokemones_objetivo);
	puts("");
	sem_post(&sem_mutex_msjs);
	if (status_conn_broker == true) {
		puts("Se envía un mensaje GET_POKEMON al BROKER por cada pokemon.");
		puts("");
		list_iterate(objetivoGlobalEntrenadores,
				(void*) enviar_msjs_get_por_clase_de_pokemon);
	}
}

void lanzar_reconexion_broker(t_log *logger) {
	pthread_mutex_init(&mutex_reconexion, NULL);
	sem_wait(&sem_mutex_msjs);
	int thread_status = pthread_create(&tid_reconexion, NULL,
			(void*) funciones_reconexion, NULL);
	if (thread_status != 0) {
		log_error(logger,
				"Error al crear el thread para reconexión con BROKER");
		exit(EXIT_FAILURE);
	} else {
		pthread_detach(tid_reconexion);
	}
}

void funciones_reconexion(void) {
	int intervalo = g_config_team->tiempo_reconexion;
	log_debug(g_logger, "(Intervalo para reconexión con Broker: %d segundos)",
			intervalo);
	sem_post(&sem_mutex_msjs);
	while (1) {
		sleep(intervalo);
		if (!status_conn_broker) {
			iniciar_suscripciones_broker();
			enviar_msjs_get_objetivos();
		}
	}
}

bool codigo_operacion_valido(op_code code_op) {
	return (code_op == CAUGHT_TEAM || code_op == APPEARED_TEAM
			|| code_op == COLA_VACIA || code_op == LOCALIZED_TEAM
			|| code_op == SUSCRIP_END);

}

void iniciar_cnt_msjs(void) {
	g_cnt_msjs_appeared = 0;
	g_cnt_msjs_caught = 0;
	g_cnt_msjs_localized = 0;
}

void iniciar_log_team(void) {
	g_logger = log_create(g_config_team->ruta_log, "TEAM", 1, LOG_LEVEL_TRACE);
}

void leer_config_team(char *path) {
	g_config = config_create(path);
	g_config_team = malloc(sizeof(t_config_team));
	g_config_team->ip_broker = config_get_string_value(g_config, "IP_BROKER");
	g_config_team->puerto_broker = config_get_string_value(g_config,
			"PUERTO_BROKER");
	g_config_team->ip_team = config_get_string_value(g_config, "IP_TEAM");
	g_config_team->puerto_team = config_get_string_value(g_config,
			"PUERTO_TEAM");
	g_config_team->tiempo_reconexion = config_get_int_value(g_config,
			"TIEMPO_RECONEXION");
	g_config_team->retardo_ciclo_cpu = config_get_int_value(g_config,
			"RETARDO_CICLO_CPU");
	g_config_team->algoritmo_planificion = config_get_string_value(g_config,
			"ALGORITMO_PLANIFICACION");
	g_config_team->quantum = config_get_int_value(g_config, "QUANTUM");
	g_config_team->estimacion_inicial = config_get_int_value(g_config,
			"ESTIMACION_INICIAL");
	g_config_team->ruta_log = config_get_string_value(g_config, "RUTA_LOG");
	g_config_team->id_suscriptor = config_get_int_value(g_config,
			"ID_SUSCRIPTOR");
}

void liberar_lista_posiciones(t_list* lista) {
	for (int i = 0; i < list_size(lista); i++) {
		free(list_get(lista, i));
	}

	list_destroy(lista);
}

void liberar_listas(char** lista) {

	int contador = 0;
	while (lista[contador] != NULL ) {
		free(lista[contador]);
		contador++;
	}

	free(lista);
}

int contador_msjs_cola(t_tipo_mensaje cola_suscripcion) {
	int contador_global;
	switch (cola_suscripcion) {
	case APPEARED_POKEMON:
		;
		contador_global = g_cnt_msjs_appeared;
		break;
	case CAUGHT_POKEMON:
		;
		contador_global = g_cnt_msjs_caught;
		break;
	case LOCALIZED_POKEMON:
		;
		contador_global = g_cnt_msjs_localized;
		break;
	}
	return contador_global;
}

////////////----------------------------------------------------------------------------------------------------------------

///Ya leagregué semaforos en los lugares en las que se la llama
t_pokemon_entrenador_reservado* buscarPokemonReservado(int id_Entrenador) {

	t_pokemon_entrenador_reservado* pokemonReservado;

	for (int i = 0; i < list_size(pokemonesReservadosEnElMapa); i++) {

		pokemonReservado = list_get(pokemonesReservadosEnElMapa, i);

		int idAux = pokemonReservado->id_entrenadorReserva;

		if (idAux == id_Entrenador) {

			return pokemonReservado;

		}
	}
	return NULL ;
}

///Ya tiene los sem{aforos afuera
t_entrenador* buscarEntrenadorDeLaReserva(int idEntrenadorBuscado) {
//	t_entrenador *entrenadorBuscado = NULL; //* entrenadorAux = NULL;
//	bool mismo_id(void* trainer) {
//		t_entrenador *entrenador = (t_entrenador*) trainer;
//		bool resultado = entrenador->id == idEntrenadorBuscado;
//		return resultado;
//	}
//	entrenadorBuscado = (t_entrenador*) list_find(
//			colaBlockedEntrenadores->elements, mismo_id);
//	return entrenadorBuscado;
	t_entrenador* entrenadorAux = NULL;
	t_entrenador *entrenadorBuscado = NULL;
	int cant_entrenadores = queue_size(colaBlockedEntrenadores);

	for (int i = 0; i < cant_entrenadores; i++) {

		entrenadorAux = (t_entrenador*) queue_pop(colaBlockedEntrenadores);

		if (idEntrenadorBuscado == entrenadorAux->id) {

			entrenadorBuscado = entrenadorAux;

		}

		queue_push(colaBlockedEntrenadores, entrenadorAux);
	}

	if(entrenadorBuscado == NULL){
		entrenadorBuscado = entrenadorEnEjecucion;

	}
	return entrenadorBuscado;
}
//Ya tiene el semaforo correspondiente afuera
void agregarPokemonAGlobalesAtrapados(t_pokemon_entrenador* pokemon) {

	char loEncontro = 0;

	for (int i = 0; i < list_size(pokemonesAtrapadosGlobal); i++) {
		t_pokemon_entrenador* pokemonABuscar =
				((t_pokemon_entrenador*) list_get(pokemonesAtrapadosGlobal, i));
		if (strcmp(pokemonABuscar->pokemon, pokemon->pokemon) == 0) {
			pokemonABuscar->cantidad++;
			loEncontro = 1;
			free(pokemon->posicion);
			free(pokemon->pokemon);
			free(pokemon);
		}
	}

	//OJO cuando recibamos un poke desde broker, va a tener que asignar pokemones de cantidad 1, por mas que aparezcan varios
	//de la misma especie en una posicion
	if (loEncontro == 0) {
		list_add(pokemonesAtrapadosGlobal, pokemon);
	}

}

void verificarYCambiarEstadoEntrenador(t_entrenador* unEntrenador) {
	t_estado_entrenador estado = unEntrenador->estado_entrenador;
	if (estado == MOVERSE_A_POKEMON) {
		//int probita = tieneDeadlockEntrenador(unEntrenador);
		//unEntrenador->estado_entrenador = MOVERSE_A_POKEMON;
		return;
	} else if (estado != MOVERSE_A_ENTRENADOR && estado != ATRAPAR
			&& estado != INTERCAMBIAR && estado != ESPERAR_CAUGHT) {
		t_list* pokemonesPendiente = pokemonesPendientes(unEntrenador);
		int cantidadPokemonesPendientes = list_size(pokemonesPendiente);
		int cantidadPokemonesObjetivo = cantidadDePokemonesEnLista(
				unEntrenador->objetivoEntrenador);
		int cantidadPokemonesObtenidos = cantidadDePokemonesEnLista(
				unEntrenador->pokemonesObtenidos);
		//OJO NO SE SI ESTA BIEN EL ULTIMO ELSE!!
		if (cantidadPokemonesPendientes == 0) {
			printf(
					"----------------------------------------------------------------------------\n");
			printf("ENTRENADOR %d, VA A EXIT CON %d CICLOS DE CPU \n",
					unEntrenador->id, unEntrenador->ciclosCPU);
			printf(
					"----------------------------------------------------------------------------\n");
			unEntrenador->estado_entrenador = EXIT;
		} else if ((cantidadPokemonesObjetivo == cantidadPokemonesObtenidos)
				&& (tieneDeadlockEntrenador(unEntrenador))) {
			printf(
					"----------------------------------------------------------------------------\n");
			printf("ENTRENADOR %d, TIENE DEADLOCK \n", unEntrenador->id);
			printf(
					"----------------------------------------------------------------------------\n");
			unEntrenador->estado_entrenador = DEADLOCK;
		} else {
			int probita = tieneDeadlockEntrenador(unEntrenador);
			unEntrenador->estado_entrenador = MOVERSE_A_POKEMON;
		}
		liberar_lista_de_pokemones(pokemonesPendiente);
	}
}

//Es un for adentro de un for, donde se busca que la lista de obtenidos sea igual a la del objetivo. Entonces hay que evaluar
//lo que se puede dar: si hay un pokemon distinto chau, hay deadlock. Ahora si son todos iguales pero la cantidad es distinta,
//también. Ahora si tanto la cantidad como el nombre son iguales, entonces todo ok, no tiene dedalock.
//Si tiene deadlock devuelve true 1 y si no tiene, false 0.
int tieneDeadlockEntrenador(t_entrenador* unEntrenador) {
	t_list* poksObjetivo = unEntrenador->objetivoEntrenador;
	t_list* poksObtenidos = unEntrenador->pokemonesObtenidos;

	for (int i = 0; i < list_size(poksObjetivo); i++) {
		t_pokemon_entrenador* unObjetivo = (t_pokemon_entrenador*) list_get(
				poksObjetivo, i);
		t_pokemon_entrenador* unObtenido = (t_pokemon_entrenador*) list_buscar(
				poksObtenidos, unObjetivo->pokemon);
		if (unObtenido == NULL) {
			return 1;
		} else if (unObtenido->cantidad != unObjetivo->cantidad) {
			return 1;
		}
	}
	return 0;
}

void agregarPokemonesDelLocalized(char *pokemon, t_posiciones_localized *posiciones)
{
	for (int i = 0; i < posiciones->cant_posic; i++) {

		t_coordenada* coordenada = ((t_coordenada*) list_get(
				posiciones->coordenadas, i));
		t_pokemon_entrenador* pokemonAAgregarAlMapa = malloc(
				sizeof(t_pokemon_entrenador));
		pokemonAAgregarAlMapa->cantidad = 1;
		pokemonAAgregarAlMapa->pokemon = pokemon;
		pokemonAAgregarAlMapa->posicion = malloc(sizeof(t_posicion_entrenador));
		pokemonAAgregarAlMapa->posicion->pos_x = coordenada->pos_x;
		pokemonAAgregarAlMapa->posicion->pos_y = coordenada->pos_y;
		if (necesitoIrAAtraparlo(pokemonAAgregarAlMapa->pokemon)) {
			sem_post(&sem_hay_pokemones_mapa);
		}
		sem_wait(&sem_pokemonesLibresEnElMapa);
		list_add(pokemonesLibresEnElMapa, pokemonAAgregarAlMapa);
		sem_post(&sem_pokemonesLibresEnElMapa);
	}
	list_destroy_and_destroy_elements(posiciones->coordenadas, (void*) free);
	free(posiciones);
	if (cantidadDeEntrenadores == queue_size(colaBlockedEntrenadores)) {
		sem_post(&sem_activacionPlanificadorMPlazo);
	}
}

int cantidadDePokemonesEnLista(t_list* lista) {

	int valorDeRetorno = 0;

	if (list_size(lista) == 0) {
		return valorDeRetorno;
	}

	for (int i = 0; i < list_size(lista); i++) {
		t_pokemon_entrenador* pokemonAux = list_get(lista, i);
		valorDeRetorno += pokemonAux->cantidad;
	}

	return valorDeRetorno;
}

char necesitoIrAAtraparlo(char* nombrePokemonLlegado) {
	char valorDeRetorno = 0;
	int cantidadEnReservados = 0;
	int cantidadLibres = 0;
	int cantidadEnObjetivoGlobal = 0;
	int cantidadEnAtrapadosGlobal = 0;

	if (necesitoPokemon(nombrePokemonLlegado)) {
		sem_wait(&sem_pokemonesObjetivoGlobal);
		t_pokemon_entrenador* pokEnObjetivoGlobal = list_buscar(
				objetivoGlobalEntrenadores, nombrePokemonLlegado);
		if (pokEnObjetivoGlobal != NULL) {
			cantidadEnObjetivoGlobal = pokEnObjetivoGlobal->cantidad;
		}
		sem_post(&sem_pokemonesObjetivoGlobal);

		sem_wait(&sem_pokemonesGlobalesAtrapados);
		t_pokemon_entrenador* pokEnAtrapadosGlobal = list_buscar(
				pokemonesAtrapadosGlobal, nombrePokemonLlegado);
		if (pokEnAtrapadosGlobal != NULL) {
			cantidadEnAtrapadosGlobal = pokEnAtrapadosGlobal->cantidad;
		}
		sem_post(&sem_pokemonesGlobalesAtrapados);

		sem_wait(&sem_pokemonesLibresEnElMapa);
		for (int i = 0; i < list_size(pokemonesLibresEnElMapa); i++) {
			t_pokemon_entrenador* pokLibre = list_get(pokemonesLibresEnElMapa,
					i);
			if (pokLibre->pokemon == nombrePokemonLlegado) {
				cantidadLibres += pokLibre->cantidad;
			}
		}
		sem_post(&sem_pokemonesLibresEnElMapa);

		sem_wait(&sem_pokemonesReservados);
		for (int i = 0; i < list_size(pokemonesReservadosEnElMapa); i++) {
			t_pokemon_entrenador_reservado* pokReservado = list_get(
					pokemonesReservadosEnElMapa, i);
			if (pokReservado->pokemon == nombrePokemonLlegado) {
				cantidadEnReservados += pokReservado->cantidad;
			}
		}
		sem_post(&sem_pokemonesReservados);

		//printf("Cantidad en reservados es %d \n", cantidadEnReservados);

		//printf("Cantidad en libres es %d \n", cantidadLibres);

		//printf("Cantidad en objetivo global es %d \n",
				//cantidadEnObjetivoGlobal);

		//printf("Cantidad en atrapados es %d \n", cantidadEnAtrapadosGlobal);

		valorDeRetorno = (cantidadEnReservados + cantidadLibres)
				< (cantidadEnObjetivoGlobal - cantidadEnAtrapadosGlobal);

	}

	//printf("ValorDeRetorno es %d \n", valorDeRetorno);

	return valorDeRetorno;
}

int enviar_end_suscripcion_broker_tm(t_tipo_mensaje cola_id, int contador_msgs,
		t_log *logger) { // ------ USAR ESTA FUNCION PARA ENVIAR MENSAJES FIN_SUSCRIPCION AL BROKER ----------//
	pthread_t tid;
	t_handsake_suscript *handshake = malloc(sizeof(t_handsake_suscript));
	handshake->id_suscriptor = g_config_team->id_suscriptor;
	handshake->msjs_recibidos = contador_msgs;
	handshake->id_recibido = handshake->msjs_recibidos;
	handshake->cola_id = cola_id;
	pthread_mutex_lock(&g_mutex_mensajes);

	int thread_status = pthread_create(&tid, NULL,
			(void*) connect_broker_y_enviar_end_suscript_tm, (void*) handshake);
	if (thread_status != 0) {
		log_error(logger, "Thread create returned %d | %s", thread_status,
				strerror(thread_status));

	} else {
		pthread_detach(tid);
	}
	return thread_status;
}

void connect_broker_y_enviar_end_suscript_tm(t_handsake_suscript *handshake) {
	int id_mensaje = -1;
	char *ip = g_config_team->ip_broker;
	char *puerto = g_config_team->puerto_broker;
	char *proceso = "BROKER";
	char *name_cola = nombre_cola(handshake->cola_id);
	int cliente_fd = crear_conexion(ip, puerto, g_logger, proceso, name_cola);
	if (cliente_fd >= 0) {
		enviar_solicitud_fin_suscripcion(cliente_fd, g_logger, handshake);
		op_code codigo_operacion = rcv_codigo_operacion(cliente_fd);
		if (codigo_operacion == MSG_CONFIRMED) {
			rcv_msg_confirmed(cliente_fd, g_logger);
		} else if (codigo_operacion == MSG_ERROR) {
			void *a_recibir = recibir_buffer(cliente_fd, &id_mensaje);
			log_warning(g_logger, "(RECEIVING: |%s)", a_recibir);
			free(a_recibir);
		}
	}
	pthread_mutex_unlock(&g_mutex_mensajes);
	close(cliente_fd);
	free(handshake);
}

void manejo_senial_externa_tm(void) {
	pthread_mutex_lock(&g_mutex_mensajes);
	int team_pid = process_get_thread_id();

	log_trace(g_logger,
			"Para finalizar Suscripción -->> Enviar Señales ''kill SIGUSR1/2 '' al proceso (PID):%d",
			team_pid);

	puts("");
	signal(SIGUSR1, funcion_captura_senial_tm);
	signal(SIGUSR2, funcion_captura_senial_tm);
	pthread_mutex_unlock(&g_mutex_mensajes);
}

void funcion_captura_senial_tm(int senial) {
	int gamecard_pid = process_get_thread_id();
	puts("");
	log_warning(g_logger, "Señal recibida: %s -->>(kill -%d  %d).",
			senial_recibida_tm(senial), senial, gamecard_pid);

	puts("");
	switch (senial) {
	case SIGUSR1:
		;

		finalizar_suscripciones_team(senial);
		break;
	case SIGUSR2:
		;

		finalizar_suscripciones_team(senial);
		break;
	default:
		puts("-->>No es posible manejar señal enviada.<<--");
		break;
	}
}

void finalizar_suscripciones_team(int senial) // -->> Si la usan desde el código: usar 10 o 12 como argumento <<--
{ // ------ USAR ESTA FUNCION PARA ENVIAR MENSAJES FIN_SUSCRIPCION AL BROKER ----------//
	if (senial == SIGUSR1 || senial == SIGUSR2) {
		enviar_end_suscripcion_broker_tm(APPEARED_POKEMON, g_cnt_msjs_appeared,
				g_logger);

		enviar_end_suscripcion_broker_tm(LOCALIZED_POKEMON,
				g_cnt_msjs_localized, g_logger);

		enviar_end_suscripcion_broker_tm(CAUGHT_POKEMON, g_cnt_msjs_caught,
				g_logger);

	}
}

char *senial_recibida_tm(int senial) {
	char *recvd_signal;
	switch (senial) {
	case SIGUSR1:
		;

		recvd_signal = "SIGUSR1";
		break;
	case SIGUSR2:
		;

		recvd_signal = "SIGUSR2";
		break;
	}
	return recvd_signal;
}

char esFifo() {
	if (strcmp(g_config_team->algoritmo_planificion, "FIFO") == 0) {
		return 1;
	}
	return 0;
}

char esRR() {
	if (strcmp(g_config_team->algoritmo_planificion, "RR")) {
		return 1;
	}
	return 0;
}
