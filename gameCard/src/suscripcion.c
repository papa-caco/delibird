/*
 * subscripcion.c
 *
 *  Created on: 3 jun. 2020
 *      Author: utnso
 */
//#include "utils_gc.h"
#include "suscripcion.h"

void iniciar_suscripciones_broker_gc(t_log *logger)
{
	iniciar_suscripcion_cola_gc(NEW_POKEMON, logger);
	iniciar_suscripcion_cola_gc(GET_POKEMON, logger);
	iniciar_suscripcion_cola_gc(CATCH_POKEMON, logger);
}

int iniciar_suscripcion_cola_gc(t_tipo_mensaje cola_suscripta, t_log *logger)
{
	pthread_t tid;
	t_tipo_mensaje *cola = malloc(sizeof(t_tipo_mensaje));
	memcpy(cola, &cola_suscripta, sizeof(t_tipo_mensaje));
	pthread_mutex_lock(&g_mutex_envio);
	int tid_status = pthread_create(&tid, NULL, (void*) suscripcion_gc, (t_tipo_mensaje*) cola);
	if (tid_status != 0) {
		log_error(logger, "Thread create returned %d | %s", tid_status,
				strerror(tid_status));
	} else {
		pthread_detach(tid);
	}
	return tid_status;
}

void suscripcion_gc(t_tipo_mensaje *cola)
{
	char *ip = g_config_gc->ip_broker;
	char *puerto = g_config_gc->puerto_broker;
	char *proceso = "BROKER";
	char *name_cola = nombre_cola(*cola);
	int cliente_fd = crear_conexion(ip, puerto, g_logger, proceso, name_cola);
	pthread_mutex_unlock(&g_mutex_envio);
	if (cliente_fd >= 0) {
		status_conexion_broker = true;
		t_handsake_suscript *handshake = malloc(sizeof(t_handsake_suscript));
		handshake->id_suscriptor = g_config_gc->id_suscriptor;
		handshake->id_recibido = 0;
		handshake->cola_id = *cola;
		handshake->msjs_recibidos = 0;
		enviar_msj_handshake_suscriptor(cliente_fd, g_logger, handshake);
		puts("");
		op_code cod_oper_mensaje = 0;
		uint32_t contador_msjs = 0;
		int flag_salida = 1;
		while (flag_salida) {  //En este bucle se queda recibiendo los mensajes que va enviando el BROKER al suscriptor
			uint32_t id_recibido;
			//pthread_mutex_lock(&g_mutex_recepcion);
			cod_oper_mensaje = rcv_codigo_operacion(cliente_fd);
			if (!es_cod_oper_mensaje_gamecard(cod_oper_mensaje)) { // Posible desconexión del BROKER
				log_error(g_logger, "(Se perdió Conexión con BROKER|%s)", nombre_cola(handshake->cola_id));
				status_conexion_broker = false;
				flag_salida = 0;
			} else if (cod_oper_mensaje != SUSCRIP_END) {
				id_recibido = rcv_msjs_broker_gc(cod_oper_mensaje,cliente_fd, g_logger);
				contador_msjs += 1;
				handshake->msjs_recibidos = contador_msjs;
				handshake->id_recibido = id_recibido;
				//Actualizo el ID_RECIBIDO porque en el próximo envío confirmo recepción del mensaje anterior
				enviar_msj_handshake_suscriptor(cliente_fd, g_logger, handshake);
				puts("");
				//pthread_mutex_unlock(&g_mutex_recepcion);
			} else if (rcv_msg_suscrip_end(cliente_fd) != g_config_gc->id_suscriptor) {
				pthread_exit((int*) EXIT_FAILURE);
			} else {
				flag_salida = 0;
			}
		}
		int contador_global = contador_msjs_colas_gc(handshake->cola_id);
		log_debug(g_logger, "(GAMECARD END_SUSCRIPTION: |RECVD_MSGs:%d|TOTAL_MSGS:%d)",contador_msjs, contador_global);
		free(cola);
		free(handshake);
		status_conexion_broker = false;
		close(cliente_fd);
	} else {
		status_conexion_broker = false;
	}
}

