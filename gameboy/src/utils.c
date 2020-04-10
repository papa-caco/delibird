/*
 *		utils.c
 *
 *  	Created on: 1 apr. 2020
 *
 *      Author:  papa-caco
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

char* leer_consola(void)
{
	printf("Ingrese el mensaje a enviar:");
	char* a_leer = readline(">");
	rl_clear_history();
	return(a_leer);
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


t_log* inicio_log(void)
{
	return log_create("log/tp0.log", "TP0-R", 1, LOG_LEVEL_INFO);
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}
