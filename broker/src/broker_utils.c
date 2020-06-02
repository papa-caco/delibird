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
	g_msg_counter = 1;
	g_cache_segment_id = 1;
	g_cache_space_used = 0;
	sem_init(&g_mutex_msjs, 0, 1);
	sem_init(&g_mutex_queue_new, 0, 1);
	sem_init(&g_mutex_queue_get, 0, 1);
	sem_init(&g_mutex_queue_localized, 0, 1);
	sem_init(&g_mutex_queue_catch, 0, 1);
	sem_init(&g_mutex_queue_caught, 0, 1);
	sem_init(&g_mutex_queue_appeared, 0, 1);
	inicializar_cola_get_pokemon();
	inicializar_cola_new_pokemon();
	inicializar_cola_catch_pokemon();
	inicializar_cola_appeared_pokemon();
	inicializar_cola_localized_pokemon();
	inicializar_cola_caught_pokemon();
}

void inicializar_cola_get_pokemon(void)
{
	g_queue_get_pokemon = malloc(sizeof(t_broker_queue));
	g_queue_get_pokemon->id_queue = GET_POKEMON;
	g_queue_get_pokemon->mensajes_cola = list_create();
	g_queue_get_pokemon->suscriptores =  list_create();
}

void inicializar_cola_new_pokemon(void)
{
	g_queue_new_pokemon = malloc(sizeof(t_broker_queue));
	g_queue_new_pokemon->id_queue = NEW_POKEMON;
	g_queue_new_pokemon->mensajes_cola = list_create();
	g_queue_new_pokemon->suscriptores =  list_create();
}

void inicializar_cola_catch_pokemon(void)
{
	g_queue_catch_pokemon = malloc(sizeof(t_broker_queue));
	g_queue_catch_pokemon->id_queue = CATCH_POKEMON;
	g_queue_catch_pokemon->mensajes_cola = list_create();
	g_queue_catch_pokemon->suscriptores =  list_create();
}

void inicializar_cola_appeared_pokemon(void)
{
	g_queue_appeared_pokemon = malloc(sizeof(t_broker_queue));
	g_queue_appeared_pokemon->id_queue = APPEARED_POKEMON;
	g_queue_appeared_pokemon->mensajes_cola = list_create();
	g_queue_appeared_pokemon->suscriptores = list_create();
}

void inicializar_cola_localized_pokemon(void)
{
	g_queue_localized_pokemon = malloc(sizeof(t_broker_queue));
	g_queue_localized_pokemon->id_queue = LOCALIZED_POKEMON;
	g_queue_localized_pokemon->mensajes_cola = list_create();
	g_queue_localized_pokemon->suscriptores = list_create();
}

void inicializar_cola_caught_pokemon(void)
{
	g_queue_caught_pokemon = malloc(sizeof(t_broker_queue));
	g_queue_caught_pokemon->id_queue = CAUGHT_POKEMON;
	g_queue_caught_pokemon->mensajes_cola = list_create();
	g_queue_caught_pokemon->suscriptores =  list_create();
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
	list_add(cola_broker->suscriptores,suscriptor);
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

bool codigo_operacion_valido_broker(op_code cod_oper)
{
	return (cod_oper == NEW_BROKER || cod_oper == GET_BROKER || cod_oper == CATCH_BROKER
			|| cod_oper == APPEARED_BROKER || cod_oper == CAUGHT_BROKER || cod_oper == LOCALIZED_BROKER
			|| cod_oper == SUSCRIBER_ACK  || cod_oper == FIN_SUSCRIPCION );
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
