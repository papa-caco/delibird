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
	pthread_t thread;
	t_tipo_mensaje *cola = malloc(sizeof(t_tipo_mensaje));
		t_tipo_mensaje cola_suscripta = NEW_POKEMON;
		memcpy(cola, &cola_suscripta, sizeof(t_tipo_mensaje));
	//	sem_wait(&sem_mutex_msjs);
	  int createerror = pthread_create(&thread, NULL, (void*) suscripcion,(void*) cola);
	  if (!createerror) /*check whether the thread creation was successful*/
	    {
		  printf("TODO BIEN \n");
	      pthread_join(thread, NULL); /*wait until the created thread terminates*/

	    }
	  printf("error %s \n", strerror(createerror));

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
			//En este punto estamos suscriptos al broker
			//Y escuchando los mensajes que nos pueda enviar.
			//TODO
				//Empezar a procesar los mensajes a los que nos hemos suscripto.
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



