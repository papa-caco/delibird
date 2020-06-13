/*
 ============================================================================
 Name        : broker_oper.c

 Author      : Los Que Aprueban.

 Created on	 : 16 may. 2020

 Version     :

 Description :
 ============================================================================
 */
#include "broker_oper.h"

void enqueue_msg_get(t_msg_get_broker *msg_get_broker, t_log *logger, int cliente_fd)
{
	t_queue_msg *msg_queue_get = generar_nuevo_mensaje_cola();
	msg_queue_get->tipo_mensaje = GET_POKEMON;
	msg_queue_get->msg_data = serializar_msg_get(msg_get_broker);
	sem_wait(&g_mutex_queue_get);
		msg_queue_get->dir_base_part_inicial = poner_datos_msj_en_particion_cache(msg_queue_get, logger);
		msg_queue_get->msg_data->data = realloc(msg_queue_get->msg_data->data, 1);
		memset(msg_queue_get->msg_data->data, 0, 1);
		poner_msj_en_cola(msg_queue_get, g_queue_get_pokemon);
	sem_post(&g_mutex_queue_get);
	enviar_id_mensaje(cliente_fd, logger, msg_queue_get->id_mensaje);
	eliminar_msg_get_broker(msg_get_broker);
	incremento_cnt_id_mensajes();
}

void enqueue_msg_new(t_msg_new_broker *msg_new_broker, t_log *logger, int cliente_fd)
{
	t_queue_msg *msg_queue_new = generar_nuevo_mensaje_cola();
	msg_queue_new->tipo_mensaje = NEW_POKEMON;
	msg_queue_new->msg_data = serializar_msg_new(msg_new_broker);
	sem_wait(&g_mutex_queue_new);
		msg_queue_new->dir_base_part_inicial = poner_datos_msj_en_particion_cache(msg_queue_new, logger);
		msg_queue_new->msg_data->data = realloc(msg_queue_new->msg_data->data, 1);
		memset(msg_queue_new->msg_data->data, 0, 1);
		poner_msj_en_cola(msg_queue_new, g_queue_new_pokemon);
	sem_post(&g_mutex_queue_new);
	enviar_id_mensaje(cliente_fd, logger, msg_queue_new->id_mensaje);
	eliminar_msg_new_broker(msg_new_broker);
	incremento_cnt_id_mensajes();
}

void enqueue_msg_catch(t_msg_catch_broker *msg_catch_broker, t_log *logger, int cliente_fd)
{
	t_queue_msg *msg_queue_catch = generar_nuevo_mensaje_cola();
	msg_queue_catch->tipo_mensaje = CATCH_POKEMON;
	msg_queue_catch->msg_data = serializar_msg_catch(msg_catch_broker);
	sem_wait(&g_mutex_queue_catch);
		msg_queue_catch->dir_base_part_inicial = poner_datos_msj_en_particion_cache(msg_queue_catch, logger);
		msg_queue_catch->msg_data->data = realloc(msg_queue_catch->msg_data->data, 1);
		memset(msg_queue_catch->msg_data->data, 0, 1);
		poner_msj_en_cola(msg_queue_catch, g_queue_catch_pokemon);
	sem_post(&g_mutex_queue_catch);
	enviar_id_mensaje(cliente_fd, logger, msg_queue_catch->id_mensaje);
	eliminar_msg_catch_broker(msg_catch_broker);
	incremento_cnt_id_mensajes();
}

void enqueue_msg_appeared(t_msg_appeared_broker *msg_appeared_broker, t_log *logger, int cliente_fd)
{
	t_queue_msg *msg_queue_appeared = generar_nuevo_mensaje_cola();
	msg_queue_appeared->id_correlativo = msg_appeared_broker->id_correlativo;
	msg_queue_appeared->tipo_mensaje = APPEARED_POKEMON;
	msg_queue_appeared->msg_data = serializar_msg_appeared(msg_appeared_broker);
	sem_wait(&g_mutex_queue_appeared);
		msg_queue_appeared->dir_base_part_inicial = poner_datos_msj_en_particion_cache(msg_queue_appeared, logger);
		msg_queue_appeared->msg_data->data = realloc(msg_queue_appeared->msg_data->data, 1);
		memset(msg_queue_appeared->msg_data->data, 0, 1);
		poner_msj_en_cola(msg_queue_appeared, g_queue_appeared_pokemon);
	sem_post(&g_mutex_queue_appeared);
	enviar_id_mensaje(cliente_fd, logger, msg_queue_appeared->id_mensaje);
	eliminar_msg_appeared_broker(msg_appeared_broker);
	incremento_cnt_id_mensajes();
}

