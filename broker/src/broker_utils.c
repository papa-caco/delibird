/*
 ============================================================================
 Name        : broker_utils.c

 Author      : Los Que Aprueban.

 Created on	 : 9 may. 2020

 Version     :

 Description :
 ============================================================================
 */

#include "broker_utils.h"

void inicializar_server_broker(char *ip, char *puerto, pthread_mutex_t mutex_clientes, t_log* logger)
{
	int socket_servidor, status;
	struct sockaddr_storage dir_cliente;
	socklen_t tam_direccion;
	struct addrinfo hints, *servinfo; 	//hints no es puntero
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;			// No importa si uso IPv4 o IPv6 - vale 0
	hints.ai_socktype = SOCK_STREAM;		// Indica que usaremos el protocolo TCP
	hints.ai_flags = AI_PASSIVE;			// Asigna el address del localhost: 127.0.0.1

	getaddrinfo(ip, puerto, &hints, &servinfo);

	if ((status = getaddrinfo(NULL, puerto, &hints, &servinfo)) != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
			exit(EXIT_FAILURE);
	}
	log_trace(logger, "(Esperando conexiones en Direccion: %s, Puerto: %s)", ip, puerto);
	socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	int yes = 1;
	setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);

	if (listen(socket_servidor, 10) == -1) {
		perror("listen");
	}

	freeaddrinfo(servinfo);

	while (1){
		tam_direccion = sizeof(dir_cliente);

		int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);
		pthread_mutex_lock(&mutex_clientes);
		pthread_t pid;
		t_socket_cliente_broker *socket = malloc(sizeof(t_socket_cliente_broker));
		socket->cliente_fd = socket_cliente;
		// inicializa contador de mensajes enviados al cliente que se conectó.
		socket->cant_msg_enviados = 0;
		int thread_status = pthread_create(&pid, NULL, (void*) atender_cliente_broker,(void*) socket);
		pthread_mutex_unlock(&mutex_clientes);
		if( thread_status != 0 ){
			log_error(logger, "Thread create returno %d", thread_status );
			log_error(logger, "Thread create returno %s", strerror( thread_status ) );
		} else {
			pthread_detach( pid );
		}
	}
}

void inicio_server_broker(void)
{
	char *ip = g_config_broker->ip_broker;
	char *puerto = g_config_broker->puerto_broker;
	inicializar_server_broker(ip, puerto, g_mutex_msjs, g_logger);
}

void atender_cliente_broker(t_socket_cliente_broker *socket)
{
	int cliente_fd = socket->cliente_fd;
	op_code cod_op = rcv_codigo_operacion(cliente_fd);
	if (!codigo_operacion_valido_broker(cod_op)) {
		log_error(g_logger,"(SUSCRIBER DISCONNECTED: Socket:%d)", cliente_fd);
		close(cliente_fd);
	}
	else if (cod_op < SUSCRIBER_ACK) {
		atender_publicacion(cod_op, socket);
	} else {
		atender_suscripcion(cod_op, socket);
	}
}

void atender_publicacion(op_code cod_op, t_socket_cliente_broker *socket)
{
	int cliente_fd = socket->cliente_fd;
	puts("");
	pthread_mutex_lock(&g_mutex_msjs);
	log_debug(g_logger,"(NEW CLIENT CONNECTED | SOCKET#:%d)", cliente_fd);
	switch (cod_op) {
	case GET_BROKER:;
		t_msg_get_broker *msg_get = rcv_msj_get_broker(cliente_fd, g_logger);
		pthread_mutex_unlock(&g_mutex_msjs);
		if(cache_espacio_suficiente(espacio_cache_msg_get(msg_get))) {
			pthread_mutex_lock(&g_mutex_msjs);
				enqueue_msg_get(msg_get, g_logger, cliente_fd);
			pthread_mutex_unlock(&g_mutex_msjs);
		} else {
			enviar_mensaje_cache_sin_espacio(cliente_fd, g_logger);
			eliminar_msg_get_broker(msg_get);
		}
		break;
	case NEW_BROKER:;
		t_msg_new_broker *msg_new = rcv_msj_new_broker(cliente_fd, g_logger);
		pthread_mutex_unlock(&g_mutex_msjs);
		int size_msg = espacio_cache_msg_new(msg_new);
		if(cache_espacio_suficiente(espacio_cache_msg_new(msg_new))) {
			pthread_mutex_lock(&g_mutex_msjs);
				enqueue_msg_new(msg_new, g_logger, cliente_fd);
			pthread_mutex_unlock(&g_mutex_msjs);
		} else {
			enviar_mensaje_cache_sin_espacio(cliente_fd, g_logger);
			eliminar_msg_new_broker(msg_new);
		}
		break;
	case CATCH_BROKER:;
		t_msg_catch_broker *msg_catch = rcv_msj_catch_broker(cliente_fd, g_logger);
		pthread_mutex_unlock(&g_mutex_msjs);
		if(cache_espacio_suficiente(espacio_cache_msg_catch(msg_catch))) {
			pthread_mutex_lock(&g_mutex_msjs);
				enqueue_msg_catch(msg_catch, g_logger, cliente_fd);
			pthread_mutex_unlock(&g_mutex_msjs);
		} else {
			enviar_mensaje_cache_sin_espacio(cliente_fd, g_logger);
			eliminar_msg_catch_broker(msg_catch);
		}
		break;
	case APPEARED_BROKER:;
		t_msg_appeared_broker *msg_appeared = rcv_msj_appeared_broker(cliente_fd, g_logger);
		pthread_mutex_unlock(&g_mutex_msjs);
		if(cache_espacio_suficiente(espacio_cache_msg_appeared(msg_appeared))) {
			pthread_mutex_lock(&g_mutex_msjs);
				enqueue_msg_appeared(msg_appeared, g_logger, cliente_fd);
			pthread_mutex_unlock(&g_mutex_msjs);
		} else {
			enviar_mensaje_cache_sin_espacio(cliente_fd, g_logger);
			eliminar_msg_appeared_broker(msg_appeared);
		}break;
	case CAUGHT_BROKER:;
		t_msg_caught_broker *msg_caught = rcv_msj_caught_broker(cliente_fd, g_logger);
		pthread_mutex_unlock(&g_mutex_msjs);
		if(cache_espacio_suficiente(espacio_cache_msg_caught(msg_caught))) {
			pthread_mutex_lock(&g_mutex_msjs);
				enqueue_msg_caught(msg_caught, g_logger, cliente_fd);
			pthread_mutex_unlock(&g_mutex_msjs);
		} else {
			enviar_mensaje_cache_sin_espacio(cliente_fd, g_logger);
			free(msg_caught);
		}
		break;
	case LOCALIZED_BROKER:;
		t_msg_localized_broker *msg_localized = rcv_msj_localized_broker(cliente_fd, g_logger);
		pthread_mutex_unlock(&g_mutex_msjs);
		if(cache_espacio_suficiente(espacio_cache_msg_localized(msg_localized))) {
			pthread_mutex_lock(&g_mutex_msjs);
				enqueue_msg_localized(msg_localized, g_logger, cliente_fd);
			pthread_mutex_unlock(&g_mutex_msjs);
		} else {
			enviar_mensaje_cache_sin_espacio(cliente_fd, g_logger);
			eliminar_msg_localized_broker(msg_localized);
		}
		break;
	case FIN_SUSCRIPCION:;
		t_handsake_suscript *handshake = handshake = rcv_msj_handshake_suscriptor(cliente_fd);
		pthread_mutex_unlock(&g_mutex_msjs);
		log_info(g_logger,"(RECEIVING: END_SUSCRIPTION|%s|ID_SUSCRIPTOR = %d|Socket# = %d)"
				,nombre_cola(handshake->cola_id),handshake->id_suscriptor, cliente_fd);
		deshabilitar_suscriptor_cola(handshake);
		pthread_mutex_lock(&g_mutex_msjs);
			enviar_msg_confirmed(cliente_fd, g_logger);
		pthread_mutex_unlock(&g_mutex_msjs);
		free(handshake);

	}
	close(cliente_fd);
	free(socket);
}

