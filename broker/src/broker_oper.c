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
	pthread_mutex_lock(&g_mutex_queue_get);
		msg_queue_get->dir_base_part_inicial = poner_datos_msj_en_particion_cache(msg_queue_get, logger);
		msg_queue_get->msg_data->data = realloc(msg_queue_get->msg_data->data, 1);
		memset(msg_queue_get->msg_data->data, 0, 1);
		poner_msj_en_cola(msg_queue_get, g_queue_get_pokemon);
	pthread_mutex_unlock(&g_mutex_queue_get);
	enviar_id_mensaje(cliente_fd, logger, msg_queue_get->id_mensaje);
	eliminar_msg_get_broker(msg_get_broker);
	incremento_cnt_id_mensajes();
}

void enqueue_msg_new(t_msg_new_broker *msg_new_broker, t_log *logger, int cliente_fd)
{
	t_queue_msg *msg_queue_new = generar_nuevo_mensaje_cola();
	msg_queue_new->tipo_mensaje = NEW_POKEMON;
	msg_queue_new->msg_data = serializar_msg_new(msg_new_broker);
	pthread_mutex_lock(&g_mutex_queue_new);
		msg_queue_new->dir_base_part_inicial = poner_datos_msj_en_particion_cache(msg_queue_new, logger);
		msg_queue_new->msg_data->data = realloc(msg_queue_new->msg_data->data, 1);
		memset(msg_queue_new->msg_data->data, 0, 1);
		poner_msj_en_cola(msg_queue_new, g_queue_new_pokemon);
	pthread_mutex_unlock(&g_mutex_queue_new);
	enviar_id_mensaje(cliente_fd, logger, msg_queue_new->id_mensaje);
	eliminar_msg_new_broker(msg_new_broker);
	incremento_cnt_id_mensajes();
}

void enqueue_msg_catch(t_msg_catch_broker *msg_catch_broker, t_log *logger, int cliente_fd)
{
	t_queue_msg *msg_queue_catch = generar_nuevo_mensaje_cola();
	msg_queue_catch->tipo_mensaje = CATCH_POKEMON;
	msg_queue_catch->msg_data = serializar_msg_catch(msg_catch_broker);
	pthread_mutex_lock(&g_mutex_queue_catch);
		msg_queue_catch->dir_base_part_inicial = poner_datos_msj_en_particion_cache(msg_queue_catch, logger);
		msg_queue_catch->msg_data->data = realloc(msg_queue_catch->msg_data->data, 1);
		memset(msg_queue_catch->msg_data->data, 0, 1);
		poner_msj_en_cola(msg_queue_catch, g_queue_catch_pokemon);
	pthread_mutex_unlock(&g_mutex_queue_catch);
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
	pthread_mutex_lock(&g_mutex_queue_appeared);
		msg_queue_appeared->dir_base_part_inicial = poner_datos_msj_en_particion_cache(msg_queue_appeared, logger);
		msg_queue_appeared->msg_data->data = realloc(msg_queue_appeared->msg_data->data, 1);
		memset(msg_queue_appeared->msg_data->data, 0, 1);
		poner_msj_en_cola(msg_queue_appeared, g_queue_appeared_pokemon);
	pthread_mutex_unlock(&g_mutex_queue_appeared);
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
	pthread_mutex_lock(&g_mutex_queue_caught);
		msg_queue_caught->dir_base_part_inicial = poner_datos_msj_en_particion_cache(msg_queue_caught, logger);
		msg_queue_caught->msg_data->data = realloc(msg_queue_caught->msg_data->data, 1);
		memset(msg_queue_caught->msg_data->data, 0, 1);
		poner_msj_en_cola(msg_queue_caught, g_queue_caught_pokemon);
	pthread_mutex_unlock(&g_mutex_queue_caught);
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
	pthread_mutex_lock(&g_mutex_queue_localized);
		msg_queue_localized->dir_base_part_inicial = poner_datos_msj_en_particion_cache(msg_queue_localized, logger);
		msg_queue_localized->msg_data->data = realloc(msg_queue_localized->msg_data->data, 1);
		memset(msg_queue_localized->msg_data->data, 0, 1);
		poner_msj_en_cola(msg_queue_localized, g_queue_localized_pokemon);
	pthread_mutex_unlock(&g_mutex_queue_localized);
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
		if (particion == NULL) {
			particion = reemplazar_particion_dinamica(msg, logger);
		}
		int offset =  particion->dir_base;
		memset((g_cache_part->partition_repo) + offset, 0, tamano_particion_dinamica(particion));
		memcpy((g_cache_part->partition_repo) + offset, msg->msg_data->data, particion->data_size);
		char *name_cola = nombre_cola(msg->tipo_mensaje);
		dir_base_particion = particion->dir_base;
		int espacio = g_cache_part->total_space - g_cache_part->used_space;
		log_debug(g_logger,"\n(%s|DATA_STORED|ID_PART:%d|START_ADDR:%d|END_ADDR:%d|DATA_SIZE:%d Bytes|QUEUE:%s|ID_MSG:%d|CACHE_SPACE:%d Bytes)\n",
			nombre_cache(g_cache_part->tipo_cache), particion->id_particion,particion->dir_base, particion->dir_heap, particion->data_size,
			name_cola, particion->id_mensaje,  espacio);
		break;
	case BS:;
		break;
	}
	return dir_base_particion;
}

