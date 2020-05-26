/*
 * teamInitializer.c
 *
 *  Created on: 30 abr. 2020
 *      Author: utnso
 */

#include"teamInitializer.h"




void iniciar_team(void){
	iniciar_logger();
	iniciar_servidor();
}

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


		char** posiciones = string_split(listaConfig[procesados], "|");

		int posicionX = atoi(posiciones[0]);
		int posicionY = atoi(posiciones[1]);

		t_posicion_entrenador *posicion = malloc(sizeof(t_posicion_entrenador));

		posicion->pos_x = posicionX;
		posicion->pos_y = posicionY;


		list_add(posicionesEntrenadores, posicion);


	}

	config_destroy(config);

	return posicionesEntrenadores;
}

t_list *extraer_pokemones_entrenadores(char* configKey){
	//Inicializo la lista, que contendrá listas de objetivos de todos los entrenadores
	t_list* objetivosEntrenadores = list_create();


	t_config* config = config_create("/home/utnso/config/team.config");

	//Inicializo el array que contiene los objetivos separados por ","
	char ** arrayConfig = config_get_array_value(config, configKey);



	for (int i = 0; arrayConfig[i] != NULL; i++){
		printf("----------CAMBIO DE ENTRENADOR ------- \n");
		//Inicializo lista que contendrá objetivos
		t_list* objetivosUnEntrenador = list_create();


		//Ahora recibo un array con todos los pokemones (tal vez se repitan) que necesita
		//el entrenador "i" pero sin el "|"
		char** pokemonesObjetivo = string_split(arrayConfig[i], "|");



		for(int j=0; pokemonesObjetivo[j] != NULL; j++){
			t_pokemon_entrenador *objetivo = malloc(sizeof(t_pokemon_entrenador));


			t_pokemon_entrenador* pokemonEncontrado = list_buscar(objetivosUnEntrenador, pokemonesObjetivo[j]);

			if(pokemonEncontrado != NULL){
				printf("Encontro el pokemon %s \n", pokemonesObjetivo[j]);
				objetivo = pokemonEncontrado;
				objetivo->cantidad++;
			}
			else{
				printf("Hay un nuevo pokemon que es %s \n", pokemonesObjetivo[j]);
				objetivo->pokemon = pokemonesObjetivo[j];
				objetivo->cantidad = 1;

				list_add(objetivosUnEntrenador, objetivo);
			}

		}

		list_add(objetivosEntrenadores, objetivosUnEntrenador);

	}

	config_destroy(config);


	return objetivosEntrenadores;
}



void iniciar_entrenadores_and_objetivoGlobal(){
	colaNewEntrenadores = queue_create();
	objetivoGlobalEntrenadores = list_create();
	t_list* objetivosEntrenadores = extraer_pokemones_entrenadores("OBJETIVOS_ENTRENADORES");
	t_list* pokemonesObtenidos = extraer_pokemones_entrenadores("POKEMON_ENTRENADORES");
	t_list* posiciones = extraer_posiciones_entrenadores();
	cargar_objetivo_global(objetivosEntrenadores);


	for(int i=0; list_get(posiciones, i) != NULL; i++){
		t_entrenador* unEntrenador = malloc(sizeof(t_entrenador));
		unEntrenador->posicion = (t_posicion_entrenador*)list_get(posiciones, i);
		unEntrenador->objetivoEntrenador = (t_list*)list_get(objetivosEntrenadores, i);
		unEntrenador->pokemonesObtenidos = (t_list*)list_get(pokemonesObtenidos, i);
		queue_push(colaNewEntrenadores, unEntrenador);
	}
	//Al finalizar el programa vamos a tener que destruir la lista de entrenadores, lo cual implicará destruir
	//también cada una de las listas que creamos acá para llenar a cada uno

}

void cargar_objetivo_global(t_list* objetivosEntrenadores){
	for(int i=0; list_get(objetivosEntrenadores, i) != NULL; i++){
		t_list* objetivosUnEntrenador = (t_list*)list_get(objetivosEntrenadores, i);


		for(int j=0; list_get(objetivosUnEntrenador, j) != NULL; j++){
			t_pokemon_entrenador *pokemonNuevo = malloc(sizeof(t_pokemon_entrenador));
			t_pokemon_entrenador* pokemonEncontrado = list_buscar(objetivoGlobalEntrenadores, ((t_pokemon_entrenador*)list_get(objetivosUnEntrenador, j))->pokemon);
			if(pokemonEncontrado != NULL){
				pokemonEncontrado -> cantidad+=((t_pokemon_entrenador*)list_get(objetivosUnEntrenador, j))->cantidad;
			}
			else{
				pokemonNuevo -> pokemon = ((t_pokemon_entrenador*)list_get(objetivosUnEntrenador, j))->pokemon;
				pokemonNuevo -> cantidad = ((t_pokemon_entrenador*)list_get(objetivosUnEntrenador, j))->cantidad;
				list_add(objetivoGlobalEntrenadores, pokemonNuevo);
			}

		}
	}
}

//------------FUNCION QUE BUSCA UN ELEMENTO DENTRO DE UNA LISTA Y SI LO ENCUENTRA, LO RETORNA, SI NO, NULL

t_pokemon_entrenador* list_buscar(t_list* lista, char* elementoAbuscar){

	for(int i = 0; list_get(lista, i) != NULL; i++){

		printf("El valor de la lista procesados es %s \n", ((t_pokemon_entrenador*)list_get(lista, i))->pokemon);
		printf("El elemento a comparar es %s \n", elementoAbuscar);
		if(strcmp(((t_pokemon_entrenador*)list_get(lista, i))->pokemon, elementoAbuscar) == 0){

			printf("Entro al if \n");
			return (t_pokemon_entrenador*)list_get(lista, i);

		}
	}
	return NULL;
}

//--------------FUNCIONES DE DESTRUCCIÓN DE ELEMENTOS DE CADA LISTA UTILIZADA--------------------
void destroy_posicion_entrenador(t_posicion_entrenador* posicion){
	free(posicion);
}

void destroy_pokemon_entrenador(t_pokemon_entrenador* objetivoEntrenador){
	free(objetivoEntrenador->pokemon);
	free(objetivoEntrenador);
}

void destroy_pokemonProcesado(char* pokemonProcesado){
	free(pokemonProcesado);
}

void liberar_lista(t_list* lista) {

    int contador = 0;
    while (list_get(lista, contador) != NULL) {
        free(list_get(lista,contador));
        contador++;
    }

    free(lista);
}

void liberar_cola(t_queue* cola) {

    while (queue_is_empty(cola)==0) {
        free(queue_pop(cola));
    }

    free(cola);
}

/*void destroy_entrenador(t_entrenador* entrenador){
	list_destroy_and_destroy_elements(entrenador->objetivoEntrenador, destroy_pokemon_entrenador);
	list_destroy_and_destroy_elements(entrenador->pokemonesObtenidos, destroy_pokemon_entrenador);
	free(entrenador);
}*/


//Con las tres funciones anteriores ya podemos crear dos funciones: Una que defina el objetivo
//global del Team, y otra que inicialice a cada uno de los entrenadores con sus posiciones
//y sus objetivos particulares