uint32_t rcv_msjs_broker_gc(op_code codigo_operacion, int socket_cliente, t_log *logger)
{
	uint32_t id_recibido;
	switch (codigo_operacion) {
	case COLA_VACIA: ;
		id_recibido = rcv_msj_cola_vacia(socket_cliente, logger);
		if (id_recibido != g_config_gc->id_suscriptor) {
			log_error(logger, "MSG_ERROR");
		}
		break;
	case NEW_GAMECARD:;
		puts("");
		t_msg_new_gamecard *msg_new = rcv_msj_new_gamecard(socket_cliente, logger);
		g_cnt_msjs_new ++;
		id_recibido = msg_new->id_mensaje;
		recibir_msg_new_pokemon(msg_new, logger);
		break;
	case CATCH_GAMECARD:;
		puts("");
		t_msg_catch_gamecard *msg_catch = rcv_msj_catch_gamecard(socket_cliente, logger);
		g_cnt_msjs_catch ++;
		id_recibido = msg_catch->id_mensaje;
		recibir_msg_catch_pokemon(msg_catch, logger);
		break;
	case GET_GAMECARD:;
		puts("");
		t_msg_get_gamecard *msg_get = rcv_msj_get_gamecard(socket_cliente, logger);
		g_cnt_msjs_get ++;
		id_recibido = msg_get->id_mensaje;
		recibir_msg_get_pokemon(msg_get, logger);
		break;
	case 0:
		pthread_exit(NULL);
	case -1:
		pthread_exit(NULL);
	}
	return id_recibido;
}

void lanzar_reconexion_broker_gc(t_log *logger)
{
	sleep(1);
	pthread_mutex_init(&mutex_reconexion, NULL);
	pthread_mutex_lock(&g_mutex_envio);
	int arg = 0;
	int thread_status = pthread_create(&tid_reconexion_gc, NULL, (void*) funciones_reconexion_gc, NULL);
	if (thread_status != 0) {
		log_error(logger, "Error al crear el thread para reconexión con BROKER");
		exit(EXIT_FAILURE);
	} else {
		pthread_detach(tid_reconexion_gc);
	}
}

void funciones_reconexion_gc(void)
{
	int intervalo = g_config_gc->tiempo_reconexion;
	log_debug(g_logdebug, "(Intervalo para reconexión con Broker: %d segundos)\n", intervalo);
	pthread_mutex_unlock(&g_mutex_envio);
	while (1) {
		sleep(intervalo);
		if (!status_conexion_broker) {
			iniciar_suscripciones_broker_gc(g_logdebug);
		}
	}
}

int enviar_appeared_pokemon_broker(t_msg_appeared_broker *msg_appeared, t_log *logger)
{	// ------ USAR ESTA FUNCION PARA ENVIAR MENSAJES APPEARED_POKEMON AL BROKER ----------//
	pthread_mutex_lock(&g_mutex_recepcion);
	int id_mensaje = -1;
	char *ip = g_config_gc->ip_broker;
	char *puerto = g_config_gc->puerto_broker;
	char *proceso = "BROKER";
	char *name_cola = nombre_cola(APPEARED_POKEMON);
	int cliente_fd = crear_conexion(ip, puerto, g_logger, proceso, name_cola);
	if (cliente_fd >= 0) {
		enviar_msj_appeared_broker(cliente_fd, g_logger, msg_appeared);
		op_code codigo_operacion = rcv_codigo_operacion(cliente_fd);
		if (codigo_operacion == ID_MENSAJE) {
			id_mensaje = rcv_id_mensaje(cliente_fd, g_logger);
			puts("");
		} else if (codigo_operacion == MSG_ERROR) {
			void *a_recibir = recibir_buffer(cliente_fd, &id_mensaje);
			log_warning(g_logger, "(RECEIVING: |%s)", a_recibir);
			free(a_recibir);
		}
	}
	pthread_mutex_unlock(&g_mutex_recepcion);
	eliminar_msg_appeared_broker(msg_appeared);
	close(cliente_fd);
	return id_mensaje;
}