void enqueue_msg_caught(t_msg_caught_broker *msg_caught_broker, t_log *logger, int cliente_fd)
{
	t_queue_msg *msg_queue_caught = generar_nuevo_mensaje_cola();
	msg_queue_caught->tipo_mensaje = CAUGHT_POKEMON;
	msg_queue_caught->id_correlativo = msg_caught_broker->id_correlativo;
	msg_queue_caught->msg_data = serializar_msg_caught(msg_caught_broker);
	sem_wait(&g_mutex_queue_caught);
		msg_queue_caught->dir_base_part_inicial = poner_datos_msj_en_particion_cache(msg_queue_caught, logger);
		msg_queue_caught->msg_data->data = realloc(msg_queue_caught->msg_data->data, 1);
		memset(msg_queue_caught->msg_data->data, 0, 1);
		poner_msj_en_cola(msg_queue_caught, g_queue_caught_pokemon);
	sem_post(&g_mutex_queue_caught);
	enviar_id_mensaje(cliente_fd, logger, msg_queue_caught->id_mensaje);
	free(msg_caught_broker);
	incremento_cnt_id_mensajes();
}

void enqueue_msg_localized(t_msg_localized_broker *msg_localized_broker, t_log *logger, int cliente_fd)
{
	t_queue_msg *msg_queue_localized = generar_nuevo_mensaje_cola();
	msg_queue_localized->id_correlativo = msg_localized_broker->id_correlativo;
	msg_queue_localized->tipo_mensaje = LOCALIZED_POKEMON;
	msg_queue_localized->msg_data = serializar_msg_localized(msg_localized_broker);
	sem_wait(&g_mutex_queue_localized);
		msg_queue_localized->dir_base_part_inicial = poner_datos_msj_en_particion_cache(msg_queue_localized, logger);
		msg_queue_localized->msg_data->data = realloc(msg_queue_localized->msg_data->data, 1);
		memset(msg_queue_localized->msg_data->data, 0, 1);
		poner_msj_en_cola(msg_queue_localized, g_queue_localized_pokemon);
	sem_post(&g_mutex_queue_localized);
	enviar_id_mensaje(cliente_fd, logger, msg_queue_localized->id_mensaje);
	eliminar_msg_localized_broker(msg_localized_broker);
	incremento_cnt_id_mensajes();
}

t_queue_msg *generar_nuevo_mensaje_cola(void)
{
	t_queue_msg *queue_msg = malloc(sizeof(t_queue_msg));
	queue_msg->id_mensaje = g_msg_counter;
	queue_msg->id_correlativo = 0;
	queue_msg->dir_base_part_inicial = 0;
	queue_msg->msg_data = NULL;
	queue_msg->recibido_por_todos = false;
	queue_msg->sended_suscriptors = list_create();
	return queue_msg;
}

int poner_datos_msj_en_particion_cache(t_queue_msg *msg, t_log *logger)
{
	t_algoritmo_memoria algoritmo = g_config_broker->algoritmo_memoria;
	int dir_base_particion;
	switch (algoritmo) {
	case PARTICIONES:;
		t_particion_dinamica * particion = buscar_particion_dinamica_libre(msg);
		if (particion == NULL && sin_espacio_ult_bloque_cache_part_din(msg)) {
			g_cache_part->dir_base_part = compactar_particiones_dinamicas(logger);
			particion = generar_particion_dinamica(msg);
		}
		else if (particion == NULL && cache_espacio_suficiente(msg->msg_data->size)) {
			particion = generar_particion_dinamica(msg);
		}
		int offset =  particion->dir_base;
		memset((g_cache_part->partition_repo) + offset, 0, tamano_particion_dinamica(particion));
		memcpy((g_cache_part->partition_repo) + offset, msg->msg_data->data, particion->data_size);
		char *name_cola = nombre_cola(msg->tipo_mensaje);
		dir_base_particion = particion->dir_base;
		int espacio = g_cache_part->total_space - g_cache_part->used_space;
		log_trace(logger,"(DATA_STORED|ID_PART:%d|START_ADDR:%d|END_ADDR:%d|DATA_SIZE:%d Bytes|QUEUE:%s|ID_MSG:%d|CACHE_SPACE:%d Bytes)",
			particion->id_particion,particion->dir_base, particion->dir_heap, particion->data_size,
			name_cola, particion->id_mensaje,  espacio);
		break;
	}
	return dir_base_particion;
}