t_particion_dinamica *buscar_particion_dinamica_libre(t_queue_msg *msg_queue)
{
	t_particion_dinamica *particion = NULL;
	t_list *particiones;
	t_algoritmo_part_libre algoritmo = g_config_broker->algoritmo_particion_libre;
	int tamano = 0;
	if ( msg_queue->msg_data->size > g_cache_part->min_size_part) {
		tamano = msg_queue->msg_data->size;
	} else {
		tamano = g_cache_part->min_size_part;
	}
	bool tamano_suficiente(void *part) {
		t_particion_dinamica *particion = (t_particion_dinamica*) part;
		return tamano_particion_dinamica(particion) >= tamano;
	}
	switch (algoritmo) {
	case FF:;
		particiones = ordenar_particiones_libres_first_fit();
		particion = list_find(particiones, (void*) tamano_suficiente);
		list_destroy(particiones);
		break;
	case BF:;
		particiones = ordenar_particiones_libres_best_fit();
		particion = list_find(particiones, (void*) tamano_suficiente);
		list_destroy(particiones);
		break;
	}
	if (particion != NULL) {
		int idx_part = particion->id_particion - 1;
		if(idx_part > 0) { //Si la part_previa está lbre consolido con la que iba a usar.
			t_particion_dinamica *prev_part = list_get(g_cache_part->partition_table, (idx_part - 1));
			if (!prev_part->presencia) {
				prev_part = consolidar_particion_dinamica(particion, g_logger);
				particion = prev_part;
			}
		}
		int tamano_original = tamano_particion_dinamica(particion);
		int tamano_minimo = 0;
		particion->id_mensaje = msg_queue->id_mensaje;
		particion->id_cola = msg_queue->tipo_mensaje;
		particion->data_size = msg_queue->msg_data->size;
		particion->orden_fifo = g_cache_part->cnt_order_fifo;
		particion->last_used = g_cache_part->cnt_order_lru;
		particion->presencia = true;
		g_cache_part->cnt_order_fifo += 1;
		g_cache_part->cnt_order_lru += 1;
		if (particion->data_size > g_cache_part->min_size_part) {
			tamano_minimo = particion->data_size;
		} else {
			tamano_minimo = g_cache_part->min_size_part;
		}
		if (tamano_original > tamano_minimo){
			particion->dir_heap = particion->dir_base + tamano_minimo - 1;
			int tamano_bloque_libre = tamano_original - tamano_minimo;
			pthread_mutex_lock(&g_mutex_cache_part);
			generar_particion_dinamica_libre((particion->dir_heap + 1), tamano_bloque_libre);
			pthread_mutex_unlock(&g_mutex_cache_part);
		}
		g_cache_part->used_space += tamano_particion_dinamica(particion);
	}
	int cant_part = g_cache_part->partition_table->elements_count;
	//TODO printf("tabla_part -- cantidad_particiones:%d\n",cant_part);
	return particion;
}