int enviar_localized_pokemon_broker(uint32_t id_correlativo, char *pokemon, t_list *posiciones, t_log *logger)
{	// ------ USAR ESTA FUNCION PARA ENVIAR MENSAJES LOCALIZED_POKEMON AL BROKER ----------//
	t_msg_localized_broker *msg_localized = malloc(sizeof(t_msg_localized_broker));
	msg_localized->id_correlativo = id_correlativo;
	msg_localized->posiciones = malloc(sizeof(t_posiciones_localized));
	msg_localized->posiciones->cant_posic = posiciones->elements_count;
	msg_localized->posiciones->coordenadas = list_create();
	list_add_all(msg_localized->posiciones->coordenadas, posiciones);
	list_destroy(posiciones);
	msg_localized->size_pokemon = strlen(pokemon) + 1;
	msg_localized->pokemon = malloc(sizeof(msg_localized->size_pokemon));
	memcpy(msg_localized->pokemon, pokemon, msg_localized->size_pokemon);
	free(pokemon);//TODO
	pthread_mutex_lock(&g_mutex_recepcion);
	int id_mensaje;
	char *ip = g_config_gc->ip_broker;
	char *puerto = g_config_gc->puerto_broker;
	char *proceso = "BROKER";
	char *name_cola = nombre_cola(LOCALIZED_POKEMON);
	int *cliente_fd = crear_conexion_broker(ip, puerto, g_logger, proceso, name_cola);
	if (*cliente_fd >= 0) {
		enviar_msj_localized_broker(*cliente_fd, g_logger, msg_localized);
		op_code codigo_operacion = rcv_codigo_operacion(*cliente_fd);
		if (codigo_operacion == ID_MENSAJE) {
			id_mensaje = rcv_id_mensaje(*cliente_fd, g_logger);
			puts("");
		} else if (codigo_operacion == MSG_ERROR) {
			void *a_recibir = recibir_buffer(*cliente_fd, &id_mensaje);
			log_warning(g_logger, "(RECEIVING: |%s)", a_recibir);
			free(a_recibir);
		}
	}
	pthread_mutex_unlock(&g_mutex_recepcion);
	close(*cliente_fd);
	free(cliente_fd);
	eliminar_msg_localized_broker(msg_localized);
	return id_mensaje;
}

int enviar_caught_pokemon_broker(uint32_t id_correlativo, t_result_caught resultado, t_log *logger)
{	// ------ USAR ESTA FUNCION PARA ENVIAR MENSAJES CAUGHT_POKEMON AL BROKER ----------//
	t_msg_caught_broker *msg_caught = malloc(sizeof(t_msg_caught_broker));
	msg_caught->id_correlativo = id_correlativo;
	msg_caught->resultado = resultado;
	pthread_mutex_lock(&g_mutex_recepcion);
	int id_mensaje = -1, cliente_fd = 0;
	char *ip = g_config_gc->ip_broker;
	char *puerto = g_config_gc->puerto_broker;
	char *proceso = "BROKER";
	char *name_cola = nombre_cola(CAUGHT_POKEMON);
	cliente_fd = crear_conexion(ip, puerto, g_logger, proceso, name_cola);
	if (cliente_fd >= 0) {
		enviar_msj_caught_broker(cliente_fd, g_logger, msg_caught);
		op_code codigo_operacion = rcv_codigo_operacion(cliente_fd);
		if (codigo_operacion == ID_MENSAJE) {
			id_mensaje = rcv_id_mensaje(cliente_fd, g_logger);
			puts("");
		} else if (codigo_operacion == MSG_ERROR) {
			void *a_recibir = recibir_buffer(cliente_fd, &id_mensaje);
			log_warning(g_logger, "(RECEIVING: |%s)", a_recibir);
			free(a_recibir);
		}
	}
	pthread_mutex_unlock(&g_mutex_recepcion);
	free(msg_caught);
	close(cliente_fd);
	return id_mensaje;
}

int enviar_end_suscripcion_broker(t_tipo_mensaje cola_id, int contador_msgs, t_log *logger)
{	// ------ USAR ESTA FUNCION PARA ENVIAR MENSAJES FIN_SUSCRIPCION AL BROKER ----------//
	pthread_t tid;
	t_handsake_suscript *handshake = malloc(sizeof(t_handsake_suscript));
	handshake->id_suscriptor =  g_config_gc->id_suscriptor;
	handshake->msjs_recibidos = contador_msgs;
	handshake->id_recibido = handshake->msjs_recibidos;
	handshake->cola_id = cola_id;
	pthread_mutex_lock(&g_mutex_envio);
	int thread_status = pthread_create(&tid, NULL,(void*) connect_broker_y_enviar_end_suscript, (void*) handshake);
	if (thread_status != 0) {
		log_error(logger, "Thread create returned %d | %s", thread_status, strerror(thread_status));
	} else {
		pthread_detach(tid);
	}
	return thread_status;
}

