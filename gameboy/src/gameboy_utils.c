/*
 *		utils.c
 *
 *  	Created on: 10 apr. 2020
 *
 *      Author:  Grupo "tp-2020-1C Los Que Aprueban"
 *
 *
 */

#include "gameboy_utils.h"

void construir_mensaje(t_mensaje_gameboy *argumentos_mensaje, t_list *lista) {
	char *proceso = list_get(lista, 0);
	char *tipo_mensaje = list_get(lista, 1);

	if (strcmp(proceso, "SUSCRIPTOR") == 0) {
		argumentos_mensaje->proceso = SUSCRIPTOR;
	} else if (strcmp(proceso, "BROKER") == 0) {
		argumentos_mensaje->proceso = BROKER;
	} else if (strcmp(proceso, "GAMECARD") == 0) {
		argumentos_mensaje->proceso = GAMECARD;
	} else if (strcmp(proceso, "TEAM") == 0) {
		argumentos_mensaje->proceso = TEAM;
	} else {
		argumentos_mensaje->proceso = UNKNOWN_PROC;
		argumentos_mensaje->tipo_mensaje = UNKNOWN_QUEUE;
		list_clean(argumentos_mensaje->argumentos);
		exit(EXIT_SUCCESS);
	}
	argumentos_mensaje->tipo_mensaje = select_tipo_mensaje(tipo_mensaje);
	cargar_argumentos(argumentos_mensaje, lista);
	list_destroy(lista);
}

t_tipo_mensaje select_tipo_mensaje(char * valor) {
	if (strcmp(valor, "NEW_POKEMON") == 0) {
		return NEW_POKEMON;
	} else if (strcmp(valor, "APPEARED_POKEMON") == 0) {
		return APPEARED_POKEMON;
	} else if (strcmp(valor, "CATCH_POKEMON") == 0) {
		return CATCH_POKEMON;
	} else if (strcmp(valor, "CAUGHT_POKEMON") == 0) {
		return CAUGHT_POKEMON;
	} else if (strcmp(valor, "GET_POKEMON") == 0) {
		return GET_POKEMON;
	} else if (strcmp(valor, "LOCALIZED_POKEMON") == 0) {
		return LOCALIZED_POKEMON;
	} else {
		return UNKNOWN_QUEUE;
	}
}

void cargar_argumentos(t_mensaje_gameboy *argumentos_mensaje, t_list *lista) {
	borrar_comienzo_lista(lista, 2);
	list_add_all(argumentos_mensaje->argumentos, lista);
}

bool validar_argumentos(t_mensaje_gameboy *argumentos_mensaje) {
	t_proceso proceso = argumentos_mensaje->proceso;
	int tipo_mensaje = argumentos_mensaje->tipo_mensaje;
	int cant_argumentos = argumentos_mensaje->argumentos->elements_count;
	int resultado;
	if (proceso == SUSCRIPTOR && tipo_mensaje != UNKNOWN_QUEUE
		&& cant_argumentos == 1) {
	if (validar_tiempo(argumentos_mensaje->argumentos) == 1) {
		resultado = 1;
	} else {
		resultado = 0;
	}
		return resultado;
	} else {
	 switch (tipo_mensaje) {
		case UNKNOWN_QUEUE:
			resultado = 0;
			break;
		case NEW_POKEMON:
			if (proceso == BROKER && validar_coordXY(argumentos_mensaje->argumentos, 1, 3) == 1
				&& (cant_argumentos == 4) == 1) {
				resultado = 1;
			} else if (proceso == GAMECARD && (cant_argumentos == 5)
					&& validar_coordXY(argumentos_mensaje->argumentos, 1, 3) == 1
					&& validar_id_mensaje(argumentos_mensaje->argumentos, 4) == 1) {
				resultado = 1;
			} else {
				resultado = 0;
			}
			break;
		case APPEARED_POKEMON:
			if (proceso == BROKER && cant_argumentos == 4
					&& validar_coordXY(argumentos_mensaje->argumentos, 1, 2) == 1
					&& validar_id_mensaje(argumentos_mensaje->argumentos, 3)
							== 1) {
				resultado = 1;
			} else if (proceso == TEAM && cant_argumentos == 3
					&& validar_coordXY(argumentos_mensaje->argumentos, 1, 2) == 1) {
				resultado = 1;
			} else {
				resultado = 0;
			}
			break;
		case CATCH_POKEMON:
			if (proceso == BROKER && cant_argumentos == 3
					&& validar_coordXY(argumentos_mensaje->argumentos, 1, 2) == 1) {
				resultado = 1;
			} else if (proceso == GAMECARD && (cant_argumentos == 4)
					&& validar_coordXY(argumentos_mensaje->argumentos, 1, 2) == 1
					&& validar_id_mensaje(argumentos_mensaje->argumentos, 3) == 1) {
				resultado = 1;
			} else {
				resultado = 0;
			}
			break;
		case CAUGHT_POKEMON:
			if (proceso == BROKER && cant_argumentos == 2
					&& validar_id_mensaje(argumentos_mensaje->argumentos, 0) == 1
					&& validar_resultado_caught(argumentos_mensaje->argumentos, 1) == 1) {
				resultado = 1;
			} else {
				resultado = 0;
			}
			break;
		case GET_POKEMON:
			if (proceso == BROKER && cant_argumentos == 1) {
				resultado = 1;
			} else if (proceso == GAMECARD && (cant_argumentos == 2)
					&& validar_id_mensaje(argumentos_mensaje->argumentos, 1) == 1) {
				resultado = 1;
			}else {
				resultado = 0;
			}
			break;
		case LOCALIZED_POKEMON:
			resultado = 0;
			break;
		}
	}
	return resultado;
}

