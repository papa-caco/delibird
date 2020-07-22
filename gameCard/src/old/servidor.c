/*
 * servidor.c
 *
 *  Created on: 21 abr. 2020
 *      Author: utnso
 */
#include "utilsGc.h"
#include "servidor.h"


/*
////////////


void* recibir_mensaje(int socket_cliente, int* size)
{
	void * buffer;
//log_info(logger,"Decodificando Mensaje recibido");
	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}



void devolver_mensaje(void* payload, int size, int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = SUSCRIBE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = size;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, payload, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}
//TODO
void suscribirse( int socket_gamecard)
{
	int conexion_broker = crear_conexion();
	int estado=0,bytes = 0;
	char* mensaje="";
	t_buffer* buffer = (t_buffer*) malloc(sizeof(t_buffer));
	buffer->size =  sizeof(int);
	void* stream =  malloc(buffer->size);
	memcpy(stream , &socket_gamecard,sizeof(int));
	buffer->stream = stream;
	//Armo el paquete
	t_paquete* paquete = (t_paquete*) malloc(sizeof(t_paquete));
	paquete->codigo_operacion = SUSCRIBE;
	paquete->buffer = buffer;

	void* aEnviar =  serializar_paquete_cliente(paquete,&bytes);
	//envio
	if( send(conexion_broker, aEnviar, bytes, 0) == -1 ){
		log_error(g_logger,"SUSCRIPTION FAILED %socket_cliente");
	}
	else{
		log_info(g_logger,"SUSCRIPTION OK  socket_gamecard %d", socket_gamecard);
	}
	eliminar_paquete(paquete);
}
*/

