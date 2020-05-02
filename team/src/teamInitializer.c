/*
 * teamInitializer.c
 *
 *  Created on: 30 abr. 2020
 *      Author: utnso
 */

#include"teamInitializer.h"
#include"utilsTeam.h"

void iniciar_servidor(void) {

	int socket_servidor;

	struct addrinfo hints, *servinfo, *p; 	//hints no es puntero

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;	// No importa si uso IPv4 o IPv6 - vale 0
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP
	hints.ai_flags = AI_PASSIVE;// Asigna el address del localhost: 127.0.0.1

	getaddrinfo(IP, PUERTO, &hints, &servinfo);

	log_info(g_logger, "Direccion: %s, Port: %s", IP, PUERTO);

	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((socket_servidor = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1)
			continue;

		if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
			close(socket_servidor);
			continue;
		}
		break;
	}

	listen(socket_servidor, SOMAXCONN);	// Maximum queue length specifiable by listen = 128 (default)

	freeaddrinfo(servinfo);

	while (1)
		esperar_cliente(socket_servidor);
}

void esperar_cliente(int socket_servidor) {
	struct sockaddr_in dir_cliente;

	socklen_t tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente,
			&tam_direccion);

	t_socket_cliente *socket = malloc(sizeof(t_socket_cliente));
	socket->cliente_fd = socket_cliente;
	// inicializa contador de mensajes de algun cliente suscriptor
	socket->cant_msg_enviados = 0; //VERIFICAR COMPORTAMIENTO

	pthread_create(&thread, NULL, (void*) serve_client, socket);
	pthread_detach(thread);
}

void serve_client(t_socket_cliente *socket) {
	int cod_op;
	int cliente_fd = socket->cliente_fd;
	// OBTENGO el Codigo_Operacion de los  Mensajes que Recibo
	if (recv(cliente_fd, &cod_op, sizeof(int), MSG_WAITALL) == -1) {
		cod_op = -1;
	}
	process_request(cod_op, socket);
}
// RECIBE todos los TIPOS de MENSAJE QUE MANEJA el GAMEBOY y resuelve segun el CODIGO_OPERACION del MENSAJE
void process_request(op_code cod_op, t_socket_cliente *socket) {
	int size;
	int cliente_fd = socket->cliente_fd;

	void* msg;

	switch (cod_op) {
	case ID_MENSAJE:
		break;
	case APPEARED_TEAM:
		log_info(g_logger, "(RECEIVING: TEAM@APPEARED_POKEMON | Socket#: %d)",
				cliente_fd);
		msg = rcv_appeared_team(cliente_fd, &size);
		devolver_recepcion_ok(cliente_fd);
		// El GameBoy no tiene que recibir ninguna repuesta en este tipo de mensaje.
		break;
	case 0:
		pthread_exit(NULL);
	case -1:
		pthread_exit(NULL);
	}
	free(msg);
	free(socket);
}

void iniciar_logger(void) {
	g_logger = log_create("/home/utnso/logs/server.log", "SERVER", 1,
			LOG_LEVEL_INFO);
}

t_list * extraer_posiciones_entrenadores() {

	t_list *posicionesEntrenadores = list_create();

	t_config* config = config_create("/home/utnso/config/team.config");

	char ** listaConfig = config_get_array_value(config,
			"POSICIONES_ENTRENADORES");



	for (int procesados = 0; listaConfig[procesados] != NULL; procesados++) {

		printf("-------------------El contenido de la posicion es %s \n",
				listaConfig[procesados]);

		char** posiciones = string_split(listaConfig[procesados], "|");

		printf(
				"-------------------El contenido de la posicion SIN BARRA es %s \n",
				posiciones[0]);
		printf(
				"-------------------El contenido de la posicion SIN BARRA es %s \n",
				posiciones[1]);



		int posicionX = atoi(posiciones[0]);
		int posicionY = atoi(posiciones[1]);

		t_posicion_entrenador *posicion = malloc(sizeof(t_posicion_entrenador));

		posicion->pos_x = posicionX;
		posicion->pos_y = posicionY;

		printf("Posicion X: %d \n", posicion->pos_x);
		printf("Posicion Y: %d \n", posicion->pos_y);
		printf("------------------------------ \n");

		list_add(posicionesEntrenadores, posicion);


	}
		//free(posicion); VERIFICAR FREE


	return posicionesEntrenadores;
}