void connect_broker_y_enviar_end_suscript(t_handsake_suscript *handshake)
{
	int id_mensaje = -1;
	char *ip = g_config_gc->ip_broker;
	char *puerto = g_config_gc->puerto_broker;
	char *proceso = "BROKER";
	char *name_cola = nombre_cola(handshake->cola_id);
	int cliente_fd = crear_conexion(ip, puerto, g_logger, proceso, name_cola);
	if (cliente_fd >= 0) {
		enviar_solicitud_fin_suscripcion(cliente_fd, g_logger, handshake);
		op_code codigo_operacion = rcv_codigo_operacion(cliente_fd);
		if (codigo_operacion == MSG_CONFIRMED) {
			rcv_msg_confirmed(cliente_fd, g_logger);
		} else if (codigo_operacion == MSG_ERROR) {
			void *a_recibir = recibir_buffer(cliente_fd, &id_mensaje);
			log_warning(g_logger, "(RECEIVING: |%s)", a_recibir);
			free(a_recibir);
		}
	}
	pthread_mutex_unlock(&g_mutex_envio);
	close(cliente_fd);
	free(handshake);
	//pthread_exit(NULL);
}

int conexion_broker_gc(t_tipo_mensaje cola, t_log *logger)
{
	int cliente_fd = -1;
	cliente_fd = crear_conexion(g_config_gc->ip_broker, g_config_gc->puerto_broker, logger, "BROKER", nombre_cola(cola));
	return cliente_fd;
}

void manejo_senial_externa_gc(void)
{
	pthread_mutex_lock(&g_mutex_envio);
	int broker_pid = process_get_thread_id();
	log_trace(g_logger,"Para finalizar Suscripción -->> Enviar Señales ''kill SIGUSR1/2 '' al proceso (PID):%d", broker_pid);
	puts("");
	signal(SIGUSR1, funcion_captura_senial_gc);
	signal(SIGUSR2, funcion_captura_senial_gc);
	pthread_mutex_unlock(&g_mutex_envio);
}

void funcion_captura_senial_gc(int senial)
{
	int gamecard_pid = process_get_thread_id();
	log_warning(g_logger,"Señal recibida: %s -->>(kill -%d  %d).",senial_recibida_gc(senial), senial, gamecard_pid);
	switch(senial) {
	case SIGUSR1:;
		finalizar_suscripciones(senial);
		break;
	case SIGUSR2:;
		finalizar_suscripciones(senial);
		break;
	default:
		puts("-->>No es posible manejar señal enviada.<<--");
		break;
	}
}

void finalizar_suscripciones(int senial)
{
	if (senial == SIGUSR1 || senial == SIGUSR2) {
		enviar_end_suscripcion_broker(NEW_POKEMON, g_cnt_msjs_new, g_logger);
		enviar_end_suscripcion_broker(GET_POKEMON, g_cnt_msjs_get, g_logger);
		enviar_end_suscripcion_broker(CATCH_POKEMON, g_cnt_msjs_catch, g_logger);
	}
}

char *senial_recibida_gc(int senial)
{
	char *recvd_signal;
	switch(senial) {
	case SIGUSR1:;
		recvd_signal = "SIGUSR1";
		break;
	case SIGUSR2:;
		recvd_signal = "SIGUSR2";
		break;
	}
	return recvd_signal;
}

int contador_msjs_colas_gc(t_tipo_mensaje cola_suscripcion)
{
	int contador_global;
	switch (cola_suscripcion) {
	case NEW_POKEMON:;
		contador_global = g_cnt_msjs_new;
		break;
	case CATCH_POKEMON:;
		contador_global = g_cnt_msjs_catch;
		break;
	case GET_POKEMON:;
		contador_global = g_cnt_msjs_get;
		break;
	}
	return contador_global;
}

void iniciar_cnt_msjs_gc(void)
{
	g_cnt_msjs_get = 0;
	g_cnt_msjs_catch = 0;
	g_cnt_msjs_new = 0;
}