void atender_suscripcion(op_code cod_op, t_socket_cliente_broker *socket)
{
	int id_suscriptor;
	int ult_id_recibido;
	int cliente_fd = socket->cliente_fd;
	int cant_msg = socket->cant_msg_enviados;
	ssize_t sent_bytes;
	bool suscr_down = false;
	switch (cod_op) {
	case SUSCRIBER_ACK:;
		t_handsake_suscript *handshake = rcv_msj_handshake_suscriptor(cliente_fd);
		pthread_mutex_unlock(&g_mutex_msjs);
		id_suscriptor = handshake->id_suscriptor;
		ult_id_recibido = handshake->id_recibido;
		t_tipo_mensaje id_cola = handshake->cola_id;
		cant_msg = handshake->msjs_recibidos;
		t_broker_queue *cola_suscript = cola_broker_suscripcion(id_cola);
		int sem_val;
		t_suscriptor_broker *suscriptor;
		if (cant_msg == 0) {
			log_info(g_logger,"(ATTENDING_SUSCRIBER|SOCKET:%d|%s|ID_SUSCRIPTOR:%d)",
				cliente_fd,nombre_cola(handshake->cola_id),handshake->id_suscriptor);
			alta_suscriptor_cola(cola_suscript,handshake);
		}
		free(handshake);
		suscriptor = obtengo_suscriptor_cola(cola_suscript, id_suscriptor);
		while(suscriptor->enabled) {
			sent_bytes = despachar_mensaje_a_suscriptor(socket, id_cola, ult_id_recibido, suscriptor, g_logger);
			if (sent_bytes == -1) {
				log_error(g_logger, "(SUSCRIBER_IS_DOWN|%s|ID_SUSCRIPTOR:%d)", nombre_cola(id_cola), id_suscriptor);
				suscriptor->enabled = false;
				suscr_down = true;
			} else {
			atender_cliente_broker(socket);
			free(socket);
			}
		}
		if (!suscr_down) {
			socket->cant_msg_enviados = cant_msg;
			enviar_msj_suscript_end(socket, g_logger, id_suscriptor);
			free(socket);
		}
		eliminar_suscriptor_por_id(cola_suscript, id_suscriptor);
		pthread_exit(EXIT_SUCCESS);
	}
}

void iniciar_estructuras_broker(void)
{
	t_algoritmo_memoria algoritmo = g_config_broker->algoritmo_memoria;
	log_debug(g_logdebug,"(Tamaño_Cache:%d Bytes)", g_config_broker->tamano_memoria);
	log_debug(g_logdebug,"(Tamaño_Mínimo_Particiones:%d Bytes)", g_config_broker->tamano_minimo_particion);
	log_debug(g_logdebug,"(Algoritmo_Memoria:%s)", config_get_string_value(g_config, "ALGORITMO_MEMORIA"));
	log_debug(g_logdebug,"(Algoritmo_Particion_Libre:%s)",config_get_string_value(g_config,"ALGORITMO_PARTICION_LIBRE"));
	log_debug(g_logdebug,"(Algoritmo_Reemplazo:%s)",config_get_string_value(g_config,"ALGORITMO_REEMPLAZO"));
	log_debug(g_logdebug,"(Frecuencia_Compactacion:%d)", g_config_broker->frecuencia_compactacion);
	g_msg_counter = 1;
	pthread_mutex_init(&g_mutex_msjs, NULL);
	pthread_mutex_init(&g_mutex_cache_part, NULL);
	pthread_mutex_init(&g_mutex_cache_buddy, NULL);
	pthread_mutex_init(&g_mutex_queue_new, NULL);
	pthread_mutex_init(&g_mutex_queue_get, NULL);
	pthread_mutex_init(&g_mutex_queue_localized, NULL);
	pthread_mutex_init(&g_mutex_queue_catch, NULL);
	pthread_mutex_init(&g_mutex_queue_caught, NULL);
	pthread_mutex_init(&g_mutex_queue_appeared, NULL);
	inicializar_colas_broker();
	g_team_suscriptos = list_create();
	g_gamecards_suscriptos = list_create();
	switch(algoritmo) {
	case PARTICIONES:;
		inicializar_cache_particiones_dinamicas();
		break;
	case BS:;
		inicializar_cache_buddy_system();
		break;
	}
}

void inicializar_cache_particiones_dinamicas(void)
{
	int tamano_cache = g_config_broker->tamano_memoria;
	g_cache_part = malloc(sizeof(t_cache_particiones));
	g_cache_part->tipo_cache = PARTICIONES;
	g_cache_part->cnt_id_partition = 1;
	g_cache_part->cnt_order_fifo = 1;
	g_cache_part->cnt_order_lru = (int) timestamp();
	g_cache_part->dir_base_part = 0;
	g_cache_part->min_size_part = g_config_broker->tamano_minimo_particion;
	g_cache_part->used_space = 0;
	g_cache_part->total_space = g_config_broker->tamano_memoria;
	g_cache_part->partition_table = list_create();
	g_cache_part->partition_repo = malloc(tamano_cache);
	memset(g_cache_part->partition_repo, 0, tamano_cache);
	generar_particion_dinamica_libre(0, tamano_cache);
}

void inicializar_cache_buddy_system(void)
{
	int tamano_cache = g_config_broker->tamano_memoria;
	g_cache_buddy = malloc(sizeof(t_cache_buddy));
	g_cache_buddy->tipo_cache = BS;
	g_cache_buddy->min_size_part = g_config_broker->tamano_minimo_particion;
	g_cache_buddy->total_space = tamano_cache;
	g_cache_buddy->used_space = 0;
	g_cache_buddy->cnt_order_fifo = 1;
	g_cache_buddy->cnt_order_lru = (int) timestamp();
	g_cache_buddy->buddy_table = list_create();
	g_cache_buddy->posiciones_buddy = list_create();
	g_cache_buddy->buddy_repo = malloc(tamano_cache);
	memset(g_cache_buddy->buddy_repo, 0, tamano_cache);
	int cant_bitarrays = g_config_broker->bit_arrays_bs;
	for (int i = 0; i < cant_bitarrays; i ++ ) {
		double tam_minimo = (double) g_cache_buddy->min_size_part;
		int indice = (int) log2(tam_minimo) + i;
		int tamano_buddy = (int) (pow (2,indice));
		int cant_buddies = (int) (tamano_cache / tamano_buddy);
		if (cant_buddies > 0) {
			char *data;
			t_posicion_buddy *posicion = malloc(sizeof(t_posicion_buddy));
			posicion->orden = i;
			posicion->buddy_size = tamano_buddy;
			posicion->cant_buddys = cant_buddies;
			posicion->free_buddys = cant_buddies;
			if (((int)cant_buddies/8) < 1) {  // -->> No puedo crear un bit_array con menos de 8 bits <<--
				data = malloc(8);
				memset(data,0,8);
				posicion->bitmap_buddy = bitarray_create_with_mode(data, 1, MSB_FIRST);
			} else {
				data = malloc(cant_buddies);
				memset(data,0,cant_buddies);
				posicion->bitmap_buddy = bitarray_create_with_mode(data, ((int)cant_buddies/8), MSB_FIRST);
			}
			void *position = (t_posicion_buddy*) posicion;
			list_add(g_cache_buddy->posiciones_buddy, position);
			log_debug(g_logdebug,"(Orden:%d|Tamaño_Particiones:%d Bytes|Cantidad_Máxima:%d)",posicion->orden, tamano_buddy,cant_buddies);
	}	}
	g_cache_buddy->cant_bitarrays = g_cache_buddy->posiciones_buddy->elements_count;
	g_cache_buddy->max_size_msg = ((t_posicion_buddy*)list_get(g_cache_buddy->posiciones_buddy, (g_cache_buddy->cant_bitarrays - 1)))->buddy_size;
	log_debug(g_logdebug,"(Tamaño_Máximo_Mensajes:%d Bytes)", g_cache_buddy->max_size_msg);
}