t_particion_dinamica *particion_proxima_victima(int index)
{
	t_particion_dinamica *particion;
	t_list *sorted_table;
	t_algoritmo_reemplazo algoritmo = g_config_broker->algoritmo_reemplazo;
	t_particion_dinamica *new_part = NULL;
	switch(algoritmo) {
	case FIFO:;
		sorted_table = list_sorted(g_cache_part->partition_table, ordenador_fifo);
		break;
	case LRU:;
		sorted_table = list_sorted(g_cache_part->partition_table, ordenador_lru);
		break;
	}
	particion = list_get(sorted_table, index);
	list_destroy(sorted_table);
	return particion;
}

t_particion_dinamica *reemplazar_particion_dinamica(t_queue_msg *msg, t_log *logger)
{
	uint32_t data_size = msg->msg_data->size;
	int frecuencia = g_config_broker->frecuencia_compactacion;
	int tamano2 = 0, tamano1 = 0, id_part = 0, idx_part = 0, cant_part = 0, contador_reemplazos = 0, cont_consolidar = 0;
	bool compactada = false, reemplazo = false;
	t_particion_dinamica *new_part = NULL, *prev_part = NULL;
	if (frecuencia == 0) {
		compactar_particiones(g_cache_part, logger);
	}
	t_particion_dinamica *particion = particion_proxima_victima(0);
	do {
		//sleep(1);
		tamano1 = tamano_particion_dinamica(particion);
		reemplazo = false;
		if (particion->presencia) {
			t_broker_queue *cola_broker = cola_broker_suscripcion(particion->id_cola);
			eliminar_mensaje_por_id(cola_broker, particion->id_mensaje, logger);
			contador_reemplazos ++;
			reemplazo = true;
		}
		new_part = consolidar_particion_dinamica(particion , logger);
		cont_consolidar += 1;
		compactada = false;
		if (frecuencia != -1 && (frecuencia == 0 || frecuencia == contador_reemplazos)) {
			compactar_particiones(g_cache_part, logger);
			//puts("");// TODO
			new_part = ultima_particion_libre();
			contador_reemplazos = 0;
			compactada = true;
		}
		cant_part = g_cache_part->partition_table->elements_count;
		//printf("reemp:%d|consolid:%d| cant_part%d\n",contador_reemplazos, cont_consolidar, cant_part);
		tamano2 = tamano_particion_dinamica(new_part);
		if (cont_consolidar > 500) {
			pthread_exit(NULL);
		}
		if (tamano2 < data_size) {
		if (reemplazo && !compactada) {
			pthread_mutex_lock(&g_mutex_cache_part);
			//puts("1");
			particion =  particion_proxima_victima(1);
			pthread_mutex_unlock(&g_mutex_cache_part);
			cont_consolidar = 0;
		} else if( !reemplazo && !compactada && tamano1 == tamano2){
			int idx = 0;
			if (cont_consolidar < cant_part) {
				idx = cont_consolidar;
			} else {
				idx = cant_part - 1;
			}
			pthread_mutex_lock(&g_mutex_cache_part);
			//printf("2 - idx:%d\n",idx);
			particion =  particion_proxima_victima(idx);
			pthread_mutex_unlock(&g_mutex_cache_part);
		} else { // Si Compacté y no me alcanza el tamaño de la partición libre,
			pthread_mutex_lock(&g_mutex_cache_part);  //busco reemplazando la próxima víctima.
			//puts("3");
			particion = particion_proxima_victima(0);
			pthread_mutex_unlock(&g_mutex_cache_part);
		}
		}
	}while (tamano2 < data_size);
	if (new_part != NULL) {
		int tamano_original = tamano_particion_dinamica(new_part);
		int tamano_minimo = 0;
		new_part->orden_fifo = g_cache_part->cnt_order_fifo;
		new_part->last_used = g_cache_part->cnt_order_lru;
		new_part->id_mensaje = msg->id_mensaje;
		new_part->id_cola = msg->tipo_mensaje;
		new_part->data_size = msg->msg_data->size;
		new_part->presencia = true;
		g_cache_part->cnt_order_fifo += 1;
		g_cache_part->cnt_order_lru += 1;
		if (new_part->data_size > g_cache_part->min_size_part) {
			tamano_minimo = new_part->data_size;
		} else {
			tamano_minimo = g_cache_part->min_size_part;
		}
		if (tamano_original > tamano_minimo){
			new_part->dir_heap = new_part->dir_base + tamano_minimo - 1;
			int tamano_bloque_libre = tamano_original - tamano_minimo;
			pthread_mutex_lock(&g_mutex_cache_part);
			generar_particion_dinamica_libre((new_part->dir_heap + 1), tamano_bloque_libre);
			pthread_mutex_unlock(&g_mutex_cache_part);
		}
		g_cache_part->used_space += tamano_particion_dinamica(new_part);
	}
	return new_part;
}

