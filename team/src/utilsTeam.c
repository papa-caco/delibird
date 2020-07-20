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
	t_msg_get_broker *msj_get = malloc(sizeof(t_msg_get_broker));
	msj_get->size_pokemon = strlen(pokemon) + 1;
	msj_get->pokemon = malloc(sizeof(msj_get->size_pokemon));
	memcpy(msj_get->pokemon, pokemon, msj_get->size_pokemon);
	sem_wait(&sem_mutex_msjs);
	int thread_status = pthread_create(&tid_send_get, NULL,
			(void*) connect_broker_y_enviar_mensaje_get, (void*) msj_get);
	if (thread_status != 0) {
		log_error(logger, "Thread create returned %d | %s", thread_status,
				strerror(thread_status));
	} else {
		pthread_detach(tid_send_get);
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
	close(cliente_fd);
	pthread_exit(&tid_send_get);
	eliminar_msg_get_broker(msg_get);

	sem_wait(&mutex_idCorrelativosGet);
	list_add(idCorrelativosGet, &id_mensaje);
	sem_post(&mutex_idCorrelativosGet);

	return id_mensaje;
}

// ------ USAR ESTA FUNCION PARA ENVIAR MENSAJES CATCH_POKEMON AL BROKER ----------//
int enviar_catch_pokemon_broker(int pos_x, int pos_y, char* pokemon,
		t_log *logger, int id_entrenador) {
	t_msg_catch_broker *msg_catch = malloc(sizeof(t_msg_catch_broker));
	msg_catch->coordenada = malloc(sizeof(t_coordenada));
	msg_catch->coordenada->pos_x = pos_x;
	msg_catch->coordenada->pos_y = pos_y;
	msg_catch->size_pokemon = strlen(pokemon) + 1;
	msg_catch->pokemon = malloc(msg_catch->size_pokemon);
	memcpy(msg_catch->pokemon, pokemon, msg_catch->size_pokemon);
	t_mensaje_Caugth_and_IdEntrenador* args;
	args->msg_catch_broker = msg_catch;
	args->id_entrenador = id_entrenador;
	int thread_status = pthread_create(&tid_send_catch, NULL,
			(void*) connect_broker_y_enviar_mensaje_catch, (void*) args);
	if (thread_status != 0) {
		log_error(logger, "Thread create returned %d | %s", thread_status,
				strerror(thread_status));
	} else {
		pthread_detach(tid_send_catch);
	}
	return thread_status;
}