void inicializar_colas_broker(void)
{
	g_queue_get_pokemon = malloc(sizeof(t_broker_queue));
	setear_id_cola_broker(g_queue_get_pokemon, GET_POKEMON);
	g_queue_get_pokemon->mensajes_cola = list_create();
	g_queue_get_pokemon->suscriptores =  list_create();
	g_queue_new_pokemon = malloc(sizeof(t_broker_queue));
	setear_id_cola_broker(g_queue_new_pokemon, NEW_POKEMON);
	g_queue_new_pokemon->mensajes_cola = list_create();
	g_queue_new_pokemon->suscriptores =  list_create();
	g_queue_catch_pokemon = malloc(sizeof(t_broker_queue));
	setear_id_cola_broker(g_queue_catch_pokemon, CATCH_POKEMON);
	g_queue_catch_pokemon->mensajes_cola = list_create();
	g_queue_catch_pokemon->suscriptores =  list_create();
	g_queue_appeared_pokemon = malloc(sizeof(t_broker_queue));
	setear_id_cola_broker(g_queue_appeared_pokemon, APPEARED_POKEMON);
	g_queue_appeared_pokemon->mensajes_cola = list_create();
	g_queue_appeared_pokemon->suscriptores =  list_create();
	g_queue_caught_pokemon = malloc(sizeof(t_broker_queue));
	setear_id_cola_broker(g_queue_caught_pokemon, CAUGHT_POKEMON);
	g_queue_caught_pokemon->mensajes_cola = list_create();
	g_queue_caught_pokemon->suscriptores =  list_create();
	g_queue_localized_pokemon = malloc(sizeof(t_broker_queue));
	setear_id_cola_broker(g_queue_localized_pokemon, LOCALIZED_POKEMON);
	g_queue_localized_pokemon->mensajes_cola = list_create();
	g_queue_localized_pokemon->suscriptores =  list_create();
}

void setear_id_cola_broker(t_broker_queue *cola, t_tipo_mensaje tipo_cola)
{
   memcpy(&(cola->id_queue), &tipo_cola, sizeof(t_tipo_mensaje));
}

void alta_suscriptor_cola(t_broker_queue *cola_broker, t_handsake_suscript *handshake)
{
	int id_suscriptor = handshake->id_suscriptor;
	t_suscriptor_broker *suscriptor = malloc(sizeof(t_suscriptor_broker));
	int cant_msjs_sem = cant_msjs_sin_enviar(cola_broker, id_suscriptor);
	//Si la cola queda en 0 mensajes sin enviar le dejo enviar 1 mensaje cola vacía.
	suscriptor->id_suscriptor = id_suscriptor;
	sem_init(&suscriptor->sem_cont_msjs,0,cant_msjs_sem);
	suscriptor->cant_msg = handshake->msjs_recibidos;
	suscriptor->enabled = true;
	void *suscriber = (t_suscriptor_broker*) suscriptor;
   	list_add(cola_broker->suscriptores,suscriber);
   	agregar_id_suscriptor_lista_suscriptores(cola_broker->id_queue, suscriptor->id_suscriptor);
}

void agregar_id_suscriptor_lista_suscriptores(t_tipo_mensaje id_cola, int id_suscriptor)
{
	t_list *lista_suscriptores = select_lista_suscriptores(id_cola);
	if (id_suscriptor_no_ingresado(lista_suscriptores, &id_suscriptor)) {
		void *id = malloc(sizeof(int));
		memcpy(id,&id_suscriptor,sizeof(int));
		list_add(lista_suscriptores,id);
	}
}

t_list *select_lista_suscriptores(t_tipo_mensaje id_cola)
{
	t_list *lista_suscriptores;
	if (es_cola_suscripcion_team(id_cola)) {
		lista_suscriptores = g_team_suscriptos;
	}
	else if (es_cola_suscripcion_gamecard(id_cola)) {
		lista_suscriptores = g_gamecards_suscriptos;
	} else {
		puts("Error ID_COLA");
	}
	return lista_suscriptores;
}

void set_mensaje_enviado_a_suscriptor_cola(t_broker_queue *cola_broker, t_suscriptor_broker *suscriptor)
{
	if (cola_broker->mensajes_cola->elements_count != 0 ) {
		for (int i = 0; i < cola_broker->mensajes_cola->elements_count; i ++) {
			void *suscript_id = malloc(sizeof(uint32_t));
			memcpy(suscript_id,&suscriptor->id_suscriptor, sizeof(uint32_t));
			t_queue_msg *queue_msg = list_get(cola_broker->mensajes_cola,i);
			list_add(queue_msg->sended_suscriptors, suscript_id);
		}
	}
}

void deshabilitar_suscriptor_cola(t_handsake_suscript *handshake)
{
	int id_suscriptor = handshake->id_suscriptor;
	t_broker_queue *cola_broker = cola_broker_suscripcion(handshake->cola_id);
	pthread_mutex_t *sem_cola = semaforo_cola(handshake->cola_id);
	pthread_mutex_lock(sem_cola);
		t_suscriptor_broker *suscriptor = obtengo_suscriptor_cola(cola_broker , id_suscriptor);
		suscriptor->enabled = false;
		int valor_sem;
		sem_getvalue(&suscriptor->sem_cont_msjs, &valor_sem);
		if (valor_sem == 0) {
			sem_post(&suscriptor->sem_cont_msjs);
		}
	pthread_mutex_unlock(sem_cola);
}

bool es_suscriptor_habilitado(t_broker_queue *cola_broker, pthread_mutex_t semaforo_cola, int id_suscriptor)
{
	pthread_mutex_lock(&semaforo_cola);
	t_suscriptor_broker *suscriptor = obtengo_suscriptor_cola(cola_broker, id_suscriptor);
	pthread_mutex_unlock(&semaforo_cola);
	return suscriptor->enabled;
}

bool msj_enviado_a_todo_suscriptor(t_queue_msg *mensaje_cola)
{
	bool resultado;
	int cant_receptores = mensaje_cola->sended_suscriptors->elements_count;
	t_list *suscriptores = select_lista_suscriptores(mensaje_cola->tipo_mensaje);
	if (cant_receptores > 0 && suscriptores->elements_count == cant_receptores) {
		int coincidencias = 0;
		for (int i = 0; i < suscriptores->elements_count; i ++) {
			int *elem = list_get(mensaje_cola->sended_suscriptors,i);
			if (! id_suscriptor_no_ingresado(suscriptores,elem)){
				coincidencias ++;
			}
		}
		resultado = cant_receptores == coincidencias;
	}
	else {
		resultado = false;
	}
	return resultado;
}

bool id_suscriptor_no_ingresado(t_list *lista_suscriptores, int *id_suscriptor)
{
	bool mismo_id(void *id)
		{
			int  *suscript_id = (int*) id;
			bool condition = *id_suscriptor == *suscript_id;
			return condition;
		}
	bool es_id_ingresado = list_any_satisfy(lista_suscriptores, mismo_id);
	return !es_id_ingresado;
}

t_suscriptor_broker *obtengo_suscriptor_cola(t_broker_queue *cola_broker, int id_suscriptor)
{
	bool mismo_id_suscriptor(void *suscriptor)
	{
		t_suscriptor_broker* t_suscript = (t_suscriptor_broker*) suscriptor;
		bool condition = id_suscriptor == t_suscript->id_suscriptor;
		return condition;
	}
	t_suscriptor_broker *suscriptor_buscado;
	suscriptor_buscado = list_find(cola_broker->suscriptores, mismo_id_suscriptor);
	if (suscriptor_buscado != NULL) {
		return suscriptor_buscado;
	} else {
		log_error(g_logdebug,"(ERROR|SUSCRIPTOR NO_EXISTE)");
		return NULL;
	}
}

void eliminar_suscriptor_por_id(t_broker_queue *cola_broker, int id_suscriptor)
{
	t_suscriptor_broker *suscriptor = obtengo_suscriptor_cola(cola_broker, id_suscriptor);
	quitar_suscriptor_cola(cola_broker , suscriptor);
}