t_particion_dinamica *consolidar_particion_dinamica(t_particion_dinamica *particion, t_log *logger)
{
	int cant_part = g_cache_part->partition_table->elements_count;
	t_particion_dinamica *result_part = NULL;
	if (cant_part > 1) {
		int index_part = particion->id_particion - 1;

		int index_prev = index_part - 1;
		int index_post = index_part + 1;
		int idx_consolida = 0;
		t_particion_dinamica *consolidada;
		if (index_part == 0) { // -->> No es la 1° particion de la tabla <<--
			result_part = particion;
			idx_consolida = index_post;
			consolidada = list_get(g_cache_part->partition_table, index_post);
		} else {
			result_part = list_get(g_cache_part->partition_table, index_prev);
			idx_consolida = index_part;
			consolidada = particion;
		}
		if (!(result_part->presencia) && !(consolidada->presencia)) {
			result_part->dir_heap = consolidada->dir_heap;
			result_part->id_mensaje = consolidada->id_mensaje;
			result_part->orden_fifo = consolidada->orden_fifo;
			result_part->last_used = consolidada->last_used;
			/*TODO printf("consolido:id%d|base:%d|heap:%d\n",
				result_part->id_particion,result_part->dir_base,result_part->dir_heap);*/
			pthread_mutex_lock(&g_mutex_cache_part);// Abajo elimino la partición que traje como argumento, me quedo con la anterior
			list_remove_and_destroy_element(g_cache_part->partition_table, idx_consolida, free);
			for (int i = 0; i < g_cache_part->partition_table->elements_count; i++) {
				t_particion_dinamica *part = list_get(g_cache_part->partition_table,i);
				part->id_particion = i + 1;
			}
			pthread_mutex_unlock(&g_mutex_cache_part);
		} else {
			result_part = particion;
			//TODO printf("No Cosolido - vecina ocupada|%d|%d\n",result_part->id_particion, particion->id_particion);
		}
	} else {
		result_part = particion;
	}
	log_info(logger,"(Partición_Consolidada|ID_PART:%d|START_ADDR:%d|END_ADDR:%d|SIZE:%d Bytes)\n",
					result_part->id_particion, result_part->dir_base,  result_part->dir_heap,tamano_particion_dinamica(result_part));
	return result_part;
}

void obtener_datos_msj_de_particion_cache(t_queue_msg *msg_queue, t_log *logger)
{
	t_algoritmo_memoria algoritmo = g_config_broker->algoritmo_memoria;
	switch (algoritmo) {
	case PARTICIONES:;
		t_particion_dinamica *particion = NULL;
		particion = get_particion_cache_por_id_mensaje(g_cache_part, msg_queue->id_mensaje);
		if (particion->data_size != msg_queue->msg_data->size) {
			log_error(logger,"(INVALID_CACHE_PARTITION)");
			pthread_exit(NULL);
		}
		int offset = particion->dir_base;
		msg_queue->msg_data->data = realloc(msg_queue->msg_data->data, msg_queue->msg_data->size);
		memcpy(msg_queue->msg_data->data, g_cache_part->partition_repo + offset, particion->data_size);
		particion->last_used = g_cache_part->cnt_order_lru;
		g_cache_part->cnt_order_lru += 1; // -->> para el ordenado por LRU <<--
		break;
	}
}

