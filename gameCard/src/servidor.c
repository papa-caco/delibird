/*
 * servidor.c
 *
 *  Created on: 21 abr. 2020
 *      Author: utnso
 */
#include "utilsGc.h"
#include "servidor.h"


int crear_conexion(){
	struct addrinfo hints;
	struct addrinfo *server_info;

	t_config* config = leer_config();
	char *ip = config_get_string_value(config, "IP_BROKER");
	char* puerto = config_get_string_value(config, "PUERTO_BROKER");

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;



	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1){
		//error
		log_error(logger,	"(CONN_FAILED |  REMOTE_IP=%s | PORT=%s)", ip, puerto);
	}
	else{
		log_info(logger,"(CONN_SUCCESS | SOCKET=%d |  REMOTE_IP=%s | PORT=%s)",
						socket_cliente, ip, puerto);
	}

	freeaddrinfo(server_info);

	return socket_cliente;
}

void iniciar_servidor(int* servidor)
{
	int socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    t_config* config = leer_config();
    char* ip = config_get_string_value(config, "IP_LOCAL");
	char* puerto = config_get_string_value(config, "PUERTO_LOCAL");

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
    	log_error(logger,	"(SERVER_ERROR |  LOCAL_IP=%s | PORT=%s  | SOCKET=%d)", ip, puerto, socket_servidor);
    }
    else{
    	log_info(logger,"(SERVER_SUCCESS | SOCKET=%d |  REMOTE_IP=%s | PORT=%s)",socket_servidor, ip, puerto);
    }
	listen(socket_servidor, SOMAXCONN);
	servidor = &socket_servidor;
//log_info(logger,"socket servidor %d %d",*servidor,socket_servidor);
suscribirse(*servidor);
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
	log_info(logger,"socket cliente, %d",*socket);
}
/**
 * COD_OP = SUSCRIBE
 * payload = cola de mensaje
 */
void process_request(int cod_op, int cliente_fd) {
	int size;
	void* msg;

		switch (cod_op) {
		case MENSAJE: //Se registra la suscripcion
			msg = recibir_mensaje(cliente_fd, &size);
			devolver_mensaje(msg, size, cliente_fd);
			free(msg);
			break;
		default:
			pthread_exit(NULL);
		}
	log_info(logger,"COMO SERVIDOR RECIBO MENSAJE", msg);
}

void* recibir_mensaje(int socket_cliente, int* size)
{
	void * buffer;
//log_info(logger,"Decodificando Mensaje recibido");
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
		log_error(logger,"SUSCRIPTION FAILED %socket_cliente");
	}
	else{
		log_info(logger,"SUSCRIPTION OK  socket_gamecard %d", socket_gamecard);
	}
	eliminar_paquete(paquete);
}