void quitar_suscriptor_cola(t_broker_queue *cola_broker, t_suscriptor_broker *suscriptor)
{
	t_suscriptor_broker *suscrip_t = (t_suscriptor_broker*) suscriptor;
	bool compare_suscript(void *otro_suscriptor){
		t_suscriptor_broker *otro_suscrip_t = (t_suscriptor_broker*) otro_suscriptor;
		bool condition = otro_suscrip_t->id_suscriptor == suscrip_t->id_suscriptor;
		return condition;
	}
	t_suscriptor_broker *suscript = list_remove_by_condition(cola_broker->suscriptores, compare_suscript);
	sem_destroy(&suscriptor->sem_cont_msjs);
	free(suscriptor);
}

int eliminar_mensajes_cola(t_tipo_mensaje id_cola, int cant_mensajes, t_log *logger)
{
	t_broker_queue *cola_broker = cola_broker_suscripcion(id_cola);
	int cant_a_borrar = 0;
	if (cola_broker->mensajes_cola->elements_count > 0) {
		t_list *msjs_enviados_a_todos = list_filter(cola_broker->mensajes_cola, (void*) msj_enviado_a_todo_suscriptor);
		if (msjs_enviados_a_todos->elements_count < cant_mensajes) {
			cant_a_borrar = msjs_enviados_a_todos->elements_count;
		} else {
			cant_a_borrar = cant_mensajes;
		}
		for (int i = 0; i < cant_a_borrar; i ++) {
			t_queue_msg* mensaje = list_get(msjs_enviados_a_todos, i);
			log_info(logger,"(REMOVING_MSG_FROM:%s|ID_MSG:%d)", nombre_cola(id_cola), mensaje->id_mensaje);
			eliminar_mensaje_por_id(cola_broker, mensaje->id_mensaje, logger);

		}
		list_destroy(msjs_enviados_a_todos);
	}
	return cant_a_borrar;
}

void eliminar_mensaje_por_id(t_broker_queue *cola_broker, int id_mensaje, t_log *logger)
{
	bool mismo_id_mensaje(void *mensaje)
	{
		t_queue_msg *queue_msg = (t_queue_msg*) mensaje;
		bool condition = es_msj_con_mismo_id(queue_msg, id_mensaje);
		return condition;
	}
	eliminar_msg_data_particion_cache(id_mensaje, logger);
	list_remove_and_destroy_by_condition(cola_broker->mensajes_cola, mismo_id_mensaje, (void*) destruir_mensaje_cola);
}

void destruir_mensaje_cola(t_queue_msg *queue_msg)
{
	eliminar_msg_data(queue_msg->msg_data);
	list_destroy_and_destroy_elements(queue_msg->sended_suscriptors, (void*) free);
	free(queue_msg);
}

bool es_cola_suscripcion_team(t_tipo_mensaje cola_suscripcion)
{
	return (cola_suscripcion == APPEARED_POKEMON || cola_suscripcion == CAUGHT_POKEMON || cola_suscripcion == LOCALIZED_POKEMON);
}

bool es_cola_suscripcion_gamecard(t_tipo_mensaje cola_suscripcion)
{
	return (cola_suscripcion == NEW_POKEMON || cola_suscripcion == CATCH_POKEMON || cola_suscripcion == GET_POKEMON);
}

bool codigo_operacion_valido_broker(op_code cod_oper)
{
	return (cod_oper == NEW_BROKER || cod_oper == GET_BROKER || cod_oper == CATCH_BROKER
			|| cod_oper == APPEARED_BROKER || cod_oper == CAUGHT_BROKER || cod_oper == LOCALIZED_BROKER
			|| cod_oper == SUSCRIBER_ACK  || cod_oper == FIN_SUSCRIPCION );
}

int compactar_particiones(t_cache_particiones *cache_part, t_log *logger)
{
	reorganizar_tabla_particiones(cache_part, logger);
	int cant_part = cache_part->partition_table->elements_count;
	int bytes_recup = 0;
	if (cant_part > 0) {
		int size1 = ultimo_bloque_cache(cache_part);
		void *auxiliar;
		t_particion_dinamica* part0 = list_get(cache_part->partition_table,0);
		int desplazamiento = part0->dir_base;
		if (desplazamiento != 0) {   // Si La 1° particion ocupada no está al comienzo del cache
			int size_part_id1 = tamano_particion_dinamica(part0);
			auxiliar = malloc(size_part_id1);
			memcpy(auxiliar, cache_part->partition_repo + desplazamiento, size_part_id1);
			memset(cache_part->partition_repo, 0, desplazamiento);
			memcpy(cache_part->partition_repo, auxiliar, size_part_id1);
			part0->dir_base = 0;
			part0->dir_heap = part0->dir_base + size_part_id1 - 1;
			// log_trace(logger, "(%s|Comprimiendo_Partición:%d|%d Bytes)", nombre_cache(cache_part->tipo_cache), part0->id_particion, desplazamiento);
			free(auxiliar);
		}
		for (int i = 0; i < (cant_part - 1); i ++) {  //Solo entra si hay más de 2 particiones ocupadas
			int dir_heap1 = ((t_particion_dinamica*)list_get(cache_part->partition_table, i))->dir_heap;
			int dir_base2_new = dir_heap1 + 1;
			int dir_base2 = ((t_particion_dinamica*)list_get(cache_part->partition_table, i + 1))->dir_base;
			int dir_heap2 = ((t_particion_dinamica*)list_get(cache_part->partition_table,i + 1))->dir_heap;
			int id_part2 = ((t_particion_dinamica*)list_get(cache_part->partition_table,i + 1))->id_particion;
			int gap = dir_base2 - dir_base2_new;
			if (gap > 0 )  {
				// log_trace(logger, "(%s|Comprimiendo_Partición:%d|%d Bytes)", nombre_cache(cache_part->tipo_cache), id_part2, gap);
				t_particion_dinamica* particion = list_get(cache_part->partition_table,i + 1);
				int tamano_particion = tamano_particion_dinamica(particion);
				uint32_t data_size = particion->data_size;
				auxiliar = malloc(data_size);
				int clean_bytes = gap + tamano_particion;
				memcpy(auxiliar, (cache_part->partition_repo + dir_base2), data_size);
				memset((cache_part->partition_repo + dir_base2_new),0,clean_bytes);
				memcpy((cache_part->partition_repo + dir_base2_new), auxiliar, data_size);
				particion->dir_base = dir_base2_new;
				particion->dir_heap = (particion->dir_base + tamano_particion) - 1;
				free(auxiliar);
			}
		}
		int size2 = ultimo_bloque_cache(cache_part);
		bytes_recup = size2 - size1;
		cache_part->dir_base_part = dir_base_ultimo_bloque_part_din(cache_part);
		cache_part->used_space = cache_part->total_space - size2;
		log_trace(logger,"\n(COMPACTANDO %s:Cantidad_Particiones_Ocupadas:%d|%d Bytes_Recuperados|Espacio_Disponible:%d bytes)\n",
				nombre_cache(cache_part->tipo_cache), cant_part, bytes_recup, ultimo_bloque_cache(cache_part));
		generar_particion_dinamica_libre(cache_part->dir_base_part, size2);

	} else {
		cache_part->dir_base_part = 0;
		cache_part->used_space = 0;
		bytes_recup = ultimo_bloque_cache(cache_part);
		generar_particion_dinamica_libre(cache_part->dir_base_part, bytes_recup);
		log_trace(logger,"(%s VACIO:%d Bytes_Recuperados)", nombre_cache(cache_part->tipo_cache), bytes_recup);
	}

	return  dir_base_ultimo_bloque_part_din(cache_part);
}

t_list *ordenar_particiones_libres_first_fit(void)
{
	t_list *particiones = obtengo_particiones_dinamicas_libres();
	list_sort(particiones, ordenar_por_direccion_base);
	return particiones;
}

t_list *ordenar_particiones_libres_best_fit(void)
{
	t_list *particiones = obtengo_particiones_dinamicas_libres();
	list_sort(particiones, ordenar_por_tamano);
	return particiones;
}

t_list *obtengo_particiones_dinamicas_libres(void) {
	t_list *particiones_libres = list_filter(g_cache_part->partition_table, es_particion_libre);
	list_sort(particiones_libres, ordenar_por_direccion_base);
	return particiones_libres;
}