int connect_broker_y_enviar_mensaje_catch(
		t_mensaje_Caugth_and_IdEntrenador* args) {
	char *ip = g_config_team->ip_broker;
	char *puerto = g_config_team->puerto_broker;
	char *proceso = "BROKER";
	char *name_cola = nombre_cola(CATCH_POKEMON);
	sem_wait(&sem_mutex_msjs);
	int cliente_fd = crear_conexion(ip, puerto, g_logger, proceso, name_cola);
	enviar_msj_catch_broker(cliente_fd, g_logger, args->msg_catch_broker);
	int id_mensaje = -1;
	if (rcv_codigo_operacion(cliente_fd) == ID_MENSAJE) {
		id_mensaje = rcv_id_mensaje(cliente_fd, g_logger);
		//TODO Guardar ID_MENSAJE que envia el BROKER de CATCH_POKEMON
	}

	sem_post(&sem_mutex_msjs);
	close(cliente_fd);
	eliminar_msg_catch_broker(args->msg_catch_broker);
	pthread_exit(&tid_send_catch);

	t_id_Correlativo_and_Entrenador* ids = malloc(
			sizeof(t_id_Correlativo_and_Entrenador));
	ids->id_Correlativo = id_mensaje;
	ids->id_Entrenador = args->id_entrenador;

	sem_wait(&mutex_idCorrelativos);
	list_add(idCorrelativosCatch, &ids);
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
	uint32_t id_recibido;
	void *msg;
	switch (codigo_operacion) {
	case COLA_VACIA:
		;
		id_recibido = rcv_msj_cola_vacia(socket_cliente, logger);
		if (id_recibido != g_config_team->id_suscriptor) {
			log_error(logger, "MSG_ERROR");
		}
		break;
	case APPEARED_TEAM:
		;
		t_msg_appeared_team *msg_appeared = (t_msg_appeared_team*) msg;
		msg_appeared = rcv_msj_appeared_team(socket_cliente, logger);
		g_cnt_msjs_appeared++;
		//TODO Agregar la función que corresponda y tome al msg_appeared
		id_recibido = msg_appeared->id_mensaje;

		//meSirvePokemon ya tiene semaforos dentro
		//if (meSirvePokemon(msg_appeared->pokemon)) {
		//Las dos líneas anteriores las comento por las dudas de que en realidad al final sí haya que realizar ese tipo de
		//validació


		//necesitoPokemon ya tiene los semáforos adentro
		if (necesitoPokemon(msg_appeared->pokemon) != 0) {

			pthread_mutex_lock(&mutex_listaPokemonesLlegadosDelBroker);
			list_add(pokemonesLlegadosDelBroker, msg_appeared->pokemon);
			pthread_mutex_unlock(&mutex_listaPokemonesLlegadosDelBroker);

			t_pokemon_entrenador* pokemonAAgregarAlMapa = malloc(sizeof(t_pokemon_entrenador));
			pokemonAAgregarAlMapa->cantidad = 1;
			pokemonAAgregarAlMapa->pokemon = msg_appeared->pokemon;
			pokemonAAgregarAlMapa->posicion = malloc(sizeof(t_posicion_entrenador));
			pokemonAAgregarAlMapa->posicion->pos_x =msg_appeared->coord->pos_x;
			pokemonAAgregarAlMapa->posicion->pos_y =msg_appeared->coord->pos_y;

			sem_wait(&sem_pokemonesLibresEnElMapa);
			list_add(pokemonesLibresEnElMapa, pokemonAAgregarAlMapa);
			sem_post(&sem_pokemonesLibresEnElMapa);

			//sem_post(&sem_hay_pokemones_mapa);

			if(cantidadDeEntrenadores == queue_size(colaNewEntrenadores)){

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
		break;
	case CAUGHT_TEAM:
		;
		t_msg_caught_team *msg_caught = (t_msg_caught_team*) msg;
		msg_caught = rcv_msj_caught_team(socket_cliente, logger);
		g_cnt_msjs_caught++;
		id_recibido = msg_caught->id_mensaje;

		//FIltramos los ids que nos corresponden por id correlativo
		t_id_Correlativo_and_Entrenador* idAux;
		char meSirve = 0;

		sem_wait(&mutex_idCorrelativos);

		for (int i = 0; i < list_size(idCorrelativosCatch); i++) {

			idAux = (t_id_Correlativo_and_Entrenador*) list_get(
					idCorrelativosCatch, i);

			if (idAux->id_Correlativo == id_recibido) {

				meSirve = 1;
			}
		}
		sem_post(&mutex_idCorrelativos);

		if (meSirve == 1) {
			//Busco al entrenador que hizo la reserva
			sem_wait(&(sem_cola_blocked));
			t_entrenador* entrenadorReservador = buscarEntrenadorDeLaReserva(
					idAux->id_Entrenador);
			sem_post(&(sem_cola_blocked));

			//Busco el pokemon que corresponde al entrenador, es decir, el que él reservó
			sem_wait(&(sem_pokemonesReservados));
			t_pokemon_entrenador_reservado* pokemonReservadoAAgregar =
					buscarPokemonReservado(idAux->id_Entrenador);
			sem_post(&(sem_pokemonesReservados));

			if (msg_caught->resultado == OK) {

				//AGREGAR POKEMON A LA LISTA DE ENTRENADORES DEL POKEMON

				//Transformo el pokemonReservado al tipo pokemon_entrenador
				t_pokemon_entrenador* pokemonAAgregarConvertido = malloc(
						sizeof(t_pokemon_entrenador));
				pokemonAAgregarConvertido->cantidad =
						pokemonReservadoAAgregar->cantidad;
				pokemonAAgregarConvertido->pokemon =
						pokemonReservadoAAgregar->pokemon;
				pokemonAAgregarConvertido->posicion = malloc(
						sizeof(t_posicion_entrenador));
				pokemonAAgregarConvertido->posicion->pos_x =
						pokemonReservadoAAgregar->posicion->pos_x;
				pokemonAAgregarConvertido->posicion->pos_y =
						pokemonReservadoAAgregar->posicion->pos_y;

				//Borro de la lista al pokemon reservado
				sem_wait(&(sem_pokemonesReservados));
				int indice;
				for (int i = 0; i < list_size(pokemonesReservadosEnElMapa);
						i++) {

					t_pokemon_entrenador_reservado* aux =
							((t_pokemon_entrenador_reservado*) list_get(
									pokemonesReservadosEnElMapa, i));

					if (aux == pokemonReservadoAAgregar) {
						indice = i;
					}
				}
				pokemonReservadoAAgregar = list_remove(
						pokemonesReservadosEnElMapa, indice);
				sem_post(&(sem_pokemonesReservados));

				free(pokemonReservadoAAgregar->posicion);
				free(pokemonReservadoAAgregar);

				//Agrego el Poke usano una funcion de nombre rancio que hace LO MISMO que la de entrenador.c
				sem_wait(&(entrenadorReservador->mutex_entrenador));
				agregarPokemon(entrenadorReservador, pokemonAAgregarConvertido);
				sem_post(&(entrenadorReservador->mutex_entrenador));

				//Muevo el pokemon a la lista global de atrapados
				sem_wait(&(sem_pokemonesGlobalesAtrapados));
				agregarPokemonAGlobalesAtrapados(pokemonAAgregarConvertido);
				sem_post(&(sem_pokemonesGlobalesAtrapados));


				//CAMBIAR ESTADO A RECIBIO OK.

				sem_wait(&(entrenadorReservador->mutex_entrenador));
				entrenadorReservador->estado_entrenador = RECIBIO_RESPUESTA_OK;
				sem_post(&(entrenadorReservador->mutex_entrenador));


				//SEMAFORO MUTEX AL ENTRENADOR

			} else {
				//Si entra acá quiere decir que la respuesta fue FAIL y no pudo atrapar, entonces sigue pudiendo atrapar poke
				//PERO OJO! Falta eliminar al pokemon de los reservados porque por algo falló.
				//Borro de la lista al pokemon reservado
				sem_wait(&(sem_pokemonesReservados));
				int indice;
				for (int i = 0; i < list_size(pokemonesReservadosEnElMapa);
						i++) {

					t_pokemon_entrenador_reservado* aux =
							((t_pokemon_entrenador_reservado*) list_get(
									pokemonesReservadosEnElMapa, i));

					if (aux == pokemonReservadoAAgregar) {
						indice = i;
					}
				}
				pokemonReservadoAAgregar = list_remove(
						pokemonesReservadosEnElMapa, indice);
				sem_post(&(sem_pokemonesReservados));

				free(pokemonReservadoAAgregar->posicion);
				free(pokemonReservadoAAgregar);

				sem_wait(&(entrenadorReservador->mutex_entrenador));
				entrenadorReservador->estado_entrenador = MOVERSE_A_POKEMON;
				sem_post(&(entrenadorReservador->mutex_entrenador));
			}

		}

		free(msg_caught);
		break;
	case LOCALIZED_TEAM:
		;
		t_msg_localized_team *msg_localized = (t_msg_localized_team*) msg;
		msg_localized = rcv_msj_localized_team(socket_cliente, logger);
		g_cnt_msjs_localized++;
		id_recibido = msg_localized->id_mensaje;


		char meSirveGet = 0;
		int idAuxGet = 0;
		char* nombreAux;
		char yaMeLlego = 0;

		//FILTRO POR ID CORRELATIVO

		sem_wait(&mutex_idCorrelativosGet);

		for (int i = 0; i < list_size(idCorrelativosGet); i++) {

			idAuxGet = (int) list_get(idCorrelativosCatch, i);

			if (idAuxGet == id_recibido) {

				meSirveGet = 1;
			}
		}
		sem_post(&mutex_idCorrelativosGet);



		if(meSirveGet == 1){

			//SI ES CORRELATIVO, ME FIJO QUE NO ME HAYA LLEGADO UN APPEARED O LOCALIZED DE LA MISMA ESPECIE ANTERIORMENTE

			for (int i = 0; i < list_size(pokemonesLlegadosDelBroker); i++) {

						nombreAux = (char*) list_get(pokemonesLlegadosDelBroker, i);

						if (nombreAux == msg_localized->pokemon) {

							yaMeLlego = 1;



						}
					}

			if(yaMeLlego == 0){

				//LOS SEMAFOROS ESTAN INCLUIDOS EN LA FUNCION
				agregarPokemonesDelLocalized(msg_localized);
			}

			/////ANALIZAR SI TENEMOS QUE ACTIVAR EL PLANIFICADOR MEDIANO PLAZO PARA IR A ATRAPAR A UN POKEMON

		}

		eliminar_msg_localized_team(msg_localized);
		break;
	case 0:
		pthread_exit(NULL);
	case -1:
		pthread_exit(NULL);
	}
	return id_recibido;
}

void atender_gameboy(int *cliente_fd) {
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


			pthread_mutex_lock(&mutex_listaPokemonesLlegadosDelBroker);

					list_add(pokemonesLlegadosDelBroker, msg_appeared->pokemon);
					pthread_mutex_unlock(&mutex_listaPokemonesLlegadosDelBroker);


					t_pokemon_entrenador* pokemonAAgregarAlMapa = malloc(sizeof(t_pokemon_entrenador));
					pokemonAAgregarAlMapa->cantidad = 1;
					pokemonAAgregarAlMapa->pokemon = malloc(strlen(msg_appeared->pokemon)+1);
					memcpy(pokemonAAgregarAlMapa->pokemon, msg_appeared->pokemon, strlen(msg_appeared->pokemon)+1);
					pokemonAAgregarAlMapa->posicion = malloc(sizeof(t_posicion_entrenador));
					pokemonAAgregarAlMapa->posicion->pos_x =msg_appeared->coord->pos_x;
					pokemonAAgregarAlMapa->posicion->pos_y =msg_appeared->coord->pos_y;



					sem_wait(&sem_pokemonesLibresEnElMapa);
					list_add(pokemonesLibresEnElMapa, pokemonAAgregarAlMapa);
					sem_post(&sem_pokemonesLibresEnElMapa);


					if(cantidadDeEntrenadores == queue_size(colaNewEntrenadores)){

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
			|| code_op == LOCALIZED_TEAM || code_op == SUSCRIP_END);
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
	while (lista[contador] != NULL) {
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


	for (int i = 0; i< list_size(pokemonesReservadosEnElMapa); i++) {

		pokemonReservado = list_get(pokemonesReservadosEnElMapa, i);

		printf("POKEMON RESERVADO ES %s \n ", pokemonReservado->pokemon);

		int idAux = pokemonReservado->id_entrenadorReserva;

		if (idAux == id_Entrenador) {

			printf("ENTRO AL IF Y SU POKEMON RESERVADO ES %s \n ", pokemonReservado->pokemon);
			return pokemonReservado;

		}
	}
	return NULL;
}

///Ya tiene los sem{aforos afuera
t_entrenador* buscarEntrenadorDeLaReserva(int idEntrenadorBuscado) {

	t_entrenador* entrenadorAux;
	t_entrenador* entrenadorBuscado;

	for (int i = 0; i < queue_size(colaBlockedEntrenadores); i++) {

		entrenadorAux = (t_entrenador*) queue_pop(colaBlockedEntrenadores);

		if (idEntrenadorBuscado == entrenadorAux->id) {

			entrenadorBuscado = entrenadorAux;

		}

		queue_push(colaBlockedEntrenadores, entrenadorAux);

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

	if( estado != MOVERSE_A_ENTRENADOR && estado!= ATRAPAR && estado!= INTERCAMBIAR && estado!= ESPERAR_CAUGHT){

		t_list* pokemonesPendiente = pokemonesPendientes(unEntrenador);
			int cantidadPokemonesPendientes = list_size(pokemonesPendiente);
			int cantidadPokemonesObjetivo = list_size(unEntrenador->objetivoEntrenador);
			int cantidadPokemonesObtenidos = list_size(unEntrenador->pokemonesObtenidos);

			//OJO NO SE SI ESTA BIEN EL ULTIMO ELSE!!!
			if (cantidadPokemonesPendientes == 0) {
				printf("ENTRENADOR %d, VA A EXIT \n", unEntrenador->id);
				unEntrenador->estado_entrenador = EXIT;
			} else if ((cantidadPokemonesObjetivo == cantidadPokemonesObtenidos) && (tieneDeadlockEntrenador(unEntrenador))) {
				unEntrenador->estado_entrenador = DEADLOCK;
			} else {
				unEntrenador->estado_entrenador = MOVERSE_A_POKEMON;
			}


			liberar_lista_de_pokemones(pokemonesPendiente);
	}



}

//Es un for adentro de un for, donde se busca que la lista de obtenidos sea igual a la del objetivo. Entonces hay que evaluar
//lo que se puede dar: si hay un pokemon distinto chau, hay deadlock. Ahora si son todos iguales pero la cantidad es distinta,
//también. Ahora si tanto la cantidad como el nombre son iguales, entonces todo ok, no tiene dedalock.
//Si tiene deadlock devuelve true 1 y si no tiene, false 0.
char tieneDeadlockEntrenador(t_entrenador* unEntrenador){
	t_list* poksObjetivo = unEntrenador->objetivoEntrenador;
	t_list* poksObtenidos = unEntrenador->pokemonesObtenidos;

	for (int i=0; i < list_size(poksObjetivo); i++){
		t_pokemon_entrenador* unObjetivo = (t_pokemon_entrenador*) list_get(poksObjetivo, i);
		t_pokemon_entrenador* unObtenido = (t_pokemon_entrenador*) list_buscar(poksObtenidos, unObjetivo->pokemon);
		if(unObtenido == NULL){
			return 1;
		}
		else if(unObtenido->cantidad!= unObjetivo->cantidad){
			return 1;
		}
	}
	return 0;
}

void agregarPokemonesDelLocalized(t_msg_localized_team* mensajeLocalized){

	t_posiciones_localized* posiciones = mensajeLocalized->posiciones;

	for (int i = 0; i < posiciones->cant_posic; i++) {

		t_coordenada* coordenada = ((t_coordenada*) list_get(posiciones->coordenadas, i));

		t_pokemon_entrenador* pokemonAAgregarAlMapa = malloc(sizeof(t_pokemon_entrenador));
		pokemonAAgregarAlMapa->cantidad = 1;
		pokemonAAgregarAlMapa->pokemon = mensajeLocalized->pokemon;
		pokemonAAgregarAlMapa->posicion = malloc(sizeof(t_posicion_entrenador));
		pokemonAAgregarAlMapa->posicion->pos_x =coordenada->pos_x;
		pokemonAAgregarAlMapa->posicion->pos_y =coordenada->pos_y;

		sem_wait(&sem_pokemonesLibresEnElMapa);
		list_add(pokemonesLibresEnElMapa, pokemonAAgregarAlMapa);
		sem_post(&sem_pokemonesLibresEnElMapa);



			}

		if(cantidadDeEntrenadores == queue_size(colaNewEntrenadores)){

							sem_post(&sem_activacionPlanificadorMPlazo);
						}
}

