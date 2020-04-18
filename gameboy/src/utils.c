/*
 *		utils.c
 *
 *  	Created on: 10 apr. 2020
 *
 *      Author:  Grupo "tp-2020-1C Los Que Aprueban"
 *
 *
 */

#include "utils.h"

void construir_mensaje(t_mensaje_gameboy *argumentos_mensaje, t_list *lista) {
	char *proceso = list_get(lista, 0);
	char *tipo_mensaje = list_get(lista, 1);

	if (strcmp(proceso, "SUSCRIPTOR") == 0) {
		argumentos_mensaje->proceso = SUSCRIPTOR;
		argumentos_mensaje->tipo_mensaje = select_tipo_mensaje(tipo_mensaje);
		cargar_argumentos(argumentos_mensaje, lista);
	} else if (strcmp(proceso, "BROKER") == 0) {
		argumentos_mensaje->proceso = BROKER;
		argumentos_mensaje->tipo_mensaje = select_tipo_mensaje(tipo_mensaje);
		cargar_argumentos(argumentos_mensaje, lista);
	} else if (strcmp(proceso, "GAMECARD") == 0) {
		argumentos_mensaje->proceso = GAMECARD;
		argumentos_mensaje->tipo_mensaje = select_tipo_mensaje(tipo_mensaje);
		cargar_argumentos(argumentos_mensaje, lista);
	} else if (strcmp(proceso, "TEAM") == 0) {
		argumentos_mensaje->proceso = TEAM;
		argumentos_mensaje->tipo_mensaje = select_tipo_mensaje(tipo_mensaje);
		cargar_argumentos(argumentos_mensaje, lista);
	} else {
		argumentos_mensaje->proceso = UNKNOWN_PROC;
		argumentos_mensaje->tipo_mensaje = UNKNOWN_QUEUE;
		list_clean(argumentos_mensaje->argumentos);

	}
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
	borrar_comienzo(lista, 2);
	list_add_all(argumentos_mensaje->argumentos, lista);
}