void reorganizar_tabla_particiones(t_cache_particiones *cache_part, t_log *logger)
{
	int cant_total = cache_part->partition_table->elements_count;
	if (cant_total > 0) {
		t_list *particiones_ocupadas = list_filter(cache_part->partition_table, es_particion_ocupada);
		int a_borrar = cant_total - particiones_ocupadas->elements_count;
		log_info(logger, "\n(Reorganizando_Tabla_Particiones %s|Total:%d|Libres:%d|Ocupadas:%d)\n",
			nombre_cache(cache_part->tipo_cache), cant_total, a_borrar, particiones_ocupadas->elements_count);
		t_list *auxiliar = list_create();
		int cant_elem = particiones_ocupadas->elements_count;
		cache_part->cnt_id_partition = cant_elem + 1;
		for (int i = 0; i < cant_elem; i ++ ) {
			t_particion_dinamica *part = (t_particion_dinamica*) list_get(particiones_ocupadas,i);
			t_particion_dinamica *new_part = duplico_particion_dinamica(part);
			new_part->id_particion = i + 1;
			list_add(auxiliar,new_part);
		}
		list_destroy(particiones_ocupadas);
		list_clean_and_destroy_elements(cache_part->partition_table,free);
		list_add_all(cache_part->partition_table, auxiliar);
		list_destroy(auxiliar);
	}
}

bool ordenar_por_direccion_base(void *part1, void *part2)
{
	return ((t_particion_dinamica*) part1)->dir_base < ((t_particion_dinamica*) part2)->dir_base;
}

bool ordenador_fifo(void *part1, void *part2)
{
	return ((t_particion_dinamica*) part1)->orden_fifo < ((t_particion_dinamica*) part2)->orden_fifo;
}

bool ordenador_lru(void *part1, void *part2)
{
	return ((t_particion_dinamica*) part1)->last_used < ((t_particion_dinamica*) part2)->last_used;
}

bool ordenar_por_tamano(void *part1, void *part2)
{
	t_particion_dinamica *particion1 = (t_particion_dinamica*) part1;
	t_particion_dinamica *particion2 = (t_particion_dinamica*) part2;
	return tamano_particion_dinamica(particion1) < tamano_particion_dinamica(particion2);
}

bool es_particion_libre(void *part) {
	t_particion_dinamica *particion = (t_particion_dinamica*) part;
	return particion->presencia == false;
}

bool es_particion_ocupada(void *part) {
	t_particion_dinamica *particion = (t_particion_dinamica*) part;
	return particion->presencia == true;
}

bool espacio_suficiente_particion_cache(t_cache_particiones *cache, int data_size)
{
	bool resultado;
	bool tamano_suficiente(void *part) {
		t_particion_dinamica *particion = (t_particion_dinamica*) part;
		return tamano_particion_dinamica(particion) >= data_size;
	}
	t_list *particiones_libres = list_filter(cache->partition_table, es_particion_libre);
	resultado = list_any_satisfy(particiones_libres, tamano_suficiente);
	list_destroy(particiones_libres);
	return resultado;
}

int dir_heap_part_dinamica(t_queue_msg *msg_queue)
{
	int data_size = msg_queue->msg_data->size;
	int heap = 0;
	if (data_size > g_cache_part->min_size_part) {
		heap = data_size - 1;
	} else {
		heap = (g_cache_part->min_size_part) - 1;
	}
	return heap;
}

int tamano_particion_dinamica(t_particion_dinamica *particion)
{
	return particion->dir_heap - particion->dir_base + 1;
}

t_particion_dinamica *duplico_particion_dinamica(t_particion_dinamica *part)
{
	t_particion_dinamica *new_part = malloc(sizeof(t_particion_dinamica));
	new_part->id_particion = 0;
	new_part->orden_fifo = part->orden_fifo;
	new_part->last_used = part->last_used;
	new_part->dir_base = part->dir_base;
	new_part->dir_heap = part->dir_heap;
	new_part->data_size = part->data_size;
	new_part->id_cola = part->id_cola;
	new_part->id_mensaje = part->id_mensaje;
	new_part->presencia = part->presencia;
	return new_part;
}

void generar_particion_dinamica_libre(int dir_base, int size)
{
	t_particion_dinamica *bloque_libre = malloc(sizeof(t_particion_dinamica));
	bloque_libre->dir_base = dir_base;
	bloque_libre->dir_heap = bloque_libre->dir_base + size - 1;
	bloque_libre->data_size = 0;
	bloque_libre->id_cola = 0;
	bloque_libre->id_mensaje = 0;
	bloque_libre->presencia = false;
	bloque_libre->id_particion = 1;
	bloque_libre->orden_fifo = g_cache_part->cnt_order_fifo;
	bloque_libre->last_used = g_cache_part->cnt_order_lru;
	g_cache_part->cnt_order_fifo += 1;
	g_cache_part->cnt_order_lru += 1;
	void *free_block = (t_particion_dinamica*) bloque_libre;
	list_add(g_cache_part->partition_table, free_block);
	if (g_cache_part->partition_table->elements_count > 1) {
		list_sort(g_cache_part->partition_table, ordenar_por_direccion_base);
	}
	if (bloque_libre->dir_base != 0 && (bloque_libre->dir_heap + 1) > g_cache_part->dir_base_part) {
		g_cache_part->dir_base_part = bloque_libre->dir_heap + 1;
	}
	int cant_part = g_cache_part->partition_table->elements_count;
	for (int i = 0; i < cant_part; i ++) {
		t_particion_dinamica *part = list_get(g_cache_part->partition_table, i);
		if (part->id_particion != i + 1) {
			part->id_particion = i + 1;
		}
}	}

t_particion_dinamica *ultima_particion_libre(void)
{
	t_particion_dinamica *last_part = NULL;
	int ultimo_elem = g_cache_part->partition_table->elements_count - 1;
	bool ocupada = ((t_particion_dinamica*) list_get(g_cache_part->partition_table, ultimo_elem))->presencia;
	if (!ocupada) {
		last_part = list_get(g_cache_part->partition_table, ultimo_elem);
	}
	return last_part;
}

int dir_base_ultimo_bloque_part_din(t_cache_particiones *cache_part)
{
	int ultima_dir_base;
	if (cache_part->partition_table->elements_count > 0) {
		int last_index = cache_part->partition_table->elements_count - 1;
		int dir_heap_ult_part = ((t_particion_dinamica*) list_get(cache_part->partition_table, last_index))->dir_heap;
		ultima_dir_base = dir_heap_ult_part + 1;
	} else {
		ultima_dir_base = 0;
	}
	return ultima_dir_base;
}

int ultimo_bloque_cache(t_cache_particiones *cache_part)
{
	return cache_part->total_space - dir_base_ultimo_bloque_part_din(cache_part);
}

bool sin_espacio_ult_bloque_cache(uint32_t data_size, t_cache_particiones *cache)
{
	bool resultado;
	if (data_size > cache->min_size_part) {
		resultado = data_size > ultimo_bloque_cache(cache);
	} else {
		resultado = cache->min_size_part > ultimo_bloque_cache(cache);
	}
	return resultado;
}

bool cache_espacio_suficiente(uint32_t data_size)
{
	bool resultado;
	int espacio_cache = 0, tamano_minimo = 0, tamano_msj = 0;
	t_algoritmo_memoria algoritmo = g_config_broker->algoritmo_memoria;
	switch (algoritmo) {
	case PARTICIONES:;
		espacio_cache = g_cache_part->total_space;
		tamano_minimo = g_cache_part->min_size_part;
		break;
	case BS:;
		espacio_cache = g_cache_buddy->max_size_msg;
		tamano_minimo = g_cache_buddy->min_size_part;
		break;
	}
	if ( data_size > tamano_minimo) {
		tamano_msj = data_size;
	} else {
		tamano_msj = tamano_minimo;
	}
	resultado = espacio_cache >= tamano_msj;
	return resultado;
}

void enviar_mensaje_cache_sin_espacio(int cliente_fd, t_log *logger)
{
	enviar_mensaje_error(cliente_fd, logger, " !!TAMAÑO_DEL_MENSAJE_EXCEDE_EL_TAMAÑO_DEL_CACHE -- REDUZCA_SU_MENSAJE!!");
}

