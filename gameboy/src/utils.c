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

int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
		printf("error");

	freeaddrinfo(server_info);

	return socket_cliente;
}

void cargar_argumentos(t_args_msg_gameboy *argumentos_mensaje, t_list *lista)
{
	char *proceso 		= list_get(lista, 0);
	char *tipo_mensaje 	= list_get(lista,1);

	if (strcmp(proceso,"SUSCRIPTOR") == 0) {
		argumentos_mensaje -> proceso = SUSCRIPTOR;
		argumentos_mensaje -> tipo_mensaje = select_tipo_mensaje(tipo_mensaje);
		borrar_comienzo(lista,2);
		list_add_all(argumentos_mensaje -> argumentos, lista);
	}
	else if (strcmp(proceso,"BROKER") == 0) {
		argumentos_mensaje -> proceso = BROKER;
		argumentos_mensaje -> tipo_mensaje = select_tipo_mensaje(tipo_mensaje);
		borrar_comienzo(lista,2);
		list_add_all(argumentos_mensaje -> argumentos, lista);
	}
	else if (strcmp(proceso,"GAMECARD") == 0) {
		argumentos_mensaje -> proceso = GAMECARD;
		argumentos_mensaje -> tipo_mensaje = select_tipo_mensaje(tipo_mensaje);
		borrar_comienzo(lista,2);
		list_add_all(argumentos_mensaje -> argumentos, lista);
	}
	else if (strcmp(proceso,"TEAM") == 0) {
		argumentos_mensaje -> proceso = TEAM;
		argumentos_mensaje -> tipo_mensaje = select_tipo_mensaje(tipo_mensaje);
		borrar_comienzo(lista,2);
		list_add_all(argumentos_mensaje -> argumentos, lista);
	}
	else {
		puts("Argumentos Incorrectos!");
		argumentos_mensaje -> proceso = UNKNOWN_PROC;
		argumentos_mensaje -> tipo_mensaje = UNKNOWN_QUEUE;
		list_clean(argumentos_mensaje -> argumentos);

	}
}

t_tipo_mensaje select_tipo_mensaje(char * valor)
{
		if (strcmp(valor,"NEW_POKEMON") == 0) {
 			return NEW_POKEMON;
 		}
 		else if (strcmp(valor,"APPEARED_POKEMON") == 0) {
 			return APPEARED_POKEMON;
		}
 		else if (strcmp(valor,"CATCH_POKEMON") == 0) {
 		 	return CATCH_POKEMON;
 		}
 		else if (strcmp(valor,"CAUGHT_POKEMON") == 0) {
 			return CAUGHT_POKEMON;
 		}
 		else if (strcmp(valor,"GET_POKEMON") == 0) {
 			return GET_POKEMON;
 		}
 		else {
 			printf("Argumentos Incorrectos!");
 			return UNKNOWN_QUEUE;
 		}
}

//TODOlisto
void enviar_mensaje(char* mensaje, int socket_cliente)
{
	t_buffer* 	_buffer;
	void* 		_stream;
	t_paquete* 	paquete;
	void* 		a_enviar;
	int 		bytes;

	_buffer = malloc(sizeof(t_buffer));
	_buffer -> size = strlen(mensaje) + 1;		//sumamos caracter centinela

	_stream = malloc(_buffer -> size);
	memcpy(_stream, mensaje, _buffer -> size);
	_buffer -> stream = _stream;

	paquete =  malloc(sizeof(t_paquete));
	paquete -> codigo_operacion = MENSAJE;
	paquete -> buffer = _buffer;

	bytes = _buffer -> size + 2 * (sizeof(int));

	// tamaño del mensaje en bytes (= _buffer -> size) + el tamaño de 2 int: code_op + buffer->size

	a_enviar = serializar_paquete(paquete, &bytes);

	send(socket_cliente, a_enviar, bytes, 0 );

	free(a_enviar);
	eliminar_paquete(paquete);
}

void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

//TODOlisto
/*
 * Recibe un paquete a serializar, y un puntero a un int en el que dejar
 * el tamaño del stream de bytes serializados que devuelve
 */
void* serializar_paquete(t_paquete* paquete, int* bytes)
{
	void* a_enviar = malloc(*bytes);
 	int offset = 0;

	memcpy(a_enviar + offset , &(paquete->codigo_operacion), sizeof(int));
	offset+= sizeof(int);
	memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(int));
	offset+= sizeof(int);
	memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);
	offset += paquete->buffer->size;

	return a_enviar;
}

// tODOlisto

char* recibir_mensaje(int socket_cliente)
{
	printf("codigo operacion = %d \n", recibir_op_code(socket_cliente));

	int 		size;
	void* 		a_recibir	= recibir_buffer(&size, socket_cliente);
	char* 		mensaje		= malloc(size);

	memcpy(mensaje, a_recibir, size);
	free(a_recibir);
	return mensaje;
}

int recibir_op_code(int socket_cliente)
{
	int code;
	recv(socket_cliente, &code, sizeof(int), MSG_WAITALL);
	return code;
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void* _stream;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);  // el argumento size es un puntero
	_stream = malloc(*size);
	recv(socket_cliente, _stream, *size, MSG_WAITALL);

	return _stream;
}

void borrar_comienzo(t_list* lista, int cant)
{
	if (lista->elements_count >= cant) {
		int i = 0;
		for (i = 0; i < cant; i++) {
			list_remove(lista,0);
		}
	}
}

void iniciar_log(void)
{
	g_logger = log_create("/home/utnso/workspace/tp-2020-1c-Los-Que-Aprueban/gameboy/src/log/gameboy.log", "GAME_BOY", 1, LOG_LEVEL_INFO);
}

void leer_config(char *path)
{
	g_config = config_create(path);
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}
