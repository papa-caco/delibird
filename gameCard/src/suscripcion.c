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
	sem_wait(&mutex_msjs_gc);
	int tid_status = pthread_create(&tid, NULL, (void*) suscripcion_gc, (void*) cola);
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
	sem_post(&mutex_msjs_gc);
	if (cliente_fd >= 0) {
		status_conexion_broker = true;
		t_handsake_suscript *handshake = malloc(sizeof(t_handsake_suscript));
		handshake->id_suscriptor = g_config_gc->id_suscriptor;
		handshake->id_recibido = 0;
		handshake->cola_id = *cola;
		handshake->msjs_recibidos = 0;
		enviar_msj_handshake_suscriptor(cliente_fd, g_logger, handshake);
		op_code cod_oper_mensaje = 0;
		uint32_t contador_msjs = 0;
		int flag_salida = 1;
		while (flag_salida) {  //En este bucle se queda recibiendo los mensajes que va enviando el BROKER al suscriptor
			uint32_t id_recibido;
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
			} else if (rcv_msg_suscrip_end(cliente_fd) != g_config_gc->id_suscriptor) {
				pthread_exit((int*) EXIT_FAILURE);
			} else {
				flag_salida = 0;
			}
		}
		int contador_global = contador_msjs_colas_gc(*cola);
		log_debug(g_logger, "(GAMECARD END_SUSCRIPTION: |RECVD_MSGs:%d|TOTAL_MSGS:%d)",
				contador_msjs, contador_global);
		free(handshake);
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
		t_msg_new_gamecard *msg_new = rcv_msj_new_gamecard(socket_cliente, logger);
		g_cnt_msjs_new ++;
		id_recibido = msg_new->id_mensaje;
		// TODO (lanzar nuevo hilo que maneje la llegada del mensaje NEW_POKEMON)
		eliminar_msg_new_gamecard(msg_new);
		break;
	case CATCH_GAMECARD:;
		t_msg_catch_gamecard *msg_catch = rcv_msj_catch_gamecard(socket_cliente, logger);
		g_cnt_msjs_catch ++;
		id_recibido = msg_catch->id_mensaje;
		// TODO (lanzar nuevo hilo que maneje la llegada del mensaje CATCH_POKEMON)
		eliminar_msg_catch_gamecard(msg_catch);
		break;
	case GET_GAMECARD:;
		t_msg_get_gamecard *msg_get = rcv_msj_get_gamecard(socket_cliente, logger);
		g_cnt_msjs_get ++;
		id_recibido = msg_get->id_mensaje;
		eliminar_msg_get_gamecard(msg_get);
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
	pthread_mutex_init(&mutex_reconexion, NULL);
	sem_wait(&mutex_msjs_gc);
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
	log_debug(g_logdebug, "(Intervalo para reconexión con Broker: %d segundos)",
			intervalo);
	sem_post(&mutex_msjs_gc);
	while (1) {
		sleep(intervalo);
		if (!status_conexion_broker) {
			iniciar_suscripciones_broker_gc(g_logdebug);
		}
	}
}

int conexion_broker_gc(t_tipo_mensaje cola, t_log *logger)
{
	int cliente_fd = -1;
	cliente_fd = crear_conexion(g_config_gc->ip_broker, g_config_gc->puerto_broker, logger, "BROKER", nombre_cola(cola));
	return cliente_fd;
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

