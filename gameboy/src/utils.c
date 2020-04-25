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
				"(CONN_FAILED | PROCESS=%s | QUEUE=%s_POKEMON | REMOTE_IP=%s | PORT=%s)",
				proceso, cola, ip, puerto);
		socket_cliente = conexion;
	} else {
		log_info(g_logger,
				"(CONN_SUCCESS | SOCKET# %d | TO: %s@%s_POKEMON | REMOTE_IP=%s | PORT=%s)",
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

int get_time_suscripcion(t_mensaje_gameboy *msg_gameboy) {
	char *tiempo_ingresado = list_get(msg_gameboy->argumentos,0);
	int tiempo = atoi(tiempo_ingresado);
	return tiempo;
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
		empaquetar_caught_broker(msg_gameboy, paquete);
		break;
	case GET_POKEMON:
		if (proceso == BROKER) {
			empaquetar_get_broker(msg_gameboy, paquete);
		} else {
			empaquetar_get_gamecard(msg_gameboy, paquete);
		}
		break;
	case NEW_POKEMON:
		if (proceso == BROKER) {
			empaquetar_new_broker(msg_gameboy, paquete);
		} else {
			empaquetar_new_gamecard(msg_gameboy, paquete);
		}
		break;
	case APPEARED_POKEMON:
		if (proceso == BROKER) {
			empaquetar_appeared_broker(msg_gameboy, paquete);
		} else {
			empaquetar_appeared_team(msg_gameboy, paquete);
		}
		break;
	}

	bytes = paquete->buffer->size + 2 * (sizeof(int));

	a_enviar = serializar_paquete(paquete, &bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

void enviar_msj_suscriptor(t_mensaje_gameboy *msg_gameboy, int socket_cliente)
{
	// En esta funcion: msg_gameboy->proceso = SUSCRIPTOR
	int tipo_mensaje = msg_gameboy->tipo_mensaje; // Una Cola del BROKER -- Cualquiera de las 6(seis)
	t_paquete *paquete = malloc(sizeof(t_paquete));
	void *a_enviar;
	int bytes = 0;

	switch (tipo_mensaje) {
	case CATCH_POKEMON:
		paquete->codigo_operacion = SUSCRIP_CATCH;
		break;
	case CAUGHT_POKEMON:
		paquete->codigo_operacion = SUSCRIP_CAUGHT;
		break;
	case NEW_POKEMON:
		paquete->codigo_operacion = SUSCRIP_NEW;
		break;
	case APPEARED_POKEMON:
		paquete->codigo_operacion = SUSCRIP_APPEARED;
		break;
	case GET_POKEMON:
		paquete->codigo_operacion = SUSCRIP_GET;
		break;
	case LOCALIZED_POKEMON:
		paquete->codigo_operacion = SUSCRIP_LOCALIZED;
		break;
	}
	empaquetar_handshake_suscriptor(paquete);

	bytes = paquete->buffer->size + 2 * (sizeof(int));

	a_enviar = serializar_paquete(paquete, &bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

void enviar_fin_suscripcion(t_mensaje_gameboy *msg_gameboy,int cant_mensajes, int socket_cliente)
{
	t_tipo_mensaje cola = msg_gameboy->tipo_mensaje;
	t_paquete *paquete = malloc(sizeof(t_paquete));
	void *a_enviar;
	int bytes = 0;
	paquete->codigo_operacion = FIN_SUSCRIPCION;

	empaquetar_fin_suscripcion(cola, cant_mensajes, paquete);

	bytes = paquete->buffer->size + 2 * (sizeof(int));

	a_enviar = serializar_paquete(paquete, &bytes);
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
	int tamano = tamano_paquete(paquete);
	log_info(g_logger, "(SENDING_MSG= %s | %d | %d -- SIZE= %d Bytes)", pokemon,
			pos_x, pos_y, tamano);
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
	int tamano = tamano_paquete(paquete);
	log_info(g_logger, "(SENDING_MSG= %s | %d | %d | %d -- SIZE= %d Bytes)",
			pokemon, pos_x, pos_y, cantidad, tamano);
}

void empaquetar_new_gamecard(t_mensaje_gameboy *msg_gameboy, t_paquete *paquete) {
	int pos_x = atoi(list_get(msg_gameboy->argumentos, 1));
	int pos_y = atoi(list_get(msg_gameboy->argumentos, 2));
	int cantidad = atoi(list_get(msg_gameboy->argumentos, 3));
	char *pokemon = list_get(msg_gameboy->argumentos, 0);
	int idUnico = g_config_gameboy->id_mensaje_unico;

	t_stream *buffer = malloc(sizeof(t_stream));
	buffer->size = 4 * sizeof(int) + strlen(pokemon) + 1;

	void *stream = malloc(buffer->size);

	int offset = 0;
	memcpy(stream + offset, &(idUnico), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(pos_x), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(pos_y), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(cantidad), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, pokemon, strlen(pokemon) + 1);
	buffer->data = stream;

	paquete->codigo_operacion = NEW_GAMECARD;
	paquete->buffer = buffer;
	int tamano = tamano_paquete(paquete);

	log_info(g_logger,
			"(SENDING_MSG= %d | %s | %d | %d | %d -- SIZE= %d Bytes)", idUnico,
			pokemon, pos_x, pos_y, cantidad, tamano);
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

	int tamano = tamano_paquete(paquete);
	log_info(g_logger, "(SENDING_MSG= %s -- SIZE= %d Bytes)", pokemon, tamano);
}

void empaquetar_get_gamecard(t_mensaje_gameboy *msg_gameboy, t_paquete *paquete) {
	char *pokemon = list_get(msg_gameboy->argumentos, 0);
	int idUnico = g_config_gameboy->id_mensaje_unico;
	t_stream *buffer = malloc(sizeof(t_stream));
	buffer->size = sizeof(int) + strlen(pokemon) + 1;
	void *stream = malloc(buffer->size);

	int offset = 0;
	memcpy(stream + offset, &(idUnico), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, pokemon, strlen(pokemon) + 1);
	buffer->data = stream;

	paquete->codigo_operacion = GET_GAMECARD;
	paquete->buffer = buffer;
	int tamano = tamano_paquete(paquete);

	log_info(g_logger, "(SENDING_MSG= %d | %s -- SIZE= %d Bytes)", idUnico,
			pokemon, tamano);
}

void empaquetar_catch_gamecard(t_mensaje_gameboy *msg_gameboy,
		t_paquete *paquete) {

	int idUnico = g_config_gameboy->id_mensaje_unico;
	char *pokemon = list_get(msg_gameboy->argumentos, 0);
	int pos_x = atoi(list_get(msg_gameboy->argumentos, 1));
	int pos_y = atoi(list_get(msg_gameboy->argumentos, 2));

	t_stream *buffer = malloc(sizeof(t_stream));
	buffer->size = 3 * sizeof(int) + strlen(pokemon) + 1;

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

	int tamano = tamano_paquete(paquete);

	log_info(g_logger, "(SENDING_MSG= %d | %s | %d | %d -- SIZE = %d Bytes) \n",
			idUnico, pokemon, pos_x, pos_y, tamano);

}

void empaquetar_caught_broker(t_mensaje_gameboy *msg_gameboy,
		t_paquete *paquete) {
	int id_mensaje = atoi(list_get(msg_gameboy->argumentos, 0));
	char *arg_resul_caught = list_get(msg_gameboy->argumentos, 1);
	t_result_caught result_caught = codificar_resultado_caught(
			arg_resul_caught);

	t_stream *buffer = malloc(sizeof(t_stream));
	buffer->size = 2 * sizeof(int);

	void *stream = malloc(buffer->size);

	int offset = 0;
	memcpy(stream + offset, &(id_mensaje), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(result_caught), sizeof(int));
	buffer->data = stream;
	paquete->codigo_operacion = CAUGHT_BROKER;
	paquete->buffer = buffer;
	int tamano = tamano_paquete(paquete);

	log_info(g_logger, "(SENDING_MSG= %d | %d -- SIZE = %d Bytes)", id_mensaje,
			result_caught, tamano);
}

void empaquetar_appeared_broker(t_mensaje_gameboy *msg_gameboy,
		t_paquete *paquete) {

	char *pokemon = list_get(msg_gameboy->argumentos, 0);
	int pos_x = atoi(list_get(msg_gameboy->argumentos, 1));
	int pos_y = atoi(list_get(msg_gameboy->argumentos, 2));
	int id_mensaje = atoi(list_get(msg_gameboy->argumentos, 3));

	t_stream *buffer = malloc(sizeof(t_stream));
	buffer->size = 3 * sizeof(int) + strlen(pokemon) + 1;

	void *stream = malloc(buffer->size);

	int offset = 0;
	memcpy(stream + offset, &(id_mensaje), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(pos_x), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(pos_y), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, pokemon, strlen(pokemon) + 1);

	buffer->data = stream;

	paquete->codigo_operacion = APPEARED_BROKER;
	paquete->buffer = buffer;
	int tamano = tamano_paquete(paquete);

	log_info(g_logger, "(SENDING_MSG= %d | %s | %d | %d -- SIZE = %d bYTES)",
			id_mensaje, pokemon, pos_x, pos_y, tamano);

}

void empaquetar_appeared_team(t_mensaje_gameboy *msg_gameboy,
		t_paquete *paquete) {
	char *namePokemon = list_get(msg_gameboy->argumentos, 0);
	int sizePokemonName = strlen(namePokemon) + 1;
	int posX = atoi(list_get(msg_gameboy->argumentos, 1));
	int posY = atoi(list_get(msg_gameboy->argumentos, 2));
	t_stream* buffer = malloc(sizeof(t_stream));
	buffer->size = sizeof(int) * 2 + sizePokemonName;
	buffer->data = malloc(buffer->size);
	int offset = 0;
	memcpy(buffer->data + offset, &(posX), sizeof(int));
	offset += sizeof(int);
	memcpy(buffer->data + offset, &(posY), sizeof(int));
	offset += sizeof(int);
	memcpy(buffer->data + offset, namePokemon, sizePokemonName);
	offset += sizePokemonName;

	paquete->codigo_operacion = APPEARED_TEAM;
	paquete->buffer = buffer;
	int tamano = tamano_paquete(paquete);

	log_info(g_logger, "(SENDING_MSG= %s | %d | %d -- SIZE = %d Bytes)",
			namePokemon, posX, posY, tamano);
}

void empaquetar_handshake_suscriptor(t_paquete *paquete)
{
	t_handsake_suscript *handshake = malloc(sizeof(t_handsake_suscript));

	handshake->id_suscriptor = g_config_gameboy->id_suscriptor;
	handshake->valor_handshake = HANDSHAKE_SUSCRIPTOR;
	handshake->msjs_recibidos = 0;
	t_stream *buffer = malloc(sizeof(t_stream));
	buffer->size = 2 * sizeof(int);
	void *stream = malloc(buffer->size);
	int offset = 0;

	memcpy(stream + offset, &(handshake->valor_handshake), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(handshake->id_suscriptor), sizeof(int));
	buffer->data = stream;
	paquete->buffer = buffer;
	free(handshake);
}

void empaquetar_fin_suscripcion(t_tipo_mensaje cola,int cant_mensajes,t_paquete *paquete)
{
	int id_suscriptor = g_config_gameboy->id_suscriptor;
	t_stream *buffer = malloc(sizeof(t_stream));
	buffer->size = 2 * sizeof(int);
	void *stream = malloc(buffer->size);
	int offset = 0;

	memcpy(stream + offset, &(id_suscriptor), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(cant_mensajes), sizeof(int));
	buffer->data = stream;
	paquete->buffer = buffer;
	char *name_cola = nombre_cola(cola);
	log_info(g_logger, "(SENDING END_SUSCRIPTION_TO = %s | ID_SUSCRIPTOR = %d | RCVD_MSGs = %d)"
			, name_cola, id_suscriptor, cant_mensajes);
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

void esperar_respuesta(int socket_cliente) {

	//printf("Entro a esperar respuesta \n");
	void *a_recibir;
	op_code codigo_operacion = recibir_op_code(socket_cliente);
	//printf("Recibi codigo de operacion : %d \n", codigo_operacion);
	int size;
	int id_mensaje;
	int pos_x;
	int pos_y;
	int size_pokemon;
	char *pokemon;
	int offset = 0;

	if (codigo_operacion == MSG_CONFIRMED) {
		a_recibir = recibir_buffer(&size, socket_cliente);
		log_info(g_logger, "(RECEIVING: %s)", a_recibir);
	}
	else if (codigo_operacion == MSG_ERROR) {
		a_recibir = recibir_buffer(&size, socket_cliente);
		log_info(g_logger, "(ANSWER_MSG | %s)", a_recibir);
	}
	else if (codigo_operacion == APPEARED_BROKER) {
		a_recibir = recibir_buffer(&size, socket_cliente);
		size_pokemon = size - 3 * sizeof(int);
		pokemon = malloc(size_pokemon);

		memcpy(&(id_mensaje), a_recibir + offset, sizeof(int));
		offset += sizeof(int);
		memcpy(&(pos_x), a_recibir + offset, sizeof(int));
		offset += sizeof(int);
		memcpy(&(pos_y), a_recibir + offset, sizeof(int));
		offset += sizeof(int);
		memcpy(pokemon, a_recibir + offset, size_pokemon);
		log_info(g_logger,
				"(RECEIVING: BROKER@APPEARED_POKEMON | ID_MENSAJE = %d | %s | POS_X = %d | POS_Y = %d)",
				id_mensaje, pokemon, pos_x, pos_y);
		free(pokemon);
	}
	else if (codigo_operacion == ID_MENSAJE) {
		a_recibir = recibir_buffer(&size, socket_cliente);
		memcpy(&(id_mensaje), a_recibir, size);
		log_info(g_logger, "(RECEIVING: ID_MENSAJE = %d)", id_mensaje);
	}
	else if (codigo_operacion == CAUGHT_BROKER) {
		a_recibir = recibir_buffer(&size, socket_cliente);
		t_result_caught resultado;
		memcpy(&(id_mensaje), a_recibir + offset, sizeof(int));
		offset += sizeof(int);
		memcpy(&(resultado), a_recibir + offset, sizeof(t_result_caught));
		log_info(g_logger,
				"(RECEIVING: BROKER@CAUGHT_POKEMON | ID_MENSAJE = %d | RESULTADO = %d)", id_mensaje, resultado);
	}
	else if (codigo_operacion == LOCALIZED_BROKER) {
		printf("Entro a LOCALIZED BROKER \n");
		a_recibir = recibir_buffer(&size, socket_cliente);
		t_list* posiciones = armar_lista(a_recibir, &size);
		//log_info(g_logger,
		//"(ANSWER_MSG | BROKER@LOCALIZED_POKEMON | ID_MENSAJE = %d | POKEMON = %s)",
		//id_mensaje, pokemon);
		//free(pokemon);
		list_destroy(posiciones);
	}
	free(a_recibir);
}

int recibir_op_code(int socket_cliente) {
	int code;
	recv(socket_cliente, &code, sizeof(int), MSG_WAITALL);
	return code;
}

void *recibir_buffer(int *size, int socket_cliente) {
	void* stream;
	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	stream = malloc(*size);
	recv(socket_cliente, stream, *size, MSG_WAITALL);
	return stream;
}

t_list* armar_lista(void* stream, int* tamanioTotal) {

	t_list* listaPosiciones = list_create();

	int cantPosiciones = (*tamanioTotal - 4)  / (3 * sizeof(int));
	printf("La cantidad de posiciones del pokemon son: %d \n", cantPosiciones);

	int offset = 0;

	int idMensajeUnico;

	memcpy(&(idMensajeUnico), stream + offset, sizeof(int));
	offset += sizeof(int);

	int cantidad;
	int pos_x;
	int pos_y;

	printf("Id mensaje unico X: %d \n", idMensajeUnico);

	while (offset < *tamanioTotal) {

		memcpy(&(cantidad), stream + offset, sizeof(int));
		offset += sizeof(int);

		memcpy(&(pos_x), stream + offset, sizeof(int));
		offset += sizeof(int);

		memcpy(&(pos_y), stream + offset, sizeof(int));
		offset += sizeof(int);

		t_posicion_pokemon *posicion = malloc(sizeof(t_posicion_pokemon));
		posicion->cantidad = cantidad;
		posicion->pos_x = pos_x;
		posicion->pos_y = pos_y;

		printf("Posicion X: %d \n", posicion->pos_x);
		printf("Posicion Y: %d \n", posicion->pos_y);
		printf("Cantidad: %d \n", posicion->cantidad);

		printf("El offset es : %d \n", offset);

		list_add(listaPosiciones, posicion);

	}

	return listaPosiciones;

}

void respuesta_publisher(op_code codigo_operacion, int socket_cliente) {
	switch(codigo_operacion){
	case COLA_VACIA:
		rcv_msg_cola_vacia(socket_cliente);
		break;
	case NEW_GAMECARD:
		rcv_msg_new(socket_cliente);
		break;
	case CATCH_GAMECARD:

		break;
	case GET_GAMECARD:
		rcv_msg_get(socket_cliente);
		break;
	case APPEARED_TEAM:

		break;
	case CAUGHT_TEAM:

		break;
	case LOCALIZED_TEAM:

		break;
	case 0:
		pthread_exit(NULL);
	case -1:
		pthread_exit(NULL);
	}
}

void rcv_msg_cola_vacia(int socket_cliente)
{
	int size;
	void *a_recibir = recibir_buffer(&size, socket_cliente);
	int id_suscriptor;
	memcpy(&id_suscriptor, a_recibir, size);
	free(a_recibir);
	if (id_suscriptor != g_config_gameboy->id_suscriptor) {
		log_error(g_logger,"ERROR_MSG EMPTY_QUEUE");
	}
	log_info(g_logger,"(RECEIVING: EMPTY_QUEUE | ID_SUSCRIPTOR = %d )", id_suscriptor);
}

void rcv_msg_get(int socket_cliente)
{
	int size;
	void *a_recibir = recibir_buffer(&size, socket_cliente);
	t_msg_get *msg_get = malloc(sizeof(t_msg_get));
	int offset = 0;
	int size_pokemon = size - sizeof(int);
	void *pokemon = malloc(size_pokemon);
	memcpy(&(msg_get->id_mensaje), a_recibir + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(pokemon, a_recibir + offset, size_pokemon);
	msg_get->pokemon = pokemon;
	log_info(g_logger,"(RECEIVING: GET_POKEMON | ID_MENSAJE = %d | Pokemon %s)",
			msg_get->id_mensaje, msg_get->pokemon);
	free(a_recibir);
	free(msg_get);
}

void rcv_msg_new(int socket_cliente)
{
	int size;
	void *a_recibir = recibir_buffer(&size, socket_cliente);
	t_msg_new *msg_new = malloc(sizeof(t_msg_new));
	int offset = 0;
	int size_pokemon = size - 4 * sizeof(int);
	void *pokemon = malloc(size_pokemon);
	memcpy(&(msg_new->id_mensaje), a_recibir + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&(msg_new->pos_x), a_recibir + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&(msg_new->pos_y), a_recibir + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&(msg_new->cantidad), a_recibir + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(pokemon, a_recibir + offset, size_pokemon);
	msg_new->pokemon = pokemon;
	log_info(g_logger,"(RECEIVING: NEW_POKEMON|ID_MSJ: %d|Pokemon: %s|POS_X: %d|POS_Y: %d|QTY: %d)",
			msg_new->id_mensaje, msg_new->pokemon, msg_new->pos_x, msg_new->pos_y, msg_new->cantidad);
	free(a_recibir);
	free(msg_new);
}

char* recibir_mensaje(int socket_cliente) {
	//printf("codigo operacion = %d \n", recibir_op_code(socket_cliente));
	int size;
	void* a_recibir = recibir_buffer(&size, socket_cliente);
	char* mensaje = malloc(size);

	memcpy(mensaje, a_recibir, size);
	free(a_recibir);
	return mensaje;
}

int tamano_paquete(t_paquete *paquete) {
	return paquete->buffer->size + sizeof(paquete->codigo_operacion)
			+ sizeof(paquete->buffer->size);
}

char *nombre_cola(t_tipo_mensaje cola)
{
	char *nombre_cola;
	switch (cola) {
		case NEW_POKEMON:
			nombre_cola = "NEW_POKEMON";
			break;
		case APPEARED_POKEMON:
			nombre_cola = "APPEARED_POKEMON";
			break;
		case CATCH_POKEMON:
			nombre_cola = "CATCH_POKEMON";
			break;
		case CAUGHT_POKEMON:
			nombre_cola = "CAUGHT_POKEMON";
			break;
		case GET_POKEMON:
			nombre_cola = "GET_POKEMON";
			break;
		case LOCALIZED_POKEMON:
			nombre_cola = "LOCALIZED_POKEMON";
			break;
		default:
			nombre_cola = "";
			break;
	}
	return nombre_cola;
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
	g_config_gameboy->time_suscrip_max = config_get_int_value(g_config,
			"TIME_SUSCRIP_MAX");
	g_config_gameboy->id_mensaje_unico = config_get_int_value(g_config,
			"ID_MENSAJE_UNICO");
	g_config_gameboy->id_suscriptor = config_get_int_value(g_config,
				"ID_SUSCRIPTOR");
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

void list_mostrar(t_list* lista)
{
	int i = 0;
	char* elemento;
	for(i=0;i<lista->elements_count;i++){
		elemento = list_get(lista,i);
		puts(elemento);
	}
}