bool validar_tiempo(t_list *lista) {
	int resultado;
	char *tiempo = list_get(lista, 0);
	int tiempo_max = g_config_gameboy->time_suscrip_max;
	if (atoi(tiempo) > 0 && atoi(tiempo) <= tiempo_max) {
		resultado = 1;
	} else {
		resultado = 0;
	}
	return resultado;
}

bool validar_coordXY(t_list *lista, int index1, int index2) {
	int resultado;
	char *coordX = list_get(lista, index1);
	if (atoi(coordX) >= 0 && atoi(coordX) <= g_config_gameboy->coord_x_max) {
		if (index2 == 2) {
			char *coordY = list_get(lista, index2);
			if (atoi(coordY) >= 0
					&& atoi(coordY) <= g_config_gameboy->coord_y_max) {
				resultado = 1;
			} else {
				resultado = 0;
			}
		} else {
			char *coordY = list_get(lista, index2 - 1);
			char *cantPokemon = list_get(lista, index2);
			if (atoi(coordY) >= 0
					&& atoi(coordY) <= g_config_gameboy->coord_y_max
					&& atoi(cantPokemon) > 0
					&& atoi(cantPokemon)
							<= g_config_gameboy->cant_max_pokemon) {
				resultado = 1;
			} else {
				resultado = 0;
			}
		}
	} else {
		resultado = 0;
	}
	return resultado;
}

bool validar_id_mensaje(t_list *lista, int index) {
	int resultado;
	char *id_mensaje = list_get(lista, index);
	if (atoi(id_mensaje) > 0 && atoi(id_mensaje) <= 9999) {
		resultado = 1;
	} else {
		resultado = 0;
	}
	return resultado;
}

bool validar_resultado_caught(t_list *lista, int index) {
	int resultado;
	char *argumento = list_get(lista, index);
	if (strcmp(argumento, "OK") == 0 || strcmp(argumento, "FAIL") == 0) {
		resultado = 1;
	} else {
		resultado = 0;
	}
	return resultado;
}

t_result_caught codificar_resultado_caught(char *valor) {
	if (strcmp(valor, "OK") == 0) {
		return OK;
	} else {
		return FAIL;
	}
}

int realizar_conexion(t_mensaje_gameboy *msg_gameboy) {
	struct addrinfo hints;
	struct addrinfo *server_info;
	char *ip = select_ip_proceso(msg_gameboy);
	char *puerto = select_puerto_proceso(msg_gameboy);
	char *proceso = obtengo_proceso(msg_gameboy);
	char *cola = obtengo_cola(msg_gameboy);
	int socket_cliente = crear_conexion(ip, puerto, g_logger, proceso, cola);
	return socket_cliente;
}

