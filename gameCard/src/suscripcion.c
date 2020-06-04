/*
 * subscripcion.c
 *
 *  Created on: 3 jun. 2020
 *      Author: utnso
 */
//#include "utils_gc.h"
#include "suscripcion.h"

/**suscripcion */

void iniciar_suscripcion(void)
{
	log_info(g_logger, "antes inicioINICIANDO SUSCRIPCION");
	t_tipo_mensaje cola_suscripta = APPEARED_POKEMON;
	pthread_t tid;
	t_tipo_mensaje *cola = malloc(sizeof(t_tipo_mensaje));
	memcpy(cola, &cola_suscripta, sizeof(t_tipo_mensaje));

	//sem_wait(&sem_mutex_msjs);
	int tid_status = pthread_create(&tid, NULL,(void*) suscripcion,(void*) cola);
	log_info(g_logger, "por aca %d", tid_status);
	if (tid_status != 0) {
		log_error(g_logger, "Thread create returned %d | %s", tid_status, strerror(tid_status));
	} else {
		log_error(g_logger, "Thread create returned %d | %s", tid_status, strerror(tid_status));
		pthread_detach(tid);
	}
	// return tid_status;
}



void suscripcion(t_tipo_mensaje *cola)
{
	log_info(g_logger, "INICIANDO SUSCRIPCION");
	char *proceso = "BROKER";
	char *name_cola = nombre_cola(*cola);
	int cliente_fd = crear_conexion(g_config_gc->ip_broker, g_config_gc->puerto_broker, g_logger, proceso, name_cola);
	sem_post(&sem_mutex_msjs);
	if (cliente_fd >= 0) {
		status_conn_broker = true;
		t_handsake_suscript *handshake = malloc(sizeof(t_handsake_suscript));
		handshake->id_suscriptor = g_config_gc->id_suscriptor;
		handshake->id_recibido = 0;
		handshake->cola_id = *cola;
		handshake->msjs_recibidos = 0;
		enviar_msj_handshake_suscriptor(cliente_fd, g_logger, handshake);
		op_code cod_oper_mensaje = 0;
		uint32_t contador_msjs = 0;
		int flag_salida =1;
		//En este bucle se queda recibiendo los mensajes que va enviando el BROKER al suscriptor
		while( flag_salida ) {
			//TODO
		}

		free(handshake);
		close(cliente_fd);
	} else {
		status_conn_broker = false;
	}

}


