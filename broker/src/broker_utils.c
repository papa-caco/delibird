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

void inicio_server_broker(void)
{
	char *ip = g_config_broker->ip_broker;
	char *puerto = g_config_broker->puerto_broker;
	iniciar_server_broker(ip, puerto, g_logger);
}

void atender_cliente_broker(t_socket_cliente_broker *socket) {
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
	switch (cod_op) {
	case GET_BROKER:;
		t_msg_get_broker *msg_get = rcv_msj_get_broker(cliente_fd, g_logger);
		if(cache_espacio_suficiente(espacio_cache_msg_get(msg_get)) == true) {
			sem_wait(&g_mutex_msjs);
				enqueue_msg_get(msg_get, g_logger, cliente_fd);
			sem_post(&g_mutex_msjs);
		} else {
			enviar_mensaje_error(cliente_fd, g_logger, "BROKER_CACHE IS FULL - FORWARD MSG LATER");
			eliminar_msg_get_broker(msg_get);
		}
		break;
	case NEW_BROKER:;
		t_msg_new_broker *msg_new = rcv_msj_new_broker(cliente_fd, g_logger);
		int size_msg = espacio_cache_msg_new(msg_new);
		if(cache_espacio_suficiente(espacio_cache_msg_new(msg_new)) == true) {
			sem_wait(&g_mutex_msjs);
				enqueue_msg_new(msg_new, g_logger, cliente_fd);
			sem_post(&g_mutex_msjs);
		} else {
			enviar_mensaje_error(cliente_fd, g_logger, "BROKER_CACHE IS FULL - FORWARD MSG LATER");
			eliminar_msg_new_broker(msg_new);
		}
		break;
	case CATCH_BROKER:;
		t_msg_catch_broker *msg_catch = rcv_msj_catch_broker(cliente_fd, g_logger);
		if(cache_espacio_suficiente(espacio_cache_msg_catch(msg_catch)) == true) {
			sem_wait(&g_mutex_msjs);
				enqueue_msg_catch(msg_catch, g_logger, cliente_fd);
			sem_post(&g_mutex_msjs);
		} else {
			enviar_mensaje_error(cliente_fd, g_logger, "BROKER_CACHE IS FULL - FORWARD MSG LATER");
			eliminar_msg_catch_broker(msg_catch);
		}
		break;
	case APPEARED_BROKER:;
		t_msg_appeared_broker *msg_appeared = rcv_msj_appeared_broker(cliente_fd, g_logger);
		if(cache_espacio_suficiente(espacio_cache_msg_appeared(msg_appeared)) == true) {
			sem_wait(&g_mutex_msjs);
				enqueue_msg_appeared(msg_appeared, g_logger, cliente_fd);
			sem_post(&g_mutex_msjs);
		} else {
			enviar_mensaje_error(cliente_fd, g_logger, "BROKER_CACHE IS FULL - FORWARD MSG LATER");
			eliminar_msg_appeared_broker(msg_appeared);
		}break;
	case CAUGHT_BROKER:;
		t_msg_caught_broker *msg_caught = rcv_msj_caught_broker(cliente_fd, g_logger);
		if(cache_espacio_suficiente(espacio_cache_msg_caught(msg_caught)) == true) {
			sem_wait(&g_mutex_msjs);
				enqueue_msg_caught(msg_caught, g_logger, cliente_fd);
			sem_post(&g_mutex_msjs);
		} else {
			enviar_mensaje_error(cliente_fd, g_logger, "BROKER_CACHE IS FULL - FORWARD MSG LATER");
			free(msg_caught);
		}
		break;
	case LOCALIZED_BROKER:;
		t_msg_localized_broker *msg_localized = rcv_msj_localized_broker(cliente_fd, g_logdebug);
		if(cache_espacio_suficiente(espacio_cache_msg_localized(msg_localized)) == true) {
			sem_wait(&g_mutex_msjs);
				enqueue_msg_localized(msg_localized, g_logger, cliente_fd);
			sem_post(&g_mutex_msjs);
		} else {
			enviar_mensaje_error(cliente_fd, g_logger, "BROKER_CACHE IS FULL - FORWARD MSG LATER");
			eliminar_msg_localized_broker(msg_localized);
		}
		break;
	case FIN_SUSCRIPCION:;
		t_handsake_suscript *handshake = handshake = rcv_msj_handshake_suscriptor(cliente_fd);
		log_info(g_logger,"(RECEIVING: END_SUSCRIPTION|%s|ID_SUSCRIPTOR = %d|Socket# = %d)"
				,nombre_cola(handshake->cola_id),handshake->id_suscriptor, cliente_fd);
		deshabilitar_suscriptor_cola(handshake);
		sem_wait(&g_mutex_msjs);
			enviar_msg_confirmed(cliente_fd, g_logger);
		sem_post(&g_mutex_msjs);
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
	log_debug(g_logdebug,"(Tamaño_Cache:%d)", g_config_broker->tamano_memoria);
	log_debug(g_logdebug,"(Algoritmo_Memoria:%s)", config_get_string_value(g_config, "ALGORITMO_MEMORIA"));
	log_debug(g_logdebug,"(Algoritmo_Particion_Libre:%s)",config_get_string_value(g_config,"ALGORITMO_PARTICION_LIBRE"));
	log_debug(g_logdebug,"(Algoritmo_Reemplazo:%s)",config_get_string_value(g_config,"ALGORITMO_REEMPLAZO"));
	g_msg_counter = 1;
	sem_init(&g_mutex_msjs, 0, 1);
	sem_init(&g_mutex_queue_new, 0, 1);
	sem_init(&g_mutex_queue_get, 0, 1);
	sem_init(&g_mutex_queue_localized, 0, 1);
	sem_init(&g_mutex_queue_catch, 0, 1);
	sem_init(&g_mutex_queue_caught, 0, 1);
	sem_init(&g_mutex_queue_appeared, 0, 1);
	inicializar_colas_broker();
	g_team_suscriptos = list_create();
	g_gamecards_suscriptos = list_create();
	switch(algoritmo) {
	case PARTICIONES:;
		int tamano_cache = g_config_broker->tamano_memoria;
		g_cache_part = malloc(sizeof(t_cache_part_din));
		g_cache_part->id_partition = 1;
		g_cache_part->dir_base_part = 0;
		g_cache_part->used_space = 0;
		g_cache_part->total_space = g_config_broker->tamano_memoria;
		g_cache_part->partition_table = list_create();
		g_cache_part->partition_repo = malloc(tamano_cache);
		memset(g_cache_part->partition_repo, 0, tamano_cache);
	break;
	}
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
	int* id_sus = malloc(sizeof(int));
	t_list *lista_suscriptores = select_lista_suscriptores(id_cola);
	memcpy(id_sus, &(id_suscriptor), sizeof(int));
	if (! id_suscriptor_ya_ingresado(lista_suscriptores, id_sus)) {
		void *id = (int*) id_sus;
		list_add(lista_suscriptores,id);
	}
	free(id_sus);
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
	sem_t sem_cola = semaforo_cola(handshake->cola_id);
	sem_wait(&sem_cola);
		t_suscriptor_broker *suscriptor = obtengo_suscriptor_cola(cola_broker , id_suscriptor);
		suscriptor->enabled = false;
		int valor_sem;
		sem_getvalue(&suscriptor->sem_cont_msjs, &valor_sem);
		if (valor_sem == 0) {
			sem_post(&suscriptor->sem_cont_msjs);
		}
	sem_post(&sem_cola);
}

bool es_suscriptor_habilitado(t_broker_queue *cola_broker, sem_t semaforo_cola, int id_suscriptor)
{
	sem_wait(&semaforo_cola);
	t_suscriptor_broker *suscriptor = obtengo_suscriptor_cola(cola_broker, id_suscriptor);
	sem_post(&semaforo_cola);
	return suscriptor->enabled;
}

bool msj_enviado_a_todo_suscriptor(t_queue_msg *mensaje_cola)
{
	bool resultado;
	t_list *suscriptores = select_lista_suscriptores(mensaje_cola->tipo_mensaje);
	if (suscriptores->elements_count == mensaje_cola->sended_suscriptors->elements_count) {
		int coincidencias = 0;
		for (int i = 0; i < suscriptores->elements_count; i ++) {
			int *elem = list_get(mensaje_cola->sended_suscriptors,i);
			if (id_suscriptor_ya_ingresado(suscriptores,elem)){
				coincidencias ++;
			}
		}
		resultado = coincidencias == mensaje_cola->sended_suscriptors->elements_count;
	}
	else {
		resultado = false;
	}
	return resultado;
}

bool id_suscriptor_ya_ingresado(t_list *lista_suscriptores, int *id_suscriptor)
{
	bool mismo_id(void *id)
		{
			int  *suscript_id = (int*) id;
			bool condition = *id_suscriptor == *suscript_id;
			return condition;
		}
	return list_any_satisfy(lista_suscriptores,(void*) mismo_id);
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
	sem_destroy(&suscriptor->sem_cont_msjs);
	free(suscriptor);
}

void quitar_suscriptor_cola(t_broker_queue *cola_broker, void *suscriptor)
{
	t_suscriptor_broker *suscrip_t = (t_suscriptor_broker*) suscriptor;
	bool compare_suscript(void *otro_suscriptor){
		t_suscriptor_broker *otro_suscrip_t = (t_suscriptor_broker*) otro_suscriptor;
		bool condition = otro_suscrip_t->id_suscriptor == suscrip_t->id_suscriptor;
		return condition;
	}
	list_remove_by_condition(cola_broker->suscriptores, compare_suscript);
}

void eliminar_mensajes_cola(t_tipo_mensaje id_cola, int cant_mensajes, t_log *logger)
{
	t_broker_queue *cola_broker = cola_broker_suscripcion(id_cola);
	if (cola_broker->mensajes_cola->elements_count > 0) {
		int cant_a_borrar = cant_mensajes;
		t_list *msjs_enviados_a_todos = list_filter(cola_broker->mensajes_cola,(void*) msj_enviado_a_todo_suscriptor);
		if (msjs_enviados_a_todos->elements_count < cant_mensajes) {
			cant_a_borrar = msjs_enviados_a_todos->elements_count;
		}
		for (int i = 0; i < cant_a_borrar; i ++) {
			t_queue_msg* mensaje = list_get(msjs_enviados_a_todos, i);
			eliminar_mensaje_por_id(cola_broker, mensaje->id_mensaje, logger);
		}
		list_destroy(msjs_enviados_a_todos);
	}
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

int compactar_particiones_dinamicas(t_log *logger)
{
	reorganizar_particiones_dinamicas_libres(logger);
	int cant_part = g_cache_part->partition_table->elements_count;
	int size1 = ultimo_bloque_part_din();
	void *auxiliar;
	t_particion_dinamica* part0 = list_get(g_cache_part->partition_table,0);
	int dir_base_part_id1 = part0->dir_base;
	if (dir_base_part_id1 != 0) {   // Si La 1° particion ocupada no está al comienzo del cache
		uint32_t size_part_id1 = part0->data_size;
		auxiliar = malloc(size_part_id1);
		int desplazamiento = dir_base_part_id1;
		dir_base_part_id1 = 0;
		memcpy(auxiliar, (g_cache_part->partition_repo + desplazamiento), size_part_id1);
		memset((g_cache_part->partition_repo + dir_base_part_id1),0,desplazamiento);
		memcpy(g_cache_part->partition_repo + dir_base_part_id1, auxiliar, size_part_id1);
		part0->dir_base = dir_base_part_id1;
		free(auxiliar);
	}
	for (int i = 0; i < (cant_part - 1); i ++) {  //Solo entra si hay más de 2 particiones ocupadas
		int dir_base1 = ((t_particion_dinamica*)list_get(g_cache_part->partition_table, i))->dir_base;
		uint32_t size_part1 = ((t_particion_dinamica*)list_get(g_cache_part->partition_table, i))->data_size;
		int dir_base2_new = dir_base1 + size_part1;
		int dir_base2 = ((t_particion_dinamica*)list_get(g_cache_part->partition_table, i + 1))->dir_base;
		uint32_t size_part2 = ((t_particion_dinamica*)list_get(g_cache_part->partition_table,i + 1))->data_size;
		int id_part2 = ((t_particion_dinamica*)list_get(g_cache_part->partition_table,i + 1))->id_particion;
		bool status_part2 = ((t_particion_dinamica*)list_get(g_cache_part->partition_table,i + 1))->presencia;
		int gap = dir_base2 - dir_base2_new;
		if (gap > 0 )  {
			log_trace(logger,"(Comprimiendo_Partición:%d|%dBytes)", id_part2,gap);
			auxiliar = malloc(size_part2);
			int clean_bytes = gap + size_part2;
			memcpy(auxiliar, (g_cache_part->partition_repo + dir_base2), size_part2);
			memset((g_cache_part->partition_repo + dir_base2_new),0,clean_bytes);
			memcpy(g_cache_part->partition_repo + dir_base2_new, auxiliar, size_part2);
			t_particion_dinamica* particion = list_get(g_cache_part->partition_table,i + 1);
			particion->dir_base = dir_base2_new;
			free(auxiliar);
		}
	}
	int size2 = ultimo_bloque_part_din();
	int bytes_recup = size2 - size1;
	log_trace(logger,"(COMPACTADO_CACHE_PARTICIONES_DINAMICAS:%d Bytes Recuperados)", bytes_recup);
	return  dir_base_ultimo_bloque_part_din();
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
	t_list *particiones_libres = list_filter(g_cache_part->partition_table, (void*) es_particion_libre);
	return particiones_libres;
}

void reorganizar_particiones_dinamicas_libres(t_log *logger)
{
	t_list *particiones_ocupadas = list_filter(g_cache_part->partition_table, (void*) es_particion_ocupada);
	int cant_total = g_cache_part->partition_table->elements_count;
	t_list *particiones_libres =obtengo_particiones_dinamicas_libres();
	int a_borrar = particiones_libres->elements_count;
	list_destroy(particiones_libres);
	t_list *auxiliar = list_duplicate(particiones_ocupadas);
	list_destroy(particiones_ocupadas);
	list_destroy_and_destroy_elements(g_cache_part->partition_table,free);
	g_cache_part->partition_table = list_duplicate(auxiliar);
	list_destroy(auxiliar);
	int cant_elem = g_cache_part->partition_table->elements_count;
	for (int i = 0; i < cant_elem; i ++ ) {
		t_particion_dinamica* part = list_get(g_cache_part->partition_table,i);
		part->id_particion = i + 1;
	}
	g_cache_part->id_partition = cant_elem + 1;
	log_trace(logger, "(Reorganizando_Tabla_Particiones_Dinamicas|Total:%d|Libres:%d|Ocupadas:%d)", cant_total, a_borrar, cant_elem);
}

bool ordenar_por_direccion_base(void *part1, void *part2)
{
		t_particion_dinamica *particion1 = (t_particion_dinamica*) part1;
		t_particion_dinamica *particion2 = (t_particion_dinamica*) part2;
		return particion1->dir_base < particion2->dir_base;
}

bool ordenar_por_tamano(void *part1, void *part2)
{
		t_particion_dinamica *particion1 = (t_particion_dinamica*) part1;
		t_particion_dinamica *particion2 = (t_particion_dinamica*) part2;
		return particion1->data_size < particion2->data_size;
}

bool es_particion_libre(void *part) {
	t_particion_dinamica *particion = (t_particion_dinamica*) part;
	return particion->id_mensaje == 0 && particion->presencia == false;
}

bool es_particion_ocupada(void *part) {
	t_particion_dinamica *particion = (t_particion_dinamica*) part;
	return particion->id_mensaje != 0 && particion->presencia == true;
}

int dir_base_ultimo_bloque_part_din(void)
{
	int ultima_dir_base;
	t_list *tabla_particiones = g_cache_part->partition_table;
	list_sort(tabla_particiones, ordenar_por_direccion_base);
	if (tabla_particiones->elements_count > 0) {
		int index = tabla_particiones->elements_count - 1;
		int dir_ult_part = ((t_particion_dinamica*) list_get(tabla_particiones,index))->dir_base;
		int size_ult_part = ((t_particion_dinamica*) list_get(tabla_particiones,index))->data_size;
		ultima_dir_base = dir_ult_part + size_ult_part;
	} else {
		ultima_dir_base = 0;
	}
	return ultima_dir_base;
}

int ultimo_bloque_part_din(void)
{
	return g_cache_part->total_space - dir_base_ultimo_bloque_part_din();
}

bool sin_espacio_ult_bloque_cache_part_din(t_queue_msg *msg_queue)
{
	return msg_queue->msg_data->size > ultimo_bloque_part_din();
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
	g_config_broker->ruta_log = config_get_string_value(g_config, "LOG_FILE");
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

void iniciar_log_broker(void) {
	g_logger = log_create(g_config_broker->ruta_log, "BROKER", 1, LOG_LEVEL_TRACE);
	g_logdebug = log_create(RUTA_LOG_DEBUG, "BROKER_DBG", 1, LOG_LEVEL_DEBUG);
}