int conexion_broker(void) {
	char *ip = g_config_gameboy->ip_broker;
	char *puerto = g_config_gameboy->puerto_broker;
	char *proceso = "BROKER";
	char *cola = nombre_cola(g_suscript_queue);
	int socket_cliente = crear_conexion(ip, puerto, g_logger, proceso, cola);
	return socket_cliente;
}

char* select_ip_proceso(t_mensaje_gameboy *msg_gameboy) {
	int proceso = msg_gameboy->proceso;
	char *ip;
	switch (proceso) {
	case SUSCRIPTOR:
		ip = g_config_gameboy->ip_broker;
		break;
	case BROKER:
		ip = g_config_gameboy->ip_broker;
		break;
	case GAMECARD:
		ip = g_config_gameboy->ip_gamecard;
		break;
	case TEAM:
		ip = g_config_gameboy->ip_team;
		break;
	}
	return ip;
}

char* select_puerto_proceso(t_mensaje_gameboy *msg_gameboy) {
	int proceso = msg_gameboy->proceso;
	char *puerto;
	switch (proceso) {
	case SUSCRIPTOR:
		puerto = g_config_gameboy->puerto_broker;
		break;
	case BROKER:
		puerto = g_config_gameboy->puerto_broker;
		break;
	case GAMECARD:
		puerto = g_config_gameboy->puerto_gamecard;
		break;
	case TEAM:
		puerto = g_config_gameboy->puerto_team;
		break;
	}
	return puerto;
}

char *obtengo_proceso(t_mensaje_gameboy *msg_gameboy) {
	int cod_proceso = msg_gameboy->proceso;
	char *proceso;
	switch (cod_proceso) {
	case SUSCRIPTOR:
		proceso = "BROKER";
		break;
	case BROKER:
		proceso = "BROKER";
		break;
	case GAMEBOY:
		proceso = "GAMEBOY";
		break;
	case GAMECARD:
		proceso = "GAMECARD";
		break;
	case TEAM:
		proceso = "TEAM";
		break;
	}
	return proceso;
}

char *obtengo_cola(t_mensaje_gameboy *msg_gameboy) {
	int cod_cola = msg_gameboy->tipo_mensaje;
	char *cola;
	switch (cod_cola) {
	case APPEARED_POKEMON:
		cola = "APPEARED_POKEMON";
		break;
	case CATCH_POKEMON:
		cola = "CATCH_POKEMON";
		break;
	case CAUGHT_POKEMON:
		cola = "CAUGHT_POKEMON";
		break;
	case GET_POKEMON:
		cola = "GET_POKEMON";
		break;
	case LOCALIZED_POKEMON:
		cola = "LOCALIZED_POKEMON";
		break;
	case NEW_POKEMON:
		cola = "NEW_POKEMON";
		break;
	}
	return cola;
}

int get_time_suscripcion(t_mensaje_gameboy *msg_gameboy) {
	char *tiempo_ingresado = list_get(msg_gameboy->argumentos,0);
	int tiempo = atoi(tiempo_ingresado);
	return tiempo;
}

void enviar_mensaje_gameboy(t_mensaje_gameboy *msg_gameboy, int socket_cliente) {
	int proceso = msg_gameboy->proceso;
	int tipo_mensaje = msg_gameboy->tipo_mensaje;
	switch (tipo_mensaje) {
	case CATCH_POKEMON:
		if (proceso == BROKER) {
			send_msg_catch_broker(msg_gameboy, socket_cliente, g_logger);
		} else {
			send_msg_catch_gamecard(msg_gameboy, socket_cliente, g_logger);
		}
		break;
	case CAUGHT_POKEMON:
		send_msg_caught_broker(msg_gameboy,socket_cliente, g_logger);
		break;
	case GET_POKEMON:
		if (proceso == BROKER) {
			send_msg_get_broker(msg_gameboy,socket_cliente, g_logger);
		} else {
			send_msg_get_gamecard(msg_gameboy,socket_cliente, g_logger);
		}
		break;
	case NEW_POKEMON:
		if (proceso == BROKER) {
			send_msg_new_broker(msg_gameboy,socket_cliente, g_logger);
		} else {
			send_msg_new_gamecard(msg_gameboy,socket_cliente, g_logger);
		}
		break;
	case APPEARED_POKEMON:
		if (proceso == BROKER) {
			send_msg_appeared_broker(msg_gameboy,socket_cliente, g_logger);
		} else {
			send_msg_appeared_team(msg_gameboy,socket_cliente, g_logger);
		}
		break;
	}
	list_destroy(msg_gameboy->argumentos);
	free(msg_gameboy);
}