t_particion_dinamica *generar_particion_dinamica(t_queue_msg *msg_queue)
{
	t_particion_dinamica *particion;
	if (ultimo_bloque_part_din() >= msg_queue->msg_data->size) {
		particion = malloc(sizeof(t_particion_dinamica));
		particion->id_particion = g_cache_part->id_partition;
		particion->dir_base = g_cache_part->dir_base_part;
		particion->dir_heap = particion->dir_base + dir_heap_part_dinamica(msg_queue);
		particion->data_size = msg_queue->msg_data->size;
		particion->id_mensaje = msg_queue->id_mensaje;
		particion->id_cola = msg_queue->tipo_mensaje;
		particion->presencia = true;
		void *part = (t_particion_dinamica*) particion;
		list_add(g_cache_part->partition_table, part);
		g_cache_part->id_partition += 1;
		g_cache_part->dir_base_part += tamano_particion_dinamica(particion);
		g_cache_part->used_space +=  tamano_particion_dinamica(particion);
	} else {
		printf("Hay espacio en cache pero No hay particion con espacio para este mensaje -- Se debe compactar!\n");
		particion = NULL;
	}
	return particion;
}

t_particion_dinamica *buscar_particion_dinamica_libre(t_queue_msg *msg_queue)
{
	t_particion_dinamica *particion;
	t_list *particiones;
	t_algoritmo_part_libre algoritmo = g_config_broker->algoritmo_particion_libre;
	bool tamano_suficiente(void *part) {
		t_particion_dinamica *particion = (t_particion_dinamica*) part;
		return tamano_particion_dinamica(particion) >= msg_queue->msg_data->size;
	}
	switch (algoritmo) {
	case (FF):;
		particiones = ordenar_particiones_libres_first_fit();
		particion = list_find(particiones, (void*) tamano_suficiente);
		list_destroy(particiones);
		break;
	case (BF):;
		particiones = ordenar_particiones_libres_best_fit();
		particion = list_find(particiones, (void*) tamano_suficiente);
		list_destroy(particiones);
		break;
	}
	if (particion != NULL) {
		particion->id_mensaje = msg_queue->id_mensaje;
		particion->id_cola = msg_queue->tipo_mensaje;
		particion->data_size = msg_queue->msg_data->size;
		particion->presencia = true;
		g_cache_part->used_space += tamano_particion_dinamica(particion);
	}
	return particion;
}

void obtener_datos_msj_de_particion_cache(t_queue_msg *msg_queue, t_log *logger)
{
	t_algoritmo_memoria algoritmo = g_config_broker->algoritmo_memoria;
	switch (algoritmo) {
	case PARTICIONES:;
		t_particion_dinamica *particion = get_particion_cache_por_id_mensaje(msg_queue->id_mensaje);
		if (particion == NULL) {
			//TODO la Particion está en SWAP y hay que traerla.
		} else if (particion->data_size != msg_queue->msg_data->size) {
				log_error(logger,"(INVALID_CACHE_PARTITION)");
				pthread_exit(NULL);
		} else {
			int offset = particion->dir_base;
			//TODO Comparar particion->dir_base con msg_queue->dir_base_part_inicial. Si no coincide hubo compactación.
			msg_queue->msg_data->data = realloc(msg_queue->msg_data->data, msg_queue->msg_data->size);
			memcpy(msg_queue->msg_data->data, g_cache_part->partition_repo + offset, particion->data_size);
		}
		break;
	}
}