void eliminar_msg_data_particion_cache(int id_mensaje, t_log *logger)
{
	t_algoritmo_memoria algoritmo = g_config_broker->algoritmo_memoria;
	switch (algoritmo) {
	case PARTICIONES:;
		t_particion_dinamica *particion = NULL;
		int espacio = 0;
		particion = get_particion_cache_por_id_mensaje(g_cache_part, id_mensaje);
		if (particion != NULL) {
			int offset = particion->dir_base;
			memset((g_cache_part->partition_repo) + offset, 0, tamano_particion_dinamica(particion));
			g_cache_part->used_space -= tamano_particion_dinamica(particion);
			espacio = g_cache_part->total_space - g_cache_part->used_space;
			log_trace(logger,"\n(%s|REPLACE-REMOVED_DATA|ID_PART:%d|START_ADDR:%d|END_ADDR:%d|PART_SIZE:%d Bytes|%s|ID_MSG:%d|CACHE_SPACE:%d Bytes)\n",
				nombre_cache(algoritmo), particion->id_particion,particion->dir_base, particion->dir_heap, tamano_particion_dinamica(particion),
				nombre_cola(particion->id_cola),particion->id_mensaje,espacio);
			particion->id_cola = 0; // -->> Actualizo metadata de la partición liberada <<--
			particion->data_size = 0;
			particion->presencia = false;
		} else {
			log_error(logger,"(INVALID_CACHE_PARTITION)");
			pthread_exit(NULL);
		}
		break;
	}
}

t_particion_dinamica *get_particion_cache_por_id_mensaje(t_cache_particiones *cache, int id_mensaje)
{
	bool mismo_id_msj(void *elem)
	{
		t_particion_dinamica *part = (t_particion_dinamica*) elem;
		bool condition = part->id_mensaje == id_mensaje;
		return condition;
	}
	t_particion_dinamica *particion = list_find(cache->partition_table, mismo_id_msj);
	return particion;
}

t_particion_dinamica *get_particion_cache_por_id_particion(t_cache_particiones *cache, int id_partcion)
{
	bool mismo_id_particion(void *elem)
	{
		t_particion_dinamica *part = (t_particion_dinamica*) elem;
		bool condition = part->id_particion == id_partcion;
		return condition;
	}
	t_particion_dinamica *particion = list_find(cache->partition_table, mismo_id_particion);
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
	pthread_mutex_lock(&g_mutex_msjs);
	msg_queue_get = get_msg_sin_enviar(g_queue_get_pokemon, suscriptor->id_suscriptor);
	if (msg_queue_get == NULL) {
		sent_bytes = enviar_msj_cola_vacia(socket, logger, suscriptor->id_suscriptor);
	} else {
		obtener_datos_msj_de_particion_cache(msg_queue_get, g_logger);
		t_msg_get_gamecard *msg_get_gamecard = deserializar_msg_get(msg_queue_get->msg_data);
		msg_get_gamecard->id_mensaje = msg_queue_get->id_mensaje;
		sent_bytes = enviar_msj_get_gamecard(socket, logger, msg_get_gamecard);
		eliminar_msg_get_gamecard(msg_get_gamecard);
	}
	pthread_mutex_unlock(&g_mutex_msjs);
	return sent_bytes;
}

ssize_t despachar_msjs_new(t_socket_cliente_broker *socket, int ult_id_recibido, t_suscriptor_broker *suscriptor, t_log * logger)
{
	t_queue_msg *msg_queue_new;
	ssize_t sent_bytes = 0;
	set_msg_enviado_a_suscriptor(g_queue_new_pokemon, suscriptor->id_suscriptor, ult_id_recibido);
	sem_wait(&suscriptor->sem_cont_msjs);
	pthread_mutex_lock(&g_mutex_msjs);
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
	pthread_mutex_unlock(&g_mutex_msjs);
	return sent_bytes;
}