void send_msg_gameboy_suscriptor(t_mensaje_gameboy *msg_gameboy, int socket_cliente)
{
	t_tipo_mensaje tipo_mensaje = msg_gameboy->tipo_mensaje;
	uint32_t id_recibido = MENSAJE_0;
	enviar_msj_suscripcion_broker(tipo_mensaje, id_recibido, socket_cliente);
	list_destroy(msg_gameboy->argumentos);
	free(msg_gameboy);
}

void enviar_msj_suscripcion_broker(t_tipo_mensaje tipo_mensaje, uint32_t id_recibido, int socket_cliente)
{
	t_handsake_suscript *handshake = malloc(sizeof(t_handsake_suscript));
	handshake->id_suscriptor = g_config_gameboy->id_suscriptor;
	handshake->id_recibido = id_recibido;
	handshake->cola_id = tipo_mensaje;
	handshake->msjs_recibidos = g_rcv_msg_qty;
	enviar_msj_handshake_suscriptor(socket_cliente, g_logger, handshake);
	free(handshake);
}

void send_msg_catch_broker(t_mensaje_gameboy *msg_gameboy, int socket_cliente, t_log *logger)
{
	t_msg_catch_broker *msg_catch_broker = malloc(sizeof(t_msg_catch_broker));
	msg_catch_broker->coordenada = malloc(sizeof(t_coordenada));
	msg_catch_broker->coordenada->pos_x = atoi(list_get(msg_gameboy->argumentos, 1));
	msg_catch_broker->coordenada->pos_y = atoi(list_get(msg_gameboy->argumentos, 2));
	char *pokemon = list_get(msg_gameboy->argumentos, 0);
	msg_catch_broker->size_pokemon = strlen(pokemon) + 1;
	msg_catch_broker->pokemon = malloc(msg_catch_broker->size_pokemon);
	memcpy(msg_catch_broker->pokemon, pokemon, msg_catch_broker->size_pokemon);
	enviar_msj_catch_broker(socket_cliente, logger, msg_catch_broker);
	eliminar_msg_catch_broker(msg_catch_broker);
}

void send_msg_new_broker(t_mensaje_gameboy *msg_gameboy, int socket_cliente, t_log *logger)
{
	t_msg_new_broker *msg_new = malloc(sizeof(t_msg_new_broker));
	msg_new->coordenada = malloc(sizeof(t_coordenada));
	msg_new->coordenada->pos_x = atoi(list_get(msg_gameboy->argumentos, 1));
	msg_new->coordenada->pos_y = atoi(list_get(msg_gameboy->argumentos, 2));
	msg_new->cantidad = atoi(list_get(msg_gameboy->argumentos, 3));
	char *pokemon = list_get(msg_gameboy->argumentos, 0);
	msg_new->size_pokemon = strlen(pokemon) + 1;
	msg_new->pokemon = malloc(msg_new->size_pokemon);
	memcpy(msg_new->pokemon, pokemon, msg_new->size_pokemon);
	enviar_msj_new_broker(socket_cliente, logger, msg_new);
	eliminar_msg_new_broker(msg_new);
}

void send_msg_new_gamecard(t_mensaje_gameboy *msg_gameboy, int socket_cliente, t_log *logger) {
	t_msg_new_gamecard *msg_new = malloc(sizeof(t_msg_new_gamecard));
	msg_new->coord = malloc(sizeof(t_coordenada));
	char *pokemon = list_get(msg_gameboy->argumentos, 0);
	msg_new->coord->pos_x = atoi(list_get(msg_gameboy->argumentos, 1));
	msg_new->coord->pos_y = atoi(list_get(msg_gameboy->argumentos, 2));
	msg_new->cantidad = atoi(list_get(msg_gameboy->argumentos, 3));
	msg_new->id_mensaje = atoi(list_get(msg_gameboy->argumentos, 4));
	msg_new->size_pokemon = strlen(pokemon) + 1;
	msg_new->pokemon = malloc(msg_new->size_pokemon);
	memcpy(msg_new->pokemon, pokemon, msg_new->size_pokemon);
	t_socket_cliente_broker *socket = malloc(sizeof(t_socket_cliente_broker));
	socket->cliente_fd = socket_cliente;
	socket->cant_msg_enviados = 0;
	enviar_msj_new_gamecard(socket, logger, msg_new);
	free(socket);
	eliminar_msg_new_gamecard(msg_new);
}