void eliminar_msg_data_particion_cache(int id_mensaje, t_log *logger)
{
	t_algoritmo_memoria algoritmo = g_config_broker->algoritmo_memoria;
	switch (algoritmo) {
	case PARTICIONES:;
		t_particion_dinamica *particion = get_particion_cache_por_id_mensaje(id_mensaje);
		particion->id_mensaje = 0;
		particion->id_cola = 0;
		particion->data_size = 0;
		particion->presencia = false;
		int offset = particion->dir_base;
		memset((g_cache_part->partition_repo) + offset, 0, tamano_particion_dinamica(particion));
		g_cache_part->used_space =  g_cache_part->used_space - tamano_particion_dinamica(particion);
		int espacio = g_cache_part->total_space - g_cache_part->used_space;
		log_debug(logger,"(REMOVED_DATA ID_PART:%d|START_ADDR:%d|END_ADDR:%d|PARTITION_SIZE:%d Bytes|CACHE_SPACE:%d Bytes)",
					particion->id_particion,particion->dir_base, particion->dir_heap, tamano_particion_dinamica(particion), espacio);
		break;
	}
}

t_particion_dinamica *get_particion_cache_por_id_mensaje(int id_mensaje)
{
	bool mismo_id_msj(void *elem)
	{
		t_particion_dinamica *part = (t_particion_dinamica*) elem;
		bool condition = part->id_mensaje == id_mensaje;
		return condition;
	}
	t_particion_dinamica *particion = list_find(g_cache_part->partition_table, mismo_id_msj);
	return particion;
}


void poner_msj_en_cola(t_queue_msg *mensaje_cola, t_broker_queue *cola_boker)
{
	void *msg_queue = (t_queue_msg*) mensaje_cola;
	int n_suscriptores = cola_boker->suscriptores->elements_count;
	list_add(cola_boker->mensajes_cola,msg_queue);
	if (n_suscriptores > 0) {
		list_iterate(cola_boker->suscriptores,(void*)incremento_sem_cont_suscriptor);
	}
}

void incremento_sem_cont_suscriptor(t_suscriptor_broker *suscriptor)
{
	sem_post(&suscriptor->sem_cont_msjs);
}

ssize_t despachar_mensaje_a_suscriptor(t_socket_cliente_broker *socket, t_tipo_mensaje id_cola, int ult_id_recibido, t_suscriptor_broker * suscriptor, t_log *logger)
{
	ssize_t sent_bytes;
	switch(id_cola) {
		case APPEARED_POKEMON:;
			sent_bytes = despachar_msjs_appeared(socket, ult_id_recibido, suscriptor, logger);
			break;
		case GET_POKEMON:;
			sent_bytes = despachar_msjs_get(socket, ult_id_recibido, suscriptor, logger);
			break;
		case NEW_POKEMON:;
			sent_bytes = despachar_msjs_new(socket, ult_id_recibido, suscriptor, logger);
			break;
		case CATCH_POKEMON:;
			sent_bytes = despachar_msjs_catch(socket, ult_id_recibido, suscriptor, logger);
			break;
		case LOCALIZED_POKEMON:;
			sent_bytes = despachar_msjs_localized(socket, ult_id_recibido, suscriptor, logger);
			break;
		case CAUGHT_POKEMON:;
			sent_bytes = despachar_msjs_caught(socket, ult_id_recibido, suscriptor, logger);
			break;
	}
	return sent_bytes;
}

ssize_t despachar_msjs_get(t_socket_cliente_broker *socket, int ult_id_recibido,t_suscriptor_broker *suscriptor,t_log *logger)
{
	t_queue_msg *msg_queue_get;
	ssize_t sent_bytes = 0;
	// Queda en espera si recibió todos los msjs hasta el próximo mensaje en cola o fin de suscripción
	set_msg_enviado_a_suscriptor(g_queue_get_pokemon, suscriptor->id_suscriptor, ult_id_recibido);
	sem_wait(&suscriptor->sem_cont_msjs);
	sem_wait(&g_mutex_msjs);
	msg_queue_get = get_msg_sin_enviar(g_queue_get_pokemon, suscriptor->id_suscriptor);
	if (msg_queue_get == NULL) {
		sent_bytes = enviar_msj_cola_vacia(socket, logger, suscriptor->id_suscriptor);
	} else {
		obtener_datos_msj_de_particion_cache(msg_queue_get, g_logger);
		t_msg_get_gamecard *msg_get_gamecard = deserializar_msg_get(msg_queue_get->msg_data);
		msg_get_gamecard->id_mensaje = msg_queue_get->id_mensaje;
		sent_bytes = enviar_msj_get_gamecard(socket, logger, msg_get_gamecard);
		eliminar_msg_get_gamecard(msg_get_gamecard);
		if (msg_queue_get->id_mensaje >= 20) {
			eliminar_mensajes_cola(msg_queue_get->tipo_mensaje, 20, g_logdebug);
		}
	}
	sem_post(&g_mutex_msjs);
	return sent_bytes;
}

