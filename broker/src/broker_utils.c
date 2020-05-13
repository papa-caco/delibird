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

void iniciar_estructuras_broker(void)
{
	g_msg_counter = 0;
	g_queue_get_pokemon = list_create();
}

void inicio_server_broker(void)
{
	char *ip = g_config_broker->ip_broker;
	char *puerto = g_config_broker->puerto_broker;
	iniciar_server_broker(ip, puerto, g_logger, g_thread_server);
}

void atender_cliente_broker(t_socket_cliente_broker *socket) {
	int cliente_fd = socket->cliente_fd;
	int cod_op = rcv_codigo_operacion(cliente_fd);
	if (cod_op == -1) {
		log_info(g_logger,"(MSG_ERROR!)");
	}
	if (cod_op < SUSCRIP_NEW) {
		atender_publicacion(cod_op, socket);
	} else {
		atender_suscripcion(cod_op, socket);
	}
}

void atender_publicacion(op_code cod_op, t_socket_cliente_broker *socket)
{
	int cliente_fd = socket->cliente_fd;
	t_stream* msg;
	incremento_cnt_id_mensajes();
	switch (cod_op) {
	case GET_BROKER:;
		msg = rcv_msg_get_broker(cliente_fd, g_logger);
		//TODO
		break;
	case NEW_BROKER:;
		msg = rcv_msg_new_broker(cliente_fd, g_logger);
		//TODO
		enviar_msg_confirmed(cliente_fd, g_logger);
		break;
	case CATCH_BROKER:;
		msg = rcv_msg_catch_broker(cliente_fd, g_logger);
		//TODO
		break;
	case APPEARED_BROKER:;
		msg = rcv_msg_appeared_broker(cliente_fd, g_logger);
		//TODO
		break;
	case CAUGHT_BROKER:;
		msg = rcv_msg_caught_broker(cliente_fd, g_logger);
		//TODO
		break;
	case LOCALIZED_BROKER:;
		msg = rcv_msg_localized_broker(cliente_fd, g_logger);
		//TODO
		break;
	}
	if (cod_op != NEW_BROKER) {
		enviar_id_mensaje(cliente_fd, g_logger, g_msg_counter);
	}
	printf("Tamaño del mensaje en Cache: 0x%X = %d Bytes\n",msg->size, msg->size);
	free(msg->data);
	free(msg);
	close(cliente_fd);
	free(socket);
}

void atender_suscripcion(op_code cod_op, t_socket_cliente_broker *socket)
{

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
	g_config_broker->freceuncia_compactacion = config_get_int_value(g_config,"FRECUENCIA_COMPACTACION");
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

void incremento_cnt_id_mensajes(void)
{
	g_msg_counter += 1;
}

void iniciar_log_broker(void) {
	g_logger = log_create(g_config_broker->ruta_log, "BROKER", 1, LOG_LEVEL_INFO);
}