void send_msg_get_broker(t_mensaje_gameboy *msg_gameboy, int socket_cliente, t_log *logger)
{
	char *pokemon = list_get(msg_gameboy->argumentos, 0);
	t_msg_get_broker *msg_get = malloc(sizeof(t_msg_get_broker));
	msg_get->size_pokemon = strlen(pokemon) + 1;
	msg_get->pokemon = malloc(msg_get->size_pokemon);
	memcpy(msg_get->pokemon, pokemon, msg_get->size_pokemon);
	enviar_msj_get_broker(socket_cliente, logger, msg_get);
	free(msg_get->pokemon);
	free(msg_get);
}

void send_msg_get_gamecard(t_mensaje_gameboy *msg_gameboy, int socket_cliente, t_log *logger)
{
	t_msg_get_gamecard *msg_get = malloc(sizeof(t_msg_get_gamecard));
	char *pokemon = list_get(msg_gameboy->argumentos, 0);
	msg_get->id_mensaje = atoi(list_get(msg_gameboy->argumentos, 1));
	msg_get->size_pokemon = strlen(pokemon) + 1;
	msg_get->pokemon = malloc(msg_get->size_pokemon);
	memcpy(msg_get->pokemon, pokemon, msg_get->size_pokemon);
	t_socket_cliente_broker *socket = malloc(sizeof(t_socket_cliente_broker));
	socket->cliente_fd = socket_cliente;
	socket->cant_msg_enviados = 0;
	enviar_msj_get_gamecard(socket, logger, msg_get);
	free(socket);
	eliminar_msg_get_gamecard(msg_get);
}

void send_msg_catch_gamecard(t_mensaje_gameboy *msg_gameboy, int socket_cliente, t_log *logger)
{
	t_msg_catch_gamecard *msg_catch = malloc(sizeof(t_msg_catch_gamecard));
	msg_catch->coord = malloc(sizeof(t_coordenada));
	char *pokemon = list_get(msg_gameboy->argumentos, 0);
	msg_catch->coord->pos_x = atoi(list_get(msg_gameboy->argumentos, 1));
	msg_catch->coord->pos_y = atoi(list_get(msg_gameboy->argumentos, 2));
	msg_catch->id_mensaje = atoi(list_get(msg_gameboy->argumentos, 3));
	msg_catch->size_pokemon = strlen(pokemon) + 1;
	msg_catch->pokemon = malloc(msg_catch->size_pokemon);
	memcpy(msg_catch->pokemon ,pokemon, msg_catch->size_pokemon);
	t_socket_cliente_broker *socket = malloc(sizeof(t_socket_cliente_broker));
	socket->cliente_fd = socket_cliente;
	socket->cant_msg_enviados = 0;
	enviar_msj_catch_gamecard(socket, logger, msg_catch);
	free(socket);
	eliminar_msg_catch_gamecard(msg_catch);
}

void send_msg_caught_broker(t_mensaje_gameboy *msg_gameboy, int socket_cliente, t_log *logger)
{
	t_msg_caught_broker *msg_caught = malloc(sizeof(t_msg_caught_broker));
	msg_caught->id_correlativo = atoi(list_get(msg_gameboy->argumentos, 0));
	char *arg_resul_caught = list_get(msg_gameboy->argumentos, 1);
	msg_caught->resultado = codificar_resultado_caught(arg_resul_caught);
	enviar_msj_caught_broker(socket_cliente, logger, msg_caught);
	free(msg_caught);
}