ssize_t despachar_msjs_new(t_socket_cliente_broker *socket, int ult_id_recibido, t_suscriptor_broker *suscriptor, t_log * logger)
{
	t_queue_msg *msg_queue_new;
	ssize_t sent_bytes = 0;
	set_msg_enviado_a_suscriptor(g_queue_new_pokemon, suscriptor->id_suscriptor, ult_id_recibido);
	sem_wait(&suscriptor->sem_cont_msjs);
	sem_wait(&g_mutex_msjs);
	msg_queue_new =  get_msg_sin_enviar(g_queue_new_pokemon, suscriptor->id_suscriptor);
	if (msg_queue_new == NULL) {
		sent_bytes = enviar_msj_cola_vacia(socket, logger, suscriptor->id_suscriptor);
	} else {
		obtener_datos_msj_de_particion_cache(msg_queue_new, g_logger);
		t_msg_new_gamecard *msg_new_gamecard = deserializar_msg_new(msg_queue_new->msg_data);
		msg_new_gamecard->id_mensaje = msg_queue_new->id_mensaje;
		sent_bytes = enviar_msj_new_gamecard(socket, logger, msg_new_gamecard);
		eliminar_msg_new_gamecard(msg_new_gamecard);
	}
	sem_post(&g_mutex_msjs);
	return sent_bytes;
}

ssize_t despachar_msjs_catch(t_socket_cliente_broker *socket, int ult_id_recibido, t_suscriptor_broker *suscriptor, t_log * logger)
{
	t_queue_msg *msg_queue_catch;
	ssize_t sent_bytes = 0;
	set_msg_enviado_a_suscriptor(g_queue_catch_pokemon, suscriptor->id_suscriptor, ult_id_recibido);
	sem_wait(&suscriptor->sem_cont_msjs);
	sem_wait(&g_mutex_msjs);
	msg_queue_catch = get_msg_sin_enviar(g_queue_catch_pokemon, suscriptor->id_suscriptor);
	if (msg_queue_catch == NULL) {
		sent_bytes = enviar_msj_cola_vacia(socket, logger, suscriptor->id_suscriptor);
	} else {
		obtener_datos_msj_de_particion_cache(msg_queue_catch, g_logger);
		t_msg_catch_gamecard *msg_catch_gamecard = deserializar_msg_catch(msg_queue_catch->msg_data);
		msg_catch_gamecard->id_mensaje = msg_queue_catch->id_mensaje;
		sent_bytes = enviar_msj_catch_gamecard(socket, logger, msg_catch_gamecard);
		eliminar_msg_catch_gamecard(msg_catch_gamecard);
	}
	sem_post(&g_mutex_msjs);
	return sent_bytes;
}

ssize_t despachar_msjs_localized(t_socket_cliente_broker *socket, int ult_id_recibido, t_suscriptor_broker *suscriptor, t_log * logger)
{
	t_queue_msg *msg_queue_localized;
	ssize_t sent_bytes = 0;
	set_msg_enviado_a_suscriptor(g_queue_localized_pokemon, suscriptor->id_suscriptor, ult_id_recibido);
	sem_wait(&suscriptor->sem_cont_msjs);
	sem_wait(&g_mutex_msjs);
	msg_queue_localized = get_msg_sin_enviar(g_queue_localized_pokemon, suscriptor->id_suscriptor);
	if (msg_queue_localized == NULL) {
		sent_bytes = enviar_msj_cola_vacia(socket, logger, suscriptor->id_suscriptor);
	} else {
		obtener_datos_msj_de_particion_cache(msg_queue_localized, g_logger);
		t_msg_localized_team *msg_localized_team = deserializar_msg_localized(msg_queue_localized->msg_data);
		msg_localized_team->id_mensaje = msg_queue_localized->id_mensaje;
		msg_localized_team->id_correlativo = msg_queue_localized->id_correlativo;
		sent_bytes = enviar_msj_localized_team(socket, logger, msg_localized_team);
		eliminar_msg_localized_team(msg_localized_team);
	}
	sem_post(&g_mutex_msjs);
	return sent_bytes;
}

