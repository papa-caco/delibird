/*
 * servidor.c
 *
 *  Created on: 21 abr. 2020
 *      Author: utnso
 */
#include "servidor.h"
#include "utils.h"


void iniciar_servidor(void)
{
	int socket_servidor;
    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    t_config* config = leer_config();
    char* ip = config_get_string_value(config, "IP_BROKER");
	char* puerto = config_get_string_value(config, "PUERTO_BROKER");

    getaddrinfo(ip, puerto, &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next)
    {
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            continue;
        }
        break;
    }
	if( socket_servidor == -1 ){
    	log_error(logger,	"(CONN_SUCESS |  LOCAL_IP=%s | PORT=%s  | SOCKET=%d)", ip, puerto, socket_servidor);
    }
    else{
    	log_info(logger,"(CONN_SUCCESS | SOCKET=%d |  REMOTE_IP=%s | PORT=%s)",socket_servidor, ip, puerto);
    }
	listen(socket_servidor, SOMAXCONN);

    freeaddrinfo(servinfo);

    while(1)
    	esperar_cliente(socket_servidor);
}

void esperar_cliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;
	int tam_direccion = sizeof(struct sockaddr_in);
	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	pthread_create(&thread,NULL,(void*)serve_client,&socket_cliente);
	pthread_detach(thread);
}

void serve_client(int* socket)
{
	int cod_op;
	if(recv(*socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		cod_op = -1;
	process_request(cod_op, *socket);
}
/**
 * COD_OP = SUSCRIBE
 * payload = cola de mensaje
 */
void process_request(int cod_op, int cliente_fd) {
	int size;
	void* msg;
	int i;
	log_info(logger,"(COD_OP %i",cod_op);
	//TODO
	// VALIDAR cod_op
		switch (cod_op) {
			case SUSCRIBE: //Se registra la suscripcion

				//msg = recibir_mensaje(cliente_fd, &size);
				log_info(logger,"solicitud de suscripcion ");
				suscribir_proceso(cliente_fd);
				break;
			case MENSAJE:
				msg = procesar_mensaje(cliente_fd,&i,&size);

				log_info(logger,"mensaje %s",msg);
				devolver_id_mensaje(i, cliente_fd);
				enviar_mensaje_a_suscriptores(msg,size);
				free(msg);
				break;
			default:
				log_error(logger,"(OPERACION_NO_MATCH %i ",cod_op);
				pthread_exit(NULL);
		}
}

void* recibir_mensaje(int socket_cliente, int* size)
{
	void * buffer;
	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);
	return buffer;
}

void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}

void devolver_mensaje(void* payload, int size, int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = size;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, payload, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);
	log_info(logger,"mensaje %s cliente %d",payload, socket_cliente);
	if(send(socket_cliente, a_enviar, bytes, 0) ==-1){
		log_error(logger,"No se puede enviar mensaje a suscriptor");
	}
	else{
		log_info(logger,"mensaje enviado a suscriptor ok");
	}
	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}


void suscribir_proceso(int cliente){

	int size,socket_suscriptor;
	void* buffer = recibir_mensaje(cliente,&size);

	memcpy(&socket_suscriptor, buffer, sizeof(int));
	log_info(logger,"socket cliente  %d", socket_suscriptor);
	log_info(logger,"(SUSCRIPCION_SUCCESS | PROCESO=%d |  COLA_DE_MENSAJE=%s)",socket_suscriptor, "COLA_GENERICA");
	suscriptores.suscriptor[0]= socket_suscriptor;
	free(buffer);

}
/**
 * guarda el mensaje en la cola de mensajes del canal
 */
void* procesar_mensaje(int socket_cliente ,int* idMensaje, int* sizeMensaje){
	void * buffer;
	void* payload;
	int size;
	int canal;


	recv(socket_cliente, &size, sizeof(int), MSG_WAITALL);
	buffer = malloc(size);
	recv(socket_cliente, buffer, size, MSG_WAITALL);


	memcpy(&canal, buffer, sizeof(int));
	buffer += sizeof(int);

	memcpy(sizeMensaje, buffer, sizeof(int));

	buffer += sizeof(int);
	payload = malloc(*sizeMensaje);
	memcpy(payload, buffer, *sizeMensaje);

	//
	memcpy(payload, buffer, sizeof(int));

	log_info(logger,"procesando mensaje c %d tam %d msg %s", canal,*sizeMensaje,payload);
	*idMensaje = 1;
	return payload;//TOD retornar entero unico
}

void devolver_id_mensaje(int idMensaje,  int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = sizeof(int);
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, &idMensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	if(send(socket_cliente, a_enviar, bytes, 0) == -1 ){
		log_error(logger,"ENVIO ERROR");
	}
	else{
		log_info(logger,"ENVIO OK");
	}


	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

//TODO

void enviar_mensaje_a_suscriptores(void* payload, int size){
	int socket_cliente = suscriptores.suscriptor[0];
	log_info(logger,"INICIO ENVIO MENSAJES AL SUSCRIPTOR %d %s",socket_cliente, payload);
	devolver_mensaje(payload, size,socket_cliente);
}