void send_msg_appeared_broker(t_mensaje_gameboy *msg_gameboy, int socket_cliente, t_log *logger)
{
	t_msg_appeared_broker *msg_appeared = malloc(sizeof(t_msg_appeared_broker));
	msg_appeared->coordenada = malloc(sizeof(t_coordenada));
	char *pokemon = list_get(msg_gameboy->argumentos, 0);
	msg_appeared->coordenada->pos_x = atoi(list_get(msg_gameboy->argumentos, 1));
	msg_appeared->coordenada->pos_y = atoi(list_get(msg_gameboy->argumentos, 2));
	msg_appeared->id_correlativo = atoi(list_get(msg_gameboy->argumentos, 3));
	msg_appeared->size_pokemon = strlen(pokemon) + 1;
	msg_appeared->pokemon = malloc(msg_appeared->size_pokemon);
	memcpy(msg_appeared->pokemon, pokemon, msg_appeared->size_pokemon);
	enviar_msj_appeared_broker(socket_cliente, logger, msg_appeared);
	eliminar_msg_appeared_broker(msg_appeared);
}

void send_msg_appeared_team(t_mensaje_gameboy *msg_gameboy, int socket_cliente, t_log *logger)
{
	t_msg_appeared_team *msg_appeared = malloc(sizeof(t_msg_appeared_team));
	msg_appeared->coord = malloc(sizeof(t_coordenada));
	char *pokemon = list_get(msg_gameboy->argumentos, 0);
	msg_appeared->coord->pos_x = atoi(list_get(msg_gameboy->argumentos, 1));
	msg_appeared->coord->pos_y = atoi(list_get(msg_gameboy->argumentos, 2));
	msg_appeared->id_mensaje = g_config_gameboy->id_mensaje_unico;
	msg_appeared->id_correlativo = msg_appeared->id_mensaje + 1;
	msg_appeared->size_pokemon = strlen(pokemon) + 1;
	msg_appeared->pokemon = malloc(msg_appeared->size_pokemon);
	memcpy(msg_appeared->pokemon, pokemon, msg_appeared->size_pokemon);
	t_socket_cliente_broker *socket = malloc(sizeof(t_socket_cliente_broker));
	socket->cliente_fd = socket_cliente;
	socket->cant_msg_enviados = 0;
	enviar_msj_appeared_team(socket, logger, msg_appeared);
	free(socket);
	eliminar_msg_appeared_team(msg_appeared);
}

void esperar_rta_servidor(int socket_cliente)
{
	op_code codigo_operacion = rcv_codigo_operacion(socket_cliente);
	int respuesta;
	void *a_recibir;
	if (codigo_operacion == MSG_CONFIRMED) {
		respuesta = rcv_msg_confirmed(socket_cliente, g_logger);
		if (respuesta != RESPUESTA_OK) {
			log_warning(g_logger, "(MSG_ERROR)");
		}
	}
	else if (codigo_operacion == MSG_ERROR) {
		a_recibir = recibir_buffer(socket_cliente, &respuesta);
		log_warning(g_logger, "(RECEIVING: |%s)", a_recibir);
		free(a_recibir);
	}
	else if (codigo_operacion == APPEARED_BROKER) {
		t_msg_appeared_broker *msg_appeared = (t_msg_appeared_broker*) a_recibir;
		msg_appeared = rcv_msj_appeared_broker(socket_cliente, g_logger);
		eliminar_msg_appeared_broker(msg_appeared);
	}
	else if (codigo_operacion == ID_MENSAJE) {
		respuesta = rcv_id_mensaje(socket_cliente, g_logger);
	}
	else if (codigo_operacion == CAUGHT_BROKER) {
		t_msg_caught_broker *msg_caught = (t_msg_caught_broker*) a_recibir;
		msg_caught = rcv_msj_caught_broker(socket_cliente, g_logger);
		free(msg_caught);
	}
	else if (codigo_operacion == LOCALIZED_BROKER) {
		t_msg_localized_broker *msg_localized = (t_msg_localized_broker*) a_recibir;
		msg_localized = rcv_msj_localized_broker(socket_cliente, g_logger);
		eliminar_msg_localized_broker(msg_localized);
	}
}