ssize_t despachar_msjs_appeared(t_socket_cliente_broker *socket, int ult_id_recibido, t_suscriptor_broker *suscriptor, t_log * logger)
{
	t_queue_msg *msg_queue_appeared;
	ssize_t sent_bytes = 0;
	set_msg_enviado_a_suscriptor(g_queue_appeared_pokemon, suscriptor->id_suscriptor, ult_id_recibido);
	sem_wait(&suscriptor->sem_cont_msjs);
	sem_wait(&g_mutex_msjs);
	msg_queue_appeared = get_msg_sin_enviar(g_queue_appeared_pokemon, suscriptor->id_suscriptor);
	if (msg_queue_appeared == NULL) {
		sent_bytes = enviar_msj_cola_vacia(socket, logger, suscriptor->id_suscriptor);
	} else {
		obtener_datos_msj_de_particion_cache(msg_queue_appeared, g_logger);
		t_msg_appeared_team *msg_appeared_team = deserializar_msg_appeared(msg_queue_appeared->msg_data);
		msg_appeared_team->id_mensaje = msg_queue_appeared->id_mensaje;
		msg_appeared_team->id_correlativo = msg_queue_appeared->id_correlativo;
		sent_bytes = enviar_msj_appeared_team(socket, logger, msg_appeared_team);
		eliminar_msg_appeared_team(msg_appeared_team);
	}
	sem_post(&g_mutex_msjs);
	return sent_bytes;
}

ssize_t despachar_msjs_caught(t_socket_cliente_broker *socket, int ult_id_recibido, t_suscriptor_broker *suscriptor, t_log * logger)
{
	t_queue_msg *msg_queue_caught;
	ssize_t sent_bytes = 0;
	set_msg_enviado_a_suscriptor(g_queue_caught_pokemon, suscriptor->id_suscriptor, ult_id_recibido);
	sem_wait(&suscriptor->sem_cont_msjs);
	sem_wait(&g_mutex_msjs);
	msg_queue_caught = get_msg_sin_enviar(g_queue_caught_pokemon, suscriptor->id_suscriptor);
	if (msg_queue_caught == NULL) {
		sent_bytes = enviar_msj_cola_vacia(socket, logger, suscriptor->id_suscriptor);
	}
	else {
		obtener_datos_msj_de_particion_cache(msg_queue_caught, g_logger);
		t_msg_caught_team *msg_caught_team = deserializar_msg_caught(msg_queue_caught->msg_data);
		msg_caught_team->id_mensaje = msg_queue_caught->id_mensaje;
		msg_caught_team->id_correlativo = msg_queue_caught->id_correlativo;
		sent_bytes = enviar_msj_caught_team(socket, logger, msg_caught_team);
		if (msg_queue_caught->id_mensaje >= 60) {
			eliminar_mensajes_cola(msg_queue_caught->tipo_mensaje, 60, g_logdebug);
		}
	}
	sem_post(&g_mutex_msjs);
	return sent_bytes;
}

void set_msg_enviado_a_suscriptor(t_broker_queue *cola_broker, int id_suscriptor, int id_mensaje)
{
	bool mismo_id_mensaje(void *mensaje)
	{
		t_queue_msg *queue_msg = (t_queue_msg*) mensaje;
		bool condition = es_msj_con_mismo_id(queue_msg, id_mensaje);
		return condition;
	}
	t_queue_msg *queue_msg = list_find(cola_broker->mensajes_cola, mismo_id_mensaje);
	if (queue_msg != NULL) {
		void *suscript_id = malloc(sizeof(int));
		memcpy(suscript_id,&id_suscriptor,sizeof(int));
		list_add(queue_msg->sended_suscriptors,suscript_id);
		if (msj_enviado_a_todo_suscriptor(queue_msg)) {
			queue_msg->recibido_por_todos = true;
		}
	}
}