ssize_t despachar_msjs_catch(t_socket_cliente_broker *socket, int ult_id_recibido, t_suscriptor_broker *suscriptor, t_log * logger)
{
	t_queue_msg *msg_queue_catch;
	ssize_t sent_bytes = 0;
	set_msg_enviado_a_suscriptor(g_queue_catch_pokemon, suscriptor->id_suscriptor, ult_id_recibido);
	sem_wait(&suscriptor->sem_cont_msjs);
	pthread_mutex_lock(&g_mutex_msjs);
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
	pthread_mutex_unlock(&g_mutex_msjs);
	return sent_bytes;
}

ssize_t despachar_msjs_localized(t_socket_cliente_broker *socket, int ult_id_recibido, t_suscriptor_broker *suscriptor, t_log * logger)
{
	t_queue_msg *msg_queue_localized;
	ssize_t sent_bytes = 0;
	set_msg_enviado_a_suscriptor(g_queue_localized_pokemon, suscriptor->id_suscriptor, ult_id_recibido);
	sem_wait(&suscriptor->sem_cont_msjs);
	pthread_mutex_lock(&g_mutex_msjs);
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
	pthread_mutex_unlock(&g_mutex_msjs);
	return sent_bytes;
}

ssize_t despachar_msjs_appeared(t_socket_cliente_broker *socket, int ult_id_recibido, t_suscriptor_broker *suscriptor, t_log * logger)
{
	t_queue_msg *msg_queue_appeared;
	ssize_t sent_bytes = 0;
	set_msg_enviado_a_suscriptor(g_queue_appeared_pokemon, suscriptor->id_suscriptor, ult_id_recibido);
	sem_wait(&suscriptor->sem_cont_msjs);
	pthread_mutex_lock(&g_mutex_msjs);
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
	pthread_mutex_unlock(&g_mutex_msjs);
	return sent_bytes;
}

ssize_t despachar_msjs_caught(t_socket_cliente_broker *socket, int ult_id_recibido, t_suscriptor_broker *suscriptor, t_log * logger)
{
	t_queue_msg *msg_queue_caught;
	ssize_t sent_bytes = 0;
	set_msg_enviado_a_suscriptor(g_queue_caught_pokemon, suscriptor->id_suscriptor, ult_id_recibido);
	sem_wait(&suscriptor->sem_cont_msjs);
	pthread_mutex_lock(&g_mutex_msjs);
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
	}
	pthread_mutex_unlock(&g_mutex_msjs);
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

pthread_mutex_t *semaforo_cola(t_tipo_mensaje tipo_mensaje)
{
	pthread_mutex_t *semaforo;
	switch (tipo_mensaje) {
	case CATCH_POKEMON:;
		semaforo = &g_mutex_queue_catch;
		break;
	case CAUGHT_POKEMON:;
		semaforo = &g_mutex_queue_caught;
		break;
	case NEW_POKEMON:;
		semaforo = &g_mutex_queue_new;
		break;
	case APPEARED_POKEMON:
		semaforo = &g_mutex_queue_appeared;
		break;
	case GET_POKEMON:;
		semaforo = &g_mutex_queue_get;
		break;
	case LOCALIZED_POKEMON:;
		semaforo = &g_mutex_queue_localized;
		break;
	}
	return semaforo;
}

int espacio_disponible_en_cache(t_cache_particiones *cache)
{
	return cache->total_space - cache->used_space;
}

int tamano_particiones_libres(int index)
{
	int tamano = 0;
	t_list *particiones = list_filter(g_cache_part->partition_table,es_particion_libre);
	if (particiones->elements_count > 0) {
		for (int i = index; i < particiones->elements_count; i ++) {
			tamano += tamano_particion_dinamica((t_particion_dinamica*) list_get(particiones, i));
		}
	}
	list_destroy(particiones);
	return tamano;
}

void incremento_cnt_id_mensajes(void)
{
	g_msg_counter += 1;
}
