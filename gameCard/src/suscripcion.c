/*
 * subscripcion.c
 *
 *  Created on: 3 jun. 2020
 *      Author: utnso
 */
//#include "utils_gc.h"
#include "suscripcion.h"

/**suscripcion */
/**
 * El gameCard se suscribe a todas sus colas de mensajes en el Broker
 */
void iniciar_suscripcion(void)
{
	int i;
	t_tipo_mensaje colas_de_suscripcion[3] = {NEW_POKEMON,GET_POKEMON,CATCH_POKEMON};
	pthread_t id_pthread[3];
	pthread_attr_t attr;
	t_tipo_mensaje *cola = malloc(sizeof(t_tipo_mensaje));
	//t_tipo_mensaje cola_suscripta;
//	pthread_mutex_init(&sem_mutex_suscripcion, NULL);
//	pthread_attr_init(&attr);
//    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	for ( i =0; i< 3 ; i++ ){

		//cola_suscripta = colas_de_suscripcion[i] ;
		//memcpy(cola, &cola_suscripta, sizeof(t_tipo_mensaje));
		sem_wait(&sem_mutex_suscripcion);
		memcpy(cola, &(colas_de_suscripcion[i]), sizeof(t_tipo_mensaje));
		int status_thread = pthread_create(&id_pthread[i], NULL, (void*) suscripcion,(void*) cola);

		if (status_thread ){
			log_error(g_logger, "NO SE PUEDE ESTABLECER SUSCRIPCION %s %s",colas_de_suscripcion[i],strerror(status_thread) );
		}
	}
	for ( i =0; i< 3 ; i++ ){
		pthread_join(id_pthread[i], NULL);
	}
}

int conexion_broker(t_tipo_mensaje cola, t_log *logger)
{
	int cliente_fd = -1;
	cliente_fd = crear_conexion(g_config_gc->ip_broker, g_config_gc->puerto_broker, logger, "BROKER", nombre_cola(cola));
	return cliente_fd;
}


void suscripcion(t_tipo_mensaje cola, t_log *logger)
{
	int cliente_fd = conexion_broker(cola, logger);
	if (cliente_fd >= 0) {
		status_conn_broker = true;
		t_handsake_suscript *handshake = malloc(sizeof(t_handsake_suscript));
		handshake->id_suscriptor = g_config_gc->id_suscriptor;
		handshake->id_recibido = 0;
		handshake->cola_id = cola;
		handshake->msjs_recibidos = 0;
		enviar_msj_handshake_suscriptor(cliente_fd, g_logger, handshake);
		op_code cod_oper_mensaje = 0;
		uint32_t contador_msjs = 0;
		sem_post(&sem_mutex_suscripcion);
		int flag_salida =1;
	//	pthread_mutex_lock (&sem_mutex_suscripcion);
		//En este bucle se queda recibiendo los mensajes que va enviando el BROKER al suscriptor
		while( flag_salida ) {
			uint32_t id_recibido;
			cod_oper_mensaje = rcv_codigo_operacion(cliente_fd);
			log_info(logger, "RECIBI MENSAJE DE SUSCRIPCION COLA %s | COD_OPER %d", nombre_cola(cola),cod_oper_mensaje);
			//TODO
			//Procesar los mensajes recibidos
			//No se por que la ultima suscripcion se queda esperando!!!
			enviar_msj_handshake_suscriptor(cliente_fd, g_logger, handshake);
			process_request(cod_oper_mensaje,cliente_fd);
		//	  pthread_mutex_unlock (&sem_mutex_suscripcion);
		//	  pthread_exit(NULL);
			//sem_post(&sem_mutex_suscripcion);
		  //	sem_wait(&sem_mutex_suscripcion);

		//	flag_salida = 0;

		}

		free(handshake);
		close(cliente_fd);
	} else {
		status_conn_broker = false;
	}

}

void *threadfunction(void *parametro)
{
  printf("dendro del hijo hijo Hello, World !\n"); /*printf() is specified as thread-safe as of C11*/
  return 0;
}