int espacio_cache_msg_get(t_msg_get_broker *msg_get)
{
	return sizeof(uint32_t) + msg_get->size_pokemon - 1;
}

int espacio_cache_msg_new(t_msg_new_broker *msg_new)
{
	return (4 * sizeof(uint32_t)) + msg_new->size_pokemon - 1;
}

int espacio_cache_msg_catch(t_msg_catch_broker *msg_catch)
{
	return (3 * sizeof(uint32_t)) + msg_catch->size_pokemon - 1;
}

int espacio_cache_msg_appeared(t_msg_appeared_broker *msg_appeared)
{
	return (3 * sizeof(uint32_t)) + msg_appeared->size_pokemon - 1;
}

int espacio_cache_msg_caught(t_msg_caught_broker *msg_caught)
{
	return sizeof(t_result_caught);
}

int espacio_cache_msg_localized(t_msg_localized_broker *msg_localized)
{
	return (2 + (2 * msg_localized->posiciones->cant_posic)) * sizeof(uint32_t) + msg_localized->size_pokemon - 1;
}

void set_bits_bitmaps_buddy_system(int tamano_buddy, int bit_index)
{
	int orden_buddy = 0;
	if (es_tamano_buddy_primer_orden(tamano_buddy)) {
		set_bits_bitmaps_superiores(orden_buddy, bit_index);
	} else if (es_tamano_buddy_ultimo_orden(tamano_buddy)) {
		orden_buddy = g_cache_buddy->cant_bitarrays - 1;
		set_bits_bitmaps_inferiores(orden_buddy , bit_index);
	} else {
		orden_buddy = obtengo_orden_buddy(tamano_buddy);
		set_bits_bitmaps_superiores(orden_buddy, bit_index);
		set_bits_bitmaps_inferiores(orden_buddy, bit_index);
	}
}

void set_bits_bitmaps_superiores(int orden_buddy, int bit_index)
{
	int orden_inicial = orden_buddy + 1, index_bitmap = bit_index;
	t_posicion_buddy *posicion = NULL;
	for (int i = orden_inicial; i < g_cache_buddy->cant_bitarrays; i ++) {
		posicion = list_get(g_cache_buddy->posiciones_buddy, i);
		index_bitmap = (int) index_bitmap / 2;
		if (! bitarray_test_bit(posicion->bitmap_buddy, index_bitmap)) {
			bitarray_set_bit(posicion->bitmap_buddy, index_bitmap);
			posicion->free_buddys --;
		}
		if ((index_bitmap != 0) && ((int)index_bitmap / 2) == 0) {
			i = g_cache_buddy->cant_bitarrays;
		}
}	}

void set_bits_bitmaps_inferiores(int orden_buddy, int bit_index)
{
	int orden_inicial = orden_buddy - 1, index_inicial = (int) bit_index * 2, iterador = 2, index_final = 0;
	for (int i = orden_inicial; i > -1; i --) {
		index_final =  index_inicial + iterador;
		t_posicion_buddy *posicion = list_get(g_cache_buddy->posiciones_buddy, i);
		for (int j = index_inicial; j < index_final; j ++) {
			if (! bitarray_test_bit(posicion->bitmap_buddy, j)) {
				bitarray_set_bit(posicion->bitmap_buddy, j);
				posicion->free_buddys --;
			}
		}
		index_inicial *= 2;
		iterador *= 2;
}	}

void clean_bits_bitmaps_buddy_system(int tamano_buddy, int bit_index)
{
	int orden_buddy = 0;
	if (es_tamano_buddy_primer_orden(tamano_buddy) && !(tiene_su_propio_buddy_ocupado(bit_index, tamano_buddy))) {
		clean_bits_bitmaps_superiores(orden_buddy, bit_index);
	} else if (es_tamano_buddy_ultimo_orden(tamano_buddy)) {
		orden_buddy = g_cache_buddy->cant_bitarrays - 1;
		clean_bits_bitmaps_inferiores(orden_buddy , bit_index);
	} else {
		orden_buddy = obtengo_orden_buddy(tamano_buddy);
		if (!(tiene_su_propio_buddy_ocupado(bit_index, tamano_buddy))) {
			clean_bits_bitmaps_superiores(orden_buddy, bit_index);
		}
		clean_bits_bitmaps_inferiores(orden_buddy, bit_index);
	}
}

void clean_bits_bitmaps_superiores(int orden_buddy, int bit_index)
{
	int orden_inicial = orden_buddy + 1, index_bitmap = bit_index;
	t_posicion_buddy *posicion = NULL;
	for (int i = orden_inicial; i < g_cache_buddy->cant_bitarrays; i ++) {
		posicion = list_get(g_cache_buddy->posiciones_buddy, i);
		index_bitmap = (int) index_bitmap / 2;
		if (bitarray_test_bit(posicion->bitmap_buddy, index_bitmap)) {
			bitarray_clean_bit(posicion->bitmap_buddy, index_bitmap);
			posicion->free_buddys ++;
		}
		if (es_nro_par(index_bitmap) && bitarray_test_bit(posicion->bitmap_buddy, (index_bitmap + 1))) {
			i = g_cache_buddy->cant_bitarrays;
		} else if(!(es_nro_par(index_bitmap)) && bitarray_test_bit(posicion->bitmap_buddy, (index_bitmap - 1))) {
			i = g_cache_buddy->cant_bitarrays;
		}
	}
}

void clean_bits_bitmaps_inferiores(int orden_buddy, int bit_index)
{
	int orden_inicial = orden_buddy - 1, index_inicial = (int) bit_index * 2, iterador = 2, index_final = 0;
	for (int i = orden_inicial; i > -1; i --) {
		index_final =  index_inicial + iterador;
		t_posicion_buddy *posicion = list_get(g_cache_buddy->posiciones_buddy, i);
		for (int j = index_inicial; j < index_final; j ++) {
			if (bitarray_test_bit(posicion->bitmap_buddy, j)) {
				bitarray_clean_bit(posicion->bitmap_buddy, j);
				posicion->free_buddys ++;
			}
		}
		index_inicial *= 2;
		iterador *= 2;
}	}

bool tiene_su_propio_buddy_ocupado(int index_bitmap, int tamano_buddy)
{
	bool mismo_tamano(void *posic) {
			t_posicion_buddy *posicion = (t_posicion_buddy*) posic;
			return posicion->buddy_size == tamano_buddy;
	}
	t_posicion_buddy *posicion = (t_posicion_buddy*) list_find(g_cache_buddy->posiciones_buddy, mismo_tamano);
	bool resultado = 0;
	if (posicion->cant_buddys == 1) {
		resultado = false;
	} else if (index_bitmap == 0) {
		resultado = bitarray_test_bit(posicion->bitmap_buddy, 1);
	} else if(index_bitmap == (posicion->cant_buddys - 1)) {
		resultado = bitarray_test_bit(posicion->bitmap_buddy, (posicion->cant_buddys - 2));
	} else if(es_nro_par(index_bitmap)) {
		resultado = bitarray_test_bit(posicion->bitmap_buddy, index_bitmap + 1);
	} else {
		resultado = bitarray_test_bit(posicion->bitmap_buddy, index_bitmap - 1);
	}
	return resultado;
}

bool all_bits_bitmap_clean(t_posicion_buddy *posicion)
{
	bool resultado = false;
	for (int i = 0; i < posicion->cant_buddys; i ++) {
		resultado = resultado || bitarray_test_bit(posicion->bitmap_buddy, i);
	}
	return !(resultado);
}

t_particion_buddy *buddy_proxima_victima(int index)
{
	t_particion_buddy *buddy = NULL;
	t_list *sorted_table;
	t_algoritmo_reemplazo algoritmo = g_config_broker->algoritmo_reemplazo;
	switch(algoritmo) {
	case FIFO:;
		sorted_table = list_sorted(g_cache_buddy->buddy_table, ordenador_fifo_buddy);
		break;
	case LRU:;
		sorted_table = list_sorted(g_cache_buddy->buddy_table, ordenador_lru_buddy);
		break;
	}
	buddy = (t_particion_buddy*) list_get(sorted_table, index);
	list_destroy(sorted_table);
	return buddy;
}

