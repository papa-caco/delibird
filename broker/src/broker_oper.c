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
	t_queue_msg *msg_queue_get = new_queue_msg();
	msg_queue_get->tipo_mensaje = GET_POKEMON;
	msg_queue_get->msg_data = serializar_msg_get(msg_get_broker);
	guardar_datos_en_segmento(msg_queue_get, logger);
	sem_wait(&g_mutex_queue_get);
		poner_msj_en_cola(msg_queue_get, g_queue_get_pokemon);
	sem_post(&g_mutex_queue_get);
	enviar_id_mensaje(cliente_fd, logger, msg_queue_get->id_mensaje);
	eliminar_msg_get_broker(msg_get_broker);
	incremento_cnt_id_mensajes();
}

void enqueue_msg_new(t_msg_new_broker *msg_new_broker, t_log *logger, int cliente_fd)
{
	t_queue_msg *msg_queue_new = new_queue_msg();
	msg_queue_new->tipo_mensaje = NEW_POKEMON;
	msg_queue_new->msg_data = serializar_msg_new(msg_new_broker);
	guardar_datos_en_segmento(msg_queue_new, logger);
	sem_wait(&g_mutex_queue_new);
		poner_msj_en_cola(msg_queue_new, g_queue_new_pokemon);
	sem_post(&g_mutex_queue_new);
	enviar_id_mensaje(cliente_fd, logger, msg_queue_new->id_mensaje);
	eliminar_msg_new_broker(msg_new_broker);
	incremento_cnt_id_mensajes();
}

void enqueue_msg_catch(t_msg_catch_broker *msg_catch_broker, t_log *logger, int cliente_fd)
{
	t_queue_msg *msg_queue_catch = new_queue_msg();
	msg_queue_catch->tipo_mensaje = CATCH_POKEMON;
	msg_queue_catch->msg_data = serializar_msg_catch(msg_catch_broker);
	guardar_datos_en_segmento(msg_queue_catch, logger);
	sem_wait(&g_mutex_queue_catch);
		poner_msj_en_cola(msg_queue_catch, g_queue_catch_pokemon);
	sem_post(&g_mutex_queue_catch);
	enviar_id_mensaje(cliente_fd, logger, msg_queue_catch->id_mensaje);
	eliminar_msg_catch_broker(msg_catch_broker);
	incremento_cnt_id_mensajes();
}

void enqueue_msg_appeared(t_msg_appeared_broker *msg_appeared_broker, t_log *logger, int cliente_fd)
{
	t_queue_msg *msg_queue_appeared = new_queue_msg();
	msg_queue_appeared->id_correlativo = msg_appeared_broker->id_correlativo;
	msg_queue_appeared->tipo_mensaje = APPEARED_POKEMON;
	msg_queue_appeared->msg_data = serializar_msg_appeared(msg_appeared_broker);
	guardar_datos_en_segmento(msg_queue_appeared, logger);
	sem_wait(&g_mutex_queue_appeared);
		poner_msj_en_cola(msg_queue_appeared, g_queue_appeared_pokemon);
	sem_post(&g_mutex_queue_appeared);
	enviar_id_mensaje(cliente_fd, logger, msg_queue_appeared->id_mensaje);
	eliminar_msg_appeared_broker(msg_appeared_broker);
	incremento_cnt_id_mensajes();
}

void enqueue_msg_caught(t_msg_caught_broker *msg_caught_broker, t_log *logger, int cliente_fd)
{
	t_queue_msg *msg_queue_caught = new_queue_msg();
	msg_queue_caught->tipo_mensaje = CAUGHT_POKEMON;
	msg_queue_caught->id_correlativo = msg_caught_broker->id_correlativo;
	msg_queue_caught->msg_data = serializar_msg_caught(msg_caught_broker);
	guardar_datos_en_segmento(msg_queue_caught, logger);
	sem_wait(&g_mutex_queue_caught);
		poner_msj_en_cola(msg_queue_caught, g_queue_caught_pokemon);
	sem_post(&g_mutex_queue_caught);
	enviar_id_mensaje(cliente_fd, logger, msg_queue_caught->id_mensaje);
	free(msg_caught_broker);
	incremento_cnt_id_mensajes();
}

void enqueue_msg_localized(t_msg_localized_broker *msg_localized_broker, t_log *logger, int cliente_fd)
{
	t_queue_msg *msg_queue_localized = new_queue_msg();
	msg_queue_localized->id_correlativo = msg_localized_broker->id_correlativo;
	msg_queue_localized->tipo_mensaje = LOCALIZED_POKEMON;
	msg_queue_localized->msg_data = serializar_msg_localized(msg_localized_broker);
	guardar_datos_en_segmento(msg_queue_localized, logger);
	sem_wait(&g_mutex_queue_localized);
		poner_msj_en_cola(msg_queue_localized, g_queue_localized_pokemon);
	sem_post(&g_mutex_queue_localized);
	enviar_id_mensaje(cliente_fd, logger, msg_queue_localized->id_mensaje);
	eliminar_msg_localized_broker(msg_localized_broker);
	incremento_cnt_id_mensajes();
}

