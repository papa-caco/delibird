/*
 * cliente.c
 *
 *  Created on: 23 abr. 2020
 *      Author: utnso
 */
#include "utilsGc.h"
#include "cliente.h"
#include "servidor.h"

//TODO
//POR AHORA OBVIO EL CANAL,
//ES UNA SUSCRIPCION GENERAL

void* serializar_paquete_cliente(t_paquete* paquete, int *bytes)
{
	int offset = 0;
	*bytes = paquete->buffer->size + 2*sizeof(int);
	void* aEnviar = malloc(*bytes);
	memcpy(aEnviar + offset , &(paquete->codigo_operacion),sizeof(int));
	offset += sizeof(int);
	memcpy(aEnviar + offset , &(paquete->buffer->size),sizeof(int));
	offset += sizeof(int);
	memcpy(aEnviar + offset , paquete->buffer->data,paquete->buffer->size);
	return aEnviar;
}

char* recibir_mensaje_broken(int socket_cliente)
{
	log_info(g_logger,"RECIBIENDO MENSAJE");
	char* mensaje;
	int cod_op,size;
	recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL); // Primero recibimos el codigo de operacion
	recv(socket_cliente, &size, sizeof(int), MSG_WAITALL);//recibimos el buffer: su tama;o

	mensaje= malloc(size);
	recv(socket_cliente, mensaje, size, MSG_WAITALL);
	return mensaje;
	/*
	switch( cod_op ) {
		case SUSCRIBE:
			recv(socket_cliente, &size, sizeof(int), MSG_WAITALL);//recibimos el buffer: su tama;o

			mensaje= malloc(size);
			recv(socket_cliente, mensaje, size, MSG_WAITALL);
			break;
		default:
			printf("RecibirMensaje -> Error OpCode: %d.\n", cod_op);
			break;
	}
	return mensaje;

	*/
}
int recibir_id_mensaje(int socket_cliente)
{
	void* buffer;
	int cod_op,size,idMensaje;
	recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL); // Primero recibimos el codigo de operacion
	recv(socket_cliente, &size, sizeof(int), MSG_WAITALL);//recibimos el buffer: su tama;o

	buffer = malloc(size);
	recv(socket_cliente, buffer, size, MSG_WAITALL);
	memcpy(&idMensaje, buffer, sizeof(int));

	//recv(socket_cliente, &idMensaje, size, MSG_WAITALL);
	return idMensaje;
	/*
	switch( cod_op ) {
		case MENSAJE:
			recv(socket_cliente, &size, sizeof(int), MSG_WAITALL);//recibimos el buffer: su tama;o

			mensaje= malloc(size);
			recv(socket_cliente, mensaje, size, MSG_WAITALL);
			break;
		default:
			printf("RecibirMensaje -> Error OpCode: %d.\n", cod_op);
			break;
	}
	return mensaje;*/
}

/**
 * Envia mensaje a un canal
 */
void enviar_mensaje_al_canal(char* mensaje,int canal,int socket_cliente){
	t_paquete *paquete = malloc(sizeof(t_paquete));
	void *a_enviar;
	int bytes = 0;

	empaquetar_mensaje(mensaje,canal,paquete);


	/*
	switch (tipo_mensaje) {
		case APPEARED_POKEMON:
			empaquetar_appeared_broker(msg_gameboy, paquete);
			break;
		case LOCALIZED_POKEMON:
			empaquetar_localized_broker(msg_gameboy, paquete);
			break;
		case CAUGHT_POKEMON:
			empaquetar_caught_broker(msg_gameboy, paquete);
			break;
	}
*/
	//bytes = paquete->buffer->size + 2 * (sizeof(int));

	a_enviar = serializar_paquete_cliente(paquete, &bytes);

	// send(socket_cliente, a_enviar, bytes, 0);
	int tamanio = tamano_paquete(paquete);

	if( send(socket_cliente, a_enviar, bytes, 0) == -1 ){
		log_error(g_logger, "(SENDING_MSG NOT FOUND | MSG = %s | CANAL= %d | -- SIZE= %d Bytes)", mensaje, canal, tamanio);
	}
	else{
		log_info(g_logger, "(SENDING_MSG OK | MSG = %s | CANAL= %d | -- SIZE= %d Bytes)", mensaje, canal, tamanio);
	}

	free(a_enviar);
	eliminar_paquete(paquete);
}

void empaquetar_mensaje(char* mensaje,int canal,t_paquete *paquete){

	t_stream *buffer = malloc(sizeof(t_stream));
	buffer->size =  sizeof(int) + strlen(mensaje) + 1;

	void *stream = malloc(buffer->size);

	int offset = 0;
	memcpy(stream + offset, &(canal), sizeof(int));
	offset += sizeof(int);
	int tamanioMensaje = strlen(mensaje);
	memcpy(stream + offset, &tamanioMensaje, sizeof(int));
	offset += sizeof(int);

	memcpy(stream + offset, mensaje, strlen(mensaje) + 1);
	buffer->data = stream;

	paquete->codigo_operacion = NEW_POKEMON;
	paquete->buffer = buffer;


}

int tamano_paquete(t_paquete *paquete){
	return paquete->buffer->size + sizeof(paquete->codigo_operacion) + sizeof(paquete->buffer->size);
}