bool ordenador_fifo_buddy(void *part1, void *part2)
{
	return ((t_particion_buddy*) part1)->orden_fifo < ((t_particion_buddy*) part2)->orden_fifo;
}

bool ordenador_lru_buddy(void *part1, void *part2)
{
	return ((t_particion_buddy*) part1)->last_used < ((t_particion_buddy*) part2)->last_used;
}

bool ordenador_buddy_posiciones(void *part1, void *part2)
{
	return ((t_particion_buddy*) part1)->posicion < ((t_particion_buddy*) part2)->posicion;
}

void leer_config_broker(char *path) {
	g_config = config_create(path);
	g_config_broker = malloc(sizeof(t_config_broker));
	g_config_broker->ip_broker = config_get_string_value(g_config,"IP_BROKER");
	g_config_broker->puerto_broker = config_get_string_value(g_config,"PUERTO_BROKER");
	g_config_broker->tamano_memoria = config_get_int_value(g_config,"TAMANO_MEMORIA");
	g_config_broker->tamano_minimo_particion = config_get_int_value(g_config,"TAMANO_MINIMO_PARTICION");
	g_config_broker->algoritmo_memoria = algoritmo_memoria(config_get_string_value(g_config,"ALGORITMO_MEMORIA"));
	g_config_broker->algoritmo_particion_libre = algoritmo_part_libre(config_get_string_value(g_config,"ALGORITMO_PARTICION_LIBRE"));
	g_config_broker->algoritmo_reemplazo = algoritmo_reemplazo(config_get_string_value(g_config,"ALGORITMO_REEMPLAZO"));
	g_config_broker->frecuencia_compactacion = config_get_int_value(g_config,"FRECUENCIA_COMPACTACION");
	g_config_broker->show_logs_on_screen = verdadero_falso(config_get_string_value(g_config,"SHOW_LOGS_ON_SCREEN"));
	g_config_broker->ruta_log = config_get_string_value(g_config, "LOG_FILE");
	g_config_broker->show_bitmaps_bs = verdadero_falso(config_get_string_value(g_config,"SHOW_BITMAPS_BS"));
	g_config_broker->bit_arrays_bs = config_get_int_value(g_config,"MAX_BITARRAYS_BS");
	g_config_broker->dump_file = config_get_string_value(g_config,"DUMP_FILE");
}

t_algoritmo_memoria algoritmo_memoria(char *valor)
{
	t_algoritmo_memoria algoritmo;
	if(strcmp(valor,"PARTICIONES") == 0) {
		algoritmo = PARTICIONES;
	}
	else if (strcmp(valor,"BS") == 0) {
		algoritmo = BS;
	}
	else {
		algoritmo = 0;
	}
	return algoritmo;
}

char *nombre_cache(t_algoritmo_memoria algoritmo)
{
	char *nombre;
	switch(algoritmo) {
	case PARTICIONES:;
		nombre = "PARTICIONES";
		break;
	case BS:;
		nombre = "BUDDY_SYSTEM";
		break;
	case SWAP:;
		nombre = "SWAP";
		break;
	}
	return nombre;
}

t_algoritmo_part_libre algoritmo_part_libre(char *valor)
{
	t_algoritmo_part_libre algoritmo;
	if(strcmp(valor,"FF") == 0) {
		algoritmo = FF;
	}
	else if (strcmp(valor,"BF") == 0) {
		algoritmo = BF;
	}
	else {
		algoritmo = 0;
	}
	return algoritmo;
}

t_algoritmo_reemplazo algoritmo_reemplazo(char *valor)
{
	t_algoritmo_reemplazo algoritmo;
	if(strcmp(valor,"FIFO") == 0) {
		algoritmo = FIFO;
	}
	else if (strcmp(valor,"LRU") == 0) {
		algoritmo = LRU;
	}
	else {
		algoritmo = 0;
	}
	return algoritmo;
}

bool es_tamano_buddy_ultimo_orden(int tamano_buddy)
{
	bool mismo_tamano(void *posic) {
		t_posicion_buddy *posicion = (t_posicion_buddy*) posic;
		return posicion->buddy_size == tamano_buddy;
	}
	int ultimo_orden = g_cache_buddy->posiciones_buddy->elements_count - 1;
	int orden_pedido = ((t_posicion_buddy*) list_find(g_cache_buddy->posiciones_buddy,mismo_tamano))->orden;
	return orden_pedido == ultimo_orden;
}

bool es_tamano_buddy_primer_orden(int tamano_buddy)
{
	bool mismo_tamano(void *posic) {
		t_posicion_buddy *posicion = (t_posicion_buddy*) posic;
		return posicion->buddy_size == tamano_buddy;
	}
	int orden_pedido = ((t_posicion_buddy*) list_find(g_cache_buddy->posiciones_buddy,mismo_tamano))->orden;
	return orden_pedido == 0;
}

int obtengo_orden_buddy(int tamano_buddy)
{
	bool mismo_tamano(void *posic) {
		t_posicion_buddy *posicion = (t_posicion_buddy*) posic;
		return posicion->buddy_size == tamano_buddy;
	}
	int orden = ((t_posicion_buddy*) list_find(g_cache_buddy->posiciones_buddy,mismo_tamano))->orden;
	return orden;
}

void print_bitmaps_buddy_system_status(void)
{
	int cant_bitmaps = g_cache_buddy->cant_bitarrays - 1;
	for(int i = cant_bitmaps; i > -1; i --) {
		t_posicion_buddy *posicion = (t_posicion_buddy*) list_get(g_cache_buddy->posiciones_buddy, i);
		char *bitword = malloc(1 + (posicion->cant_buddys * 2));
		strcpy(bitword, "(");
		for(int j = 0; j < posicion->cant_buddys; j ++) {
			int bit = bitarray_test_bit(posicion->bitmap_buddy, j);
			char *cero_uno = string_itoa(bit);
			char auxiliar[1];
			strcpy(auxiliar,cero_uno);
			strcat(bitword,auxiliar);
			if (j == (posicion->cant_buddys - 1)) {
				strcat(bitword,")");
			}
			else {
				strcat(bitword,"|");
			}
			free(cero_uno);
		}
		puts(bitword);
		free(bitword);
}	}

bool es_nro_par(int numero)
{
	int resto =(int) (numero % 2);
	return resto == 0;
}

void manejo_senial_externa(void)
{
	int broker_pid = process_get_thread_id();
	log_info(g_logger,"Enviar Señales SIGUSR1/2 al proceso (PID):%d", broker_pid);
	signal(SIGUSR1, funcion_captura_senial);
	signal(SIGUSR2, funcion_captura_senial);
	signal(SIGILL, funcion_captura_senial);
	signal(SIGHUP, funcion_captura_senial);
	signal(SIGTRAP, funcion_captura_senial);
}

void funcion_captura_senial(int senial)
{
	int broker_pid = process_get_thread_id();
	log_warning(g_logger,"Señal recibida: %s -->>(kill -%d  %d).",senial_recibida(senial), senial, broker_pid);
	switch(senial) {
	case SIGUSR1:;
		dump_print_cache_particiones(senial);
		break;
	case SIGUSR2:;
		dump_print_cache_particiones(senial);
		break;
	default:
		puts("-->>No es posible manejar señal enviada.<<--");
		break;
	}
}

char *senial_recibida(int senial)
{
	char *recvd_signal;
	switch(senial) {
	case SIGUSR1:;
		recvd_signal = "SIGUSR1";
		break;
	case SIGUSR2:;
		recvd_signal = "SIGUSR2";
		break;
	case SIGILL:;
		recvd_signal = "SIGILL";
		break;
	case SIGHUP:;
		recvd_signal = "SIGHUP";
		break;
	case SIGTRAP:;
		recvd_signal = "SIGTRAP";
		break;
	}
	return recvd_signal;
}

uint64_t timestamp(void)
{
    struct timeval valor;
    gettimeofday(&valor, NULL);
    unsigned long long result = (((unsigned long long )valor.tv_sec) * 1000 + ((unsigned long) valor.tv_usec));
    uint64_t tiempo = result;
    return tiempo;
}