uint32_t rcv_mensaje_publisher(op_code codigo_operacion, int socket_cliente)
{
	uint32_t id_recibido;
	void *msg;
	switch(codigo_operacion){
	case COLA_VACIA:;
		id_recibido = rcv_msj_cola_vacia(socket_cliente, g_logger);
		if (id_recibido != g_config_gameboy->id_suscriptor) {
			log_error(g_logger,"MSG_ERROR");
		}
		break;
	case NEW_GAMECARD:;
		t_msg_new_gamecard *msg_new = (t_msg_new_gamecard*) msg;
		msg_new = rcv_msj_new_gamecard(socket_cliente, g_logger);
		id_recibido = msg_new->id_mensaje;
		eliminar_msg_new_gamecard(msg_new);
		break;
	case CATCH_GAMECARD:;
		t_msg_catch_gamecard *msg_catch = (t_msg_catch_gamecard*) msg;
		msg_catch = rcv_msj_catch_gamecard(socket_cliente, g_logger);
		id_recibido = msg_catch->id_mensaje;
		eliminar_msg_catch_gamecard(msg_catch);
		break;
	case GET_GAMECARD:;
		t_msg_get_gamecard *msg_get = (t_msg_get_gamecard*) msg;
		msg_get = rcv_msj_get_gamecard(socket_cliente, g_logger);
		id_recibido = msg_get->id_mensaje;
		eliminar_msg_get_gamecard(msg_get);
		break;
	case APPEARED_TEAM:;
		t_msg_appeared_team *msg_appeared = (t_msg_appeared_team*) msg;
		msg_appeared = rcv_msj_appeared_team(socket_cliente, g_logger);
		id_recibido = msg_appeared->id_mensaje;
		eliminar_msg_appeared_team(msg_appeared);
		break;
	case CAUGHT_TEAM:;
		t_msg_caught_team *msg_caught = (t_msg_caught_team*) msg;
		msg_caught = rcv_msj_caught_team(socket_cliente, g_logger);
		id_recibido = msg_caught->id_mensaje;
		free(msg_caught);
		break;
	case LOCALIZED_TEAM:;
		t_msg_localized_team *msg_localized = (t_msg_localized_team*) msg;
		msg_localized = rcv_msj_localized_team(socket_cliente, g_logger);
		id_recibido = msg_localized->id_mensaje;
		eliminar_msg_localized_team(msg_localized);
		break;
	case 0:
		pthread_exit(NULL);
	case -1:
		pthread_exit(NULL);
	}
	return id_recibido;
}

void borrar_comienzo_lista(t_list* lista, int cant) {
	if (lista->elements_count >= cant) {
		int i = 0;
		for (i = 0; i < cant; i++) {
			list_remove(lista, 0);
		}
	}
}

void iniciar_log_gameboy(void) {
	g_logger = log_create(g_config_gameboy->ruta_log, "GAME_BOY", 1, LOG_LEVEL_TRACE);
	//------------ Quitar el "1" para que no loguee por Pantalla -------//
}

void leer_config_gameboy(char *path) {
	g_config = config_create(path);
	g_config_gameboy = malloc(sizeof(t_config_gameboy));
	g_config_gameboy->ip_broker = config_get_string_value(g_config,"IP_BROKER");
	g_config_gameboy->ip_gamecard = config_get_string_value(g_config,"IP_GAMECARD");
	g_config_gameboy->ip_team = config_get_string_value(g_config, "IP_TEAM");
	g_config_gameboy->puerto_broker = config_get_string_value(g_config,"PUERTO_BROKER");
	g_config_gameboy->puerto_gamecard = config_get_string_value(g_config,"PUERTO_GAMECARD");
	g_config_gameboy->puerto_team = config_get_string_value(g_config,"PUERTO_TEAM");
	g_config_gameboy->coord_x_max = config_get_int_value(g_config,"COORDENADA_X-MAX");
	g_config_gameboy->coord_y_max = config_get_int_value(g_config,"COORDENADA_Y-MAX");
	g_config_gameboy->cant_max_pokemon = config_get_int_value(g_config,"CANT_MAX_POKEMON");
	g_config_gameboy->time_suscrip_max = config_get_int_value(g_config,"TIME_SUSCRIP_MAX");
	g_config_gameboy->id_mensaje_unico = config_get_int_value(g_config,"ID_MENSAJE_UNICO");
	g_config_gameboy->id_suscriptor = config_get_int_value(g_config,"ID_SUSCRIPTOR");
	g_config_gameboy->ruta_log = config_get_string_value(g_config, "RUTA_LOG");
}

void finalizar_gameboy(t_config_gameboy *config_gameboy, t_log *log, t_config *config) {
	log_destroy(log);
	config_destroy(config);
	free(config_gameboy);
}