t_queue_msg *get_msg_sin_enviar(t_broker_queue *cola_broker,int id_suscriptor)
{
	bool no_fue_enviado(void *mensaje)
	{
		t_queue_msg *queue_msg = (t_queue_msg*) mensaje;
		bool condition = es_msj_sin_enviar(queue_msg,id_suscriptor);
		return condition;
	}
	t_queue_msg *queue_msg = list_find(cola_broker->mensajes_cola, no_fue_enviado);
	return queue_msg;
}

int cant_msjs_sin_enviar(t_broker_queue *cola_broker, int id_suscriptor)
{
	bool no_fue_enviado(void *mensaje)
	{
		t_queue_msg *queue_msg = (t_queue_msg*) mensaje;
		bool condition = es_msj_sin_enviar(queue_msg,id_suscriptor);
		return condition;
	}
	t_list *msjs_sin_enviar = list_filter(cola_broker->mensajes_cola, no_fue_enviado);
	list_iterate(msjs_sin_enviar, (void*) set_sin_recibir_por_todos);
	int cant_msjs = msjs_sin_enviar->elements_count;
	list_destroy(msjs_sin_enviar);
	return cant_msjs;
}

void set_sin_recibir_por_todos(t_queue_msg *mensaje_cola)
{
	if (mensaje_cola->recibido_por_todos) {
		mensaje_cola->recibido_por_todos = false;
	}
}

bool es_msj_con_mismo_id(t_queue_msg *mensaje, int id_mensaje)
{
	return mensaje->id_mensaje == id_mensaje;
}


bool es_msj_sin_enviar(t_queue_msg *mensaje, int id_suscriptor)
{
	bool mismo_id(void *id)
	{
		int elem_id;
		memcpy(&elem_id,id, sizeof(int));
		bool condition = elem_id == id_suscriptor;
		return condition;
	}

	bool resultado;
	int cant_suscript = mensaje->sended_suscriptors->elements_count;
	if(cant_suscript == 0) {
		resultado = true;
	}
	else if(list_any_satisfy(mensaje->sended_suscriptors,mismo_id)) {
		resultado = false;
	} else{
		resultado = true;
	}
	return resultado;
}

t_broker_queue *cola_broker_suscripcion(t_tipo_mensaje tipo_mensaje)
{
	t_broker_queue *cola_broker = NULL;
	switch (tipo_mensaje) {
	case CATCH_POKEMON:;
		cola_broker = g_queue_catch_pokemon;
		break;
	case CAUGHT_POKEMON:;
		cola_broker = g_queue_caught_pokemon;
		break;
	case NEW_POKEMON:;
		cola_broker = g_queue_new_pokemon;
		break;
	case APPEARED_POKEMON:
		cola_broker = g_queue_appeared_pokemon;
		break;
	case GET_POKEMON:;
		cola_broker = g_queue_get_pokemon;
		break;
	case LOCALIZED_POKEMON:;
		cola_broker = g_queue_localized_pokemon;
		break;
	}
	return cola_broker;
}

sem_t semaforo_cola(t_tipo_mensaje tipo_mensaje)
{
	sem_t semaforo;
	switch (tipo_mensaje) {
	case CATCH_POKEMON:;
		semaforo = g_mutex_queue_catch;
		break;
	case CAUGHT_POKEMON:;
		semaforo = g_mutex_queue_caught;
		break;
	case NEW_POKEMON:;
		semaforo = g_mutex_queue_new;
		break;
	case APPEARED_POKEMON:
		semaforo = g_mutex_queue_appeared;
		break;
	case GET_POKEMON:;
		semaforo = g_mutex_queue_get;
		break;
	case LOCALIZED_POKEMON:;
		semaforo = g_mutex_queue_localized;
		break;
	}
	return semaforo;
}

int espacio_disponible_en_cache(void)
{
	return g_config_broker->tamano_memoria - g_cache_part->used_space;
}

void incremento_cnt_id_mensajes(void)
{
	g_msg_counter += 1;
}