char *fecha_hora_actual(void)
{
	char *fecha_hora = malloc(20);
	time_t ahora = time(NULL);
	strftime(fecha_hora, 20, "%d/%m/%Y %H:%M:%S", localtime(&ahora));
	return fecha_hora;
}

void dump_print_cache_particiones(int senial)
{
	FILE* dump_file = fopen(g_config_broker->dump_file, "a");
	t_algoritmo_memoria algoritmo = g_config_broker->algoritmo_memoria;
	char *fecha_hora = fecha_hora_actual();
	int cant_particiones = 0;
	if (senial == SIGUSR1) {
		fprintf(dump_file, "<<----------------------------------------------------------------------------------------------------------------------------------------------------------------------->>\n");
		fprintf(dump_file, "Dump:%s -- Algoritmo Memoria: %s\n", fecha_hora, nombre_cache(algoritmo));
	} else {
		printf("Dump:%s -- Algoritmo Memoria: %s\n", fecha_hora, nombre_cache(algoritmo));
	}
	free(fecha_hora);
	switch(algoritmo) {
	case PARTICIONES:;
		pthread_mutex_lock(&g_mutex_cache_part);
		cant_particiones = g_cache_part->partition_table->elements_count;
		for (int i = 0; i < cant_particiones; i ++) {
			t_particion_dinamica *particion = (t_particion_dinamica*) list_get(g_cache_part->partition_table, i);
			if (senial == SIGUSR1) {
				file_dump_particion_dinamica(particion, dump_file);
			} else {
				imprimir_particion_dinamica(particion);
			}
		}
		pthread_mutex_unlock(&g_mutex_cache_part);
		break;
	case BS:;
		pthread_mutex_lock(&g_mutex_cache_buddy);
		dump_print_particiones_buddy(senial, dump_file);
		pthread_mutex_unlock(&g_mutex_cache_buddy);
		break;
	}
	fclose(dump_file);
}

void dump_print_particiones_buddy(int senial, FILE *dump_file)
{
	t_particion_buddy *particion = NULL, *particion2 = NULL;
	int cant_particiones = 0, i = 0, orden_buddys = 1, proxima_posicion = 0, limite = 0, tamano =0;
	cant_particiones = g_cache_buddy->buddy_table->elements_count;
	if (cant_particiones > 0) {
		t_list *buddys = list_sorted(g_cache_buddy->buddy_table,ordenador_buddy_posiciones);
		do {
			particion = (t_particion_buddy*) list_get(buddys, i);
			if (senial == SIGUSR1) {
				file_dump_particion_buddy(particion, orden_buddys, dump_file);
			} else {
				imprimir_particion_buddy(particion, orden_buddys);
			}
			proxima_posicion = particion->posicion + particion->tamano;
			if (i < cant_particiones - 1) {
				particion2 = (t_particion_buddy*) list_get(buddys, i + 1);
				if (proxima_posicion != particion2->posicion) {
					orden_buddys ++;
					limite = particion2->posicion - 1;
					tamano = limite - proxima_posicion + 1;
					if (senial == SIGUSR1) {
						fprintf(dump_file, "Partición	%d:	%p	-	%p.	[L]	Size:	%db\n",orden_buddys, g_cache_buddy->buddy_repo + proxima_posicion,
							g_cache_buddy->buddy_repo + limite, tamano);
					} else {
						printf("Partición	%d:	%p	-	%p.	[L]	Size:	%db\n",orden_buddys, g_cache_buddy->buddy_repo + proxima_posicion,
							g_cache_buddy->buddy_repo + limite, tamano);
				}	}
			} else {
				if (proxima_posicion < g_cache_buddy->total_space) {
					limite = g_cache_buddy->total_space - 1;
					tamano = g_cache_buddy->total_space - proxima_posicion;
					if (senial == SIGUSR1) {
						fprintf(dump_file, "Partición	%d:	%p	-	%p.	[L]	Size:	%db\n",orden_buddys + 1, g_cache_buddy->buddy_repo + proxima_posicion,
							g_cache_buddy->buddy_repo + limite, tamano);
					} else {
						printf("Partición	%d:	%p	-	%p.	[L]	Size:	%db\n",orden_buddys + 1, g_cache_buddy->buddy_repo + proxima_posicion,
							g_cache_buddy->buddy_repo + limite, tamano);
			}	}	}
			i ++;
			orden_buddys ++;

		} while (i < cant_particiones);
			list_destroy(buddys);
	} else {
		limite = g_cache_buddy->total_space - 1;
		tamano = g_cache_buddy->total_space;
		if (senial == SIGUSR1) {
			fprintf(dump_file, "Partición	%d:	%p	-	%p.	[L]	Size:	%db\n",orden_buddys, g_cache_buddy->buddy_repo + proxima_posicion,
				g_cache_buddy->buddy_repo + limite, tamano);
		} else {
			printf("Partición	%d:	%p	-	%p.	[L]	Size:	%db\n",orden_buddys, g_cache_buddy->buddy_repo + proxima_posicion,
				g_cache_buddy->buddy_repo + limite, tamano);
	}	}
}

void file_dump_particion_dinamica(t_particion_dinamica *particion, FILE *dump_file)
{
	if (particion->presencia) {
		fprintf(dump_file,"Partición	%d:	%p	-	%p.	[X]	Size:	%db	LRU:<%d>	Cola:<%s>	ID:<%d>\n",
		particion->id_particion, g_cache_part->partition_repo + (particion->dir_base), g_cache_part->partition_repo + (particion->dir_heap),
		 tamano_particion_dinamica(particion), particion->last_used, nombre_cola(particion->id_cola), particion->id_mensaje);
	} else {
		fprintf(dump_file, "Partición	%d:	%p	-	%p.	[L]	Size:	%db\n",
		particion->id_particion, g_cache_part->partition_repo + (particion->dir_base), g_cache_part->partition_repo + (particion->dir_heap),
		tamano_particion_dinamica(particion));
	}
}

void file_dump_particion_buddy(t_particion_buddy *particion, int orden, FILE *dump_file)
{
	fprintf(dump_file, "Partición	%d:	%p	-	%p.	[X]	Size:	%db	LRU:<%d>	Cola:<%s>	ID:<%d>\n",
		orden, g_cache_buddy->buddy_repo + (particion->posicion), g_cache_buddy->buddy_repo + (particion->posicion + (particion->tamano - 1)),
		 particion->tamano, particion->last_used, nombre_cola(particion->id_cola), particion->id_mensaje);
}

void imprimir_particion_dinamica(t_particion_dinamica *particion)
{
	if (particion->presencia) {
		printf("Partición	%d:	%p	-	%p.	[X]	Size:	%db	LRU:<%d>	Cola:<%s>	ID:<%d>\n",
		particion->id_particion, g_cache_part->partition_repo + (particion->dir_base), g_cache_part->partition_repo + (particion->dir_heap),
		 tamano_particion_dinamica(particion), particion->last_used, nombre_cola(particion->id_cola), particion->id_mensaje);
	} else {
		printf("Partición	%d:	%p	-	%p.	[L]	Size:	%db\n",
		particion->id_particion, g_cache_part->partition_repo + (particion->dir_base), g_cache_part->partition_repo + (particion->dir_heap),
		tamano_particion_dinamica(particion));
	}
}

void imprimir_particion_buddy(t_particion_buddy *particion, int orden)
{
	printf("Partición	%d:	%p	-	%p.	[X]	Size:	%db  LRU:<%d>	Cola:<%s>	ID:<%d>\n",
		orden, g_cache_buddy->buddy_repo + (particion->posicion), g_cache_buddy->buddy_repo + (particion->posicion + (particion->tamano - 1)),
		 particion->tamano, particion->last_used, nombre_cola(particion->id_cola), particion->id_mensaje);
}

void iniciar_log_broker(void) {
	bool log_habilitado = false;
	if (g_config_broker->show_logs_on_screen) {
		log_habilitado = true;
	}
	g_logger = log_create(g_config_broker->ruta_log, "BROKER", log_habilitado, LOG_LEVEL_TRACE);
	g_logdebug = log_create(RUTA_LOG_DEBUG, "BROKER_DBG", log_habilitado, LOG_LEVEL_TRACE);
}