t_queue_msg *new_queue_msg(void)
{
	t_queue_msg *queue_msg = malloc(sizeof(t_queue_msg));
	queue_msg->id_mensaje = g_msg_counter;
	queue_msg->id_correlativo = 0;
	queue_msg->id_cache_segmemt = 0;
	queue_msg->msg_data = NULL;
	queue_msg->es_victima_reemplazo = false;
	queue_msg->sended_suscriptors = list_create();
	queue_msg->receipt_confirmed = list_create();
	return queue_msg;
}

void guardar_datos_en_segmento(t_queue_msg *msg_queue, t_log *logger)
{
	//TODO
	msg_queue->id_cache_segmemt = g_cache_segment_id;
	incremento_id_partition();
	int part_addr_start = g_cache_space_used;
	incremento_espacio_cache_usado(msg_queue->msg_data->size);
	char *cola = nombre_cola(msg_queue->tipo_mensaje);
	int espacio = espacio_disponible_en_cache();
	log_info(logger,"(DATA_STORED|PARTITION:%d| START_ADDRESS 0x%X|QUEUE:%s|ID_MSG:%d|SIZE:%d Bytes|CACHE_SPACE:%d Bytes)",
			msg_queue->id_cache_segmemt, part_addr_start ,cola, msg_queue->id_mensaje, msg_queue->msg_data->size, espacio);
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

void despachar_mensaje_a_suscriptor(t_socket_cliente_broker *socket, t_tipo_mensaje id_cola, t_suscriptor_broker * suscriptor, t_log *logger)
{
	switch(id_cola) {
		case GET_POKEMON:;
			despachar_msjs_get(socket, suscriptor, logger);
			break;
		case NEW_POKEMON:;
			despachar_msjs_new(socket, suscriptor, logger);
			break;
		case CATCH_POKEMON:;
			despachar_msjs_catch(socket, suscriptor, logger);
			break;
		case LOCALIZED_POKEMON:;
			despachar_msjs_localized(socket, suscriptor, logger);
			break;
		case APPEARED_POKEMON:;
			despachar_msjs_appeared(socket, suscriptor, logger);
			break;
		case CAUGHT_POKEMON:;
			despachar_msjs_caught(socket, suscriptor, logger);
			break;
	}
}

void despachar_msjs_get(t_socket_cliente_broker *socket,t_suscriptor_broker *suscriptor,t_log *logger)
{
	t_queue_msg *msg_queue_get;
	// Queda en espera si recibió todos los msjs hasta el próximo mensaje en cola o fin de suscripción
	sem_wait(&suscriptor->sem_cont_msjs);
	sem_wait(&g_mutex_msjs);
	msg_queue_get = get_msg_sin_enviar(g_queue_get_pokemon, suscriptor->id_suscriptor);
	if (msg_queue_get == NULL) {
		enviar_msj_cola_vacia(socket, logger, suscriptor->id_suscriptor);
	} else {
		t_msg_get_gamecard *msg_get_gamecard = deserializar_msg_get(msg_queue_get->msg_data);
		msg_get_gamecard->id_mensaje = msg_queue_get->id_mensaje;
		enviar_msj_get_gamecard(socket, logger, msg_get_gamecard);
		eliminar_msg_get_gamecard(msg_get_gamecard);
	}
	sem_post(&g_mutex_msjs);
}

void despachar_msjs_new(t_socket_cliente_broker *socket, t_suscriptor_broker *suscriptor, t_log * logger)
{
	t_queue_msg *msg_queue_new;
	sem_wait(&suscriptor->sem_cont_msjs);
	sem_wait(&g_mutex_msjs);
	msg_queue_new =  get_msg_sin_enviar(g_queue_new_pokemon, suscriptor->id_suscriptor);
	if (msg_queue_new == NULL) {
		enviar_msj_cola_vacia(socket, logger, suscriptor->id_suscriptor);
	} else {
		t_msg_new_gamecard *msg_new_gamecard = deserializar_msg_new(msg_queue_new->msg_data);
		msg_new_gamecard->id_mensaje = msg_queue_new->id_mensaje;
		enviar_msj_new_gamecard(socket, logger, msg_new_gamecard);
		eliminar_msg_new_gamecard(msg_new_gamecard);
	}
	sem_post(&g_mutex_msjs);
}

void despachar_msjs_catch(t_socket_cliente_broker *socket, t_suscriptor_broker *suscriptor, t_log * logger)
{
	t_queue_msg *msg_queue_catch;
	sem_wait(&suscriptor->sem_cont_msjs);
	sem_wait(&g_mutex_msjs);
	msg_queue_catch = get_msg_sin_enviar(g_queue_catch_pokemon, suscriptor->id_suscriptor);
	if (msg_queue_catch == NULL) {
		enviar_msj_cola_vacia(socket, logger, suscriptor->id_suscriptor);
	} else {
		t_msg_catch_gamecard *msg_catch_gamecard = deserializar_msg_catch(msg_queue_catch->msg_data);
		msg_catch_gamecard->id_mensaje = msg_queue_catch->id_mensaje;
		enviar_msj_catch_gamecard(socket, logger, msg_catch_gamecard);
		eliminar_msg_catch_gamecard(msg_catch_gamecard);
	}
	sem_post(&g_mutex_msjs);
}

void despachar_msjs_localized(t_socket_cliente_broker *socket, t_suscriptor_broker *suscriptor, t_log * logger)
{
	t_queue_msg *msg_queue_localized;
	sem_wait(&suscriptor->sem_cont_msjs);
	sem_wait(&g_mutex_msjs);
	msg_queue_localized = get_msg_sin_enviar(g_queue_localized_pokemon, suscriptor->id_suscriptor);
	if (msg_queue_localized == NULL) {
		enviar_msj_cola_vacia(socket, logger, suscriptor->id_suscriptor);
	} else {
		t_msg_localized_team *msg_localized_team = deserializar_msg_localized(msg_queue_localized->msg_data);
		msg_localized_team->id_mensaje = msg_queue_localized->id_mensaje;
		msg_localized_team->id_correlativo = msg_queue_localized->id_correlativo;
		enviar_msj_localized_team(socket, logger, msg_localized_team);
		eliminar_msg_localized_team(msg_localized_team);
	}
	sem_post(&g_mutex_msjs);
}

void despachar_msjs_appeared(t_socket_cliente_broker *socket, t_suscriptor_broker *suscriptor, t_log * logger)
{
	t_queue_msg *msg_queue_appeared;
	sem_wait(&suscriptor->sem_cont_msjs);
	sem_wait(&g_mutex_msjs);
	msg_queue_appeared = get_msg_sin_enviar(g_queue_appeared_pokemon, suscriptor->id_suscriptor);
	if (msg_queue_appeared == NULL) {
		enviar_msj_cola_vacia(socket, logger, suscriptor->id_suscriptor);
	} else {
		t_msg_appeared_team *msg_appeared_team = deserializar_msg_appeared(msg_queue_appeared->msg_data);
		msg_appeared_team->id_mensaje = msg_queue_appeared->id_mensaje;
		msg_appeared_team->id_correlativo = msg_queue_appeared->id_correlativo;
		enviar_msj_appeared_team(socket, logger, msg_appeared_team);
		eliminar_msg_appeared_team(msg_appeared_team);
	}
	sem_post(&g_mutex_msjs);
}

void despachar_msjs_caught(t_socket_cliente_broker *socket, t_suscriptor_broker *suscriptor, t_log * logger)
{
	t_queue_msg *msg_queue_caught;
	sem_wait(&suscriptor->sem_cont_msjs);
	sem_wait(&g_mutex_msjs);
	msg_queue_caught = get_msg_sin_enviar(g_queue_caught_pokemon, suscriptor->id_suscriptor);
	if (msg_queue_caught == NULL) {
		enviar_msj_cola_vacia(socket, logger, suscriptor->id_suscriptor);
	}
	else {
		t_msg_caught_team *msg_caught_team = deserializar_msg_caught(msg_queue_caught->msg_data);
		msg_caught_team->id_mensaje = msg_queue_caught->id_mensaje;
		msg_caught_team->id_correlativo = msg_queue_caught->id_correlativo;
		enviar_msj_caught_team(socket, logger, msg_caught_team);
	}
	sem_post(&g_mutex_msjs);
}


t_queue_msg *get_msg_sin_enviar(t_broker_queue *cola_broker,int id_suscriptor)
{
	int cant_msj = cola_broker->mensajes_cola->elements_count;
	bool no_fue_enviado(void *mensaje)
	{
		t_queue_msg *queue_msg = (t_queue_msg*) mensaje;
		bool condition = es_msj_sin_enviar(queue_msg,id_suscriptor);
		return condition;
	}

	t_queue_msg *queue_msg = list_find(cola_broker->mensajes_cola, no_fue_enviado);
	//TODO traer los datos del mensaje que estan en la particion
	if (queue_msg != NULL) {
		void *suscript_id = malloc(sizeof(int));
		memcpy(suscript_id,&id_suscriptor,sizeof(int));
		list_add(queue_msg->sended_suscriptors,suscript_id);
	}
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
	int cant_msjs = msjs_sin_enviar->elements_count;
	return cant_msjs;
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


bool cache_espacio_suficiente(int data_size)
{
	return espacio_disponible_en_cache() >= data_size;
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
	return g_config_broker->tamano_memoria - g_cache_space_used;
}

void incremento_espacio_cache_usado(int tamano)
{
	g_cache_space_used += tamano;
}

void incremento_cnt_id_mensajes(void)
{
	g_msg_counter += 1;
}

void incremento_id_partition(void)
{
	g_cache_segment_id += 2;
}