bool validar_argumentos(t_mensaje_gameboy *argumentos_mensaje) {
	int proceso = argumentos_mensaje->proceso;
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
			if (proceso != TEAM && proceso != GAMEBOY && cant_argumentos == 4
					&& validar_coordXY(argumentos_mensaje->argumentos, 1, 3)
							== 1) {
				resultado = 1;
			} else {
				resultado = 0;
			}
			break;
		case APPEARED_POKEMON:
			if (proceso == BROKER && cant_argumentos == 4
					&& validar_coordXY(argumentos_mensaje->argumentos, 1, 2)
							== 1
					&& validar_id_mensaje(argumentos_mensaje->argumentos, 3)
							== 1) {
				resultado = 1;
			} else if (proceso == TEAM && cant_argumentos == 3
					&& validar_coordXY(argumentos_mensaje->argumentos, 1, 2)
							== 1) {
				resultado = 1;
			} else {
				resultado = 0;
			}
			break;
		case CATCH_POKEMON:
			if (proceso != TEAM && proceso != GAMEBOY && cant_argumentos == 3
					&& validar_coordXY(argumentos_mensaje->argumentos, 1, 2)
							== 1) {
				resultado = 1;
			} else {
				resultado = 0;
			}
			break;
		case CAUGHT_POKEMON:
			if (proceso == BROKER && cant_argumentos == 2
					&& validar_id_mensaje(argumentos_mensaje->argumentos, 0)
							== 1
					&& validar_resultado_caught(argumentos_mensaje->argumentos,
							1) == 1) {
				resultado = 1;
			} else {
				resultado = 0;
			}
			break;
		case GET_POKEMON:
			if (proceso != TEAM && proceso != GAMEBOY && cant_argumentos == 1) {
				resultado = 1;
			} else {
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
	if (atoi(tiempo) > 0 && atoi(tiempo) <= 120) {
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

int crear_conexion(t_mensaje_gameboy *msg_gameboy) {
	struct addrinfo hints;
	struct addrinfo *server_info;
	char *ip = select_ip_proceso(msg_gameboy);
	char *puerto = select_puerto_proceso(msg_gameboy);
	char *proceso = obtengo_proceso(msg_gameboy);
	char *cola = obtengo_cola(msg_gameboy);

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family,
			server_info->ai_socktype, server_info->ai_protocol);
	int conexion = connect(socket_cliente, server_info->ai_addr,
			server_info->ai_addrlen);
	if (conexion < 0) {
		log_error(g_logger,
				"(CONN_FAILED | PROCESS=%s | QUEUE=%s | REMOTE_IP=%s | PORT=%s)",
				proceso, cola, ip, puerto);
		socket_cliente = conexion;
	} else {
		log_info(g_logger,
				"(CONN_SUCCESS | SOCKET=%d | PROCESS=%s | QUEUE=%s | REMOTE_IP=%s | PORT=%s)",
				socket_cliente, proceso, cola, ip, puerto);
	}
	freeaddrinfo(server_info);
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
		proceso = "BROKER | MODE=SUSCRIBER";
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
		cola = "APPEARED";
		break;
	case CATCH_POKEMON:
		cola = "CATCH";
		break;
	case CAUGHT_POKEMON:
		cola = "CAUGHT";
		break;
	case GET_POKEMON:
		cola = "GET";
		break;
	case LOCALIZED_POKEMON:
		cola = "LOCALIZED";
		break;
	case NEW_POKEMON:
		cola = "NEW";
		break;
	}
	return cola;
}

void enviar_mensaje(t_mensaje_gameboy *msg_gameboy, int socket_cliente) {
	int proceso = msg_gameboy->proceso;
	int tipo_mensaje = msg_gameboy->tipo_mensaje;
	t_paquete *paquete = malloc(sizeof(t_paquete));
	void *a_enviar;
	int bytes = 0;

	switch (tipo_mensaje) {
	case CATCH_POKEMON:
		if (proceso == BROKER) {
			empaquetar_catch_broker(msg_gameboy, paquete);
		} else {
			empaquetar_catch_gamecard(msg_gameboy, paquete);
		}
		break;
	case CAUGHT_POKEMON:

		break;
	case GET_POKEMON:
		if (proceso == BROKER) {
			empaquetar_get_broker(msg_gameboy, paquete);
		}
		break;
	case NEW_POKEMON:
		if (proceso == BROKER) {
			empaquetar_new_broker(msg_gameboy, paquete);
		}
		break;
	case APPEARED_POKEMON:

		break;
	}

	bytes = paquete->buffer->size + 2 * (sizeof(int));

	a_enviar = serializar_paquete(paquete, &bytes);

	log_info(g_logger, "Se envia mensaje de %d bytes", bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

void empaquetar_catch_broker(t_mensaje_gameboy *msg_gameboy, t_paquete *paquete) {
	int pos_x = atoi(list_get(msg_gameboy->argumentos, 1));
	int pos_y = atoi(list_get(msg_gameboy->argumentos, 2));
	char *pokemon = list_get(msg_gameboy->argumentos, 0);

	t_stream *buffer = malloc(sizeof(t_stream));
	buffer->size = 2 * sizeof(int) + strlen(pokemon) + 1;

	void *stream = malloc(buffer->size);

	int offset = 0;
	memcpy(stream + offset, &(pos_x), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(pos_y), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, pokemon, strlen(pokemon) + 1);
	buffer->data = stream;

	paquete->codigo_operacion = CATCH_BROKER;
	paquete->buffer = buffer;
	log_info(g_logger, "(SENDING_MSG= %s | %d | %d)", pokemon, pos_x, pos_y);
}

void empaquetar_new_broker(t_mensaje_gameboy *msg_gameboy, t_paquete *paquete) {

	int pos_x = atoi(list_get(msg_gameboy->argumentos, 1));
	int pos_y = atoi(list_get(msg_gameboy->argumentos, 2));
	int cantidad = atoi(list_get(msg_gameboy->argumentos, 3));
	char *pokemon = list_get(msg_gameboy->argumentos, 0);

	t_stream *buffer = malloc(sizeof(t_stream));
	buffer->size = 3 * sizeof(int) + strlen(pokemon) + 1;

	void *stream = malloc(buffer->size);

	int offset = 0;
	memcpy(stream + offset, &(pos_x), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(pos_y), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(cantidad), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, pokemon, strlen(pokemon) + 1);
	buffer->data = stream;

	paquete->codigo_operacion = NEW_BROKER;
	paquete->buffer = buffer;
	log_info(g_logger, "(SENDING_MSG= %s | %d | %d | %d)", pokemon, pos_x,
			pos_y, cantidad);

}

void empaquetar_get_broker(t_mensaje_gameboy *msg_gameboy, t_paquete *paquete) {

	char *pokemon = list_get(msg_gameboy->argumentos, 0);

	t_stream *buffer = malloc(sizeof(t_stream));
	buffer->size = strlen(pokemon) + 1;

	void *stream = malloc(buffer->size);

	memcpy(stream, pokemon, strlen(pokemon) + 1);
	buffer->data = stream;

	paquete->codigo_operacion = GET_BROKER;
	paquete->buffer = buffer;

	log_info(g_logger, "(SENDING_MSG= %s)", pokemon);

}

void empaquetar_catch_gamecard(t_mensaje_gameboy *msg_gameboy,
		t_paquete *paquete) {

	int idUnico = g_config_gameboy->id_mensaje_unico;
	int pos_x = atoi(list_get(msg_gameboy->argumentos, 0));
	int pos_y = atoi(list_get(msg_gameboy->argumentos, 1));
	char* pokemon = list_get(msg_gameboy->argumentos, 2);

	t_stream *buffer = malloc(sizeof(t_stream));
	buffer->size = strlen(pokemon) + 1 + (3 * sizeof(int));

	void *stream = malloc(buffer->size);

	int offset = 0;
	memcpy(stream + offset, &(idUnico), sizeof(int));
	offset += sizeof(int);

	memcpy(stream + offset, &(pos_x), sizeof(int));
	offset += sizeof(int);

	memcpy(stream + offset, &(pos_y), sizeof(int));
	offset += sizeof(int);

	memcpy(stream + offset, pokemon, strlen(pokemon) + 1);
	offset += strlen(pokemon) + 1;
	buffer->data = stream;

	paquete->codigo_operacion = CATCH_GAMECARD;
	paquete->buffer = buffer;

	log_info(g_logger, "(SENDING_MSG= %s | %d | %d | %d) \n", pokemon, pos_x,
			pos_y, idUnico);

}

void eliminar_paquete(t_paquete* paquete) {
	free(paquete->buffer->data);
	free(paquete->buffer);
	free(paquete);
}

void* serializar_paquete(t_paquete *paquete, int *bytes) {
	void *a_enviar = malloc(*bytes);
	int offset = 0;

	memcpy(a_enviar + offset, &(paquete->codigo_operacion), sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, paquete->buffer->data, paquete->buffer->size);
	offset += paquete->buffer->size;
	return a_enviar;
}

char* recibir_mensaje(int socket_cliente) {
	printf("codigo operacion = %d \n", recibir_op_code(socket_cliente));
	int size;
	void* a_recibir = recibir_buffer(&size, socket_cliente);
	char* mensaje = malloc(size);

	memcpy(mensaje, a_recibir, size);
	free(a_recibir);
	return mensaje;
}

int recibir_op_code(int socket_cliente) {
	int code;
	recv(socket_cliente, &code, sizeof(int), MSG_WAITALL);
	return code;
}

void* recibir_buffer(int* size, int socket_cliente) {
	void* _stream;
	recv(socket_cliente, size, sizeof(int), MSG_WAITALL); // el argumento size es un puntero
	_stream = malloc(*size);
	recv(socket_cliente, _stream, *size, MSG_WAITALL);
	return _stream;
}

void borrar_comienzo(t_list* lista, int cant) {
	if (lista->elements_count >= cant) {
		int i = 0;
		for (i = 0; i < cant; i++) {
			list_remove(lista, 0);
		}
	}
}

void iniciar_log(void) {
	g_logger = log_create(g_config_gameboy->ruta_log, "GAME_BOY", 1,
			LOG_LEVEL_INFO);
	//------------ Quitar el "1" para que no loguee por Pantalla -------//
}

void leer_config(char *path) {
	g_config = config_create(path);
	g_config_gameboy = malloc(sizeof(t_config_gameboy));
	g_config_gameboy->ip_broker = config_get_string_value(g_config,
			"IP_BROKER");
	g_config_gameboy->ip_gamecard = config_get_string_value(g_config,
			"IP_GAMECARD");
	g_config_gameboy->ip_team = config_get_string_value(g_config, "IP_TEAM");
	g_config_gameboy->puerto_broker = config_get_string_value(g_config,
			"PUERTO_BROKER");
	g_config_gameboy->puerto_gamecard = config_get_string_value(g_config,
			"PUERTO_GAMECARD");
	g_config_gameboy->puerto_team = config_get_string_value(g_config,
			"PUERTO_TEAM");
	g_config_gameboy->coord_x_max = config_get_int_value(g_config,
			"COORDENADA_X-MAX");
	g_config_gameboy->coord_y_max = config_get_int_value(g_config,
			"COORDENADA_Y-MAX");
	g_config_gameboy->cant_max_pokemon = config_get_int_value(g_config,
			"CANT_MAX_POKEMON");
	g_config_gameboy->id_mensaje_unico = config_get_int_value(g_config,
			"ID_MENSAJE_UNICO");
	g_config_gameboy->ruta_log = config_get_string_value(g_config, "RUTA_LOG");
}

void terminar_programa(t_mensaje_gameboy *msg_gameboy,
		t_config_gameboy *config_gameboy, t_list *lista, t_log *log,
		t_config *config, int conexion) {
	log_destroy(log);
	config_destroy(config);
	list_destroy(lista);
	list_destroy(msg_gameboy->argumentos);
	free(config_gameboy);
	free(msg_gameboy);
	liberar_conexion(conexion);
}

void liberar_conexion(int socket_cliente) {
	close(socket_cliente);
}
