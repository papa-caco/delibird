/*
 * conexiones.c
 *
 *  Created on: 3 mar. 2019
 *      Author: utnso
 */
#include "teamInitializer.h"
#include "utilsTeam.h"

void inicio_server_team(void)
{
	char *ip = g_config_team->ip_team;
	char *puerto = g_config_team->puerto_team;
	iniciar_servidor(ip, puerto, g_logger);
}

int enviar_mensaje_get(t_msg_get_broker *msg_get)
{
	char *ip = g_config_team->ip_broker;
	char *puerto = g_config_team->puerto_broker;
	char *proceso = "BROKER";
	char *name_cola = nombre_cola(GET_POKEMON);
	int cliente_fd = crear_conexion(ip, puerto, g_logger, proceso, name_cola);
	enviar_msj_get_broker(cliente_fd, g_logger, msg_get);
	int id_mensaje = -1;
	if ( rcv_codigo_operacion(cliente_fd) != ID_MENSAJE) {
		log_error(g_logger,"RTA Broker Erronea");
	} else {
		id_mensaje = rcv_id_mensaje(cliente_fd, g_logger);
	}
	close(cliente_fd);
	return id_mensaje;
}

int enviar_catch_pokemon_broker(t_msg_catch_broker *msg_catch, t_log *logger)
{
	int thread_status = pthread_create(&tid_send_msjs, NULL,(void*) enviar_mensaje_catch,(void*) msg_catch);
	if (thread_status != 0) {
		log_error(logger, "Thread create returned %d | %s", thread_status, strerror(thread_status));
	} else {
		pthread_detach(tid_send_msjs);
	}
	return thread_status;
}

void enviar_mensaje_catch(t_msg_catch_broker *msg_catch)
{
	char *ip = g_config_team->ip_broker;
	char *puerto = g_config_team->puerto_broker;
	char *proceso = "BROKER";
	char *name_cola = nombre_cola(GET_POKEMON);
	int cliente_fd = crear_conexion(ip, puerto, g_logger, proceso, name_cola);
	enviar_msj_catch_broker(cliente_fd, g_logger, msg_catch);
	int id_mensaje = -1;
	if ( rcv_codigo_operacion(cliente_fd) != ID_MENSAJE) {
		log_error(g_logger,"RTA Broker Erronea");
	} else {
		id_mensaje = rcv_id_mensaje(cliente_fd, g_logger);
	}
	close(cliente_fd);
	//TODO Guardar ID_MENSAJE que envia el BROKER de CATCH_POKEMON
	printf("(RTA_CATCH_POKEMON|ID_MSG:%d)\n",id_mensaje);
	eliminar_msg_catch_broker(msg_catch);
}

void inicio_suscripcion(t_tipo_mensaje *cola)
{
	char *ip = g_config_team->ip_broker;
	char *puerto = g_config_team->puerto_broker;
	char *proceso = "BROKER";
	char *name_cola = nombre_cola(*cola);
	int cliente_fd = crear_conexion(ip, puerto, g_logger, proceso, name_cola);
	if (cliente_fd >= 0) {
		t_handsake_suscript *handshake = malloc(sizeof(t_handsake_suscript));
		handshake->id_suscriptor = g_config_team->id_suscriptor;
		handshake->id_recibido = 0;
		handshake->cola_id = *cola;
		handshake->msjs_recibidos = 0;
		enviar_msj_handshake_suscriptor(cliente_fd, g_logger, handshake);
		op_code cod_oper_mensaje = 0;
		uint32_t contador_msjs = 0;
		int flag_salida =1;
		//En este bucle se queda recibiendo los mensajes que va enviando el BROKER al suscriptor
		while(flag_salida) {
			uint32_t id_recibido;
			cod_oper_mensaje = rcv_codigo_operacion(cliente_fd);
			if (cod_oper_mensaje != SUSCRIP_END) {
				id_recibido = rcv_msjs_broker_publish(cod_oper_mensaje, cliente_fd, g_logger);
				contador_msjs += 1;
				handshake->msjs_recibidos = contador_msjs;
				handshake->id_recibido = id_recibido;
				//Actualizo el ID_RECIBIDO porque en el próximo envío confirmo recepción del mensaje anterior
				enviar_msj_handshake_suscriptor(cliente_fd, g_logger, handshake);
			}
			else if (rcv_msg_suscrip_end(cliente_fd) != g_config_team->id_suscriptor) {
				exit(EXIT_FAILURE);
			} else {
				flag_salida = 0;
			}
		}
		int contador_global = contador_msjs_cola(*cola);
		log_debug(g_logger,"(TEAM END_SUSCRIPTION: |RECVD_MSGs:%d|TOTAL_MSGS:%d)" , contador_msjs, contador_global);
		free(handshake);
		close(cliente_fd);
	}
}

uint32_t rcv_msjs_broker_publish(op_code codigo_operacion, int socket_cliente, t_log *logger)
{
	uint32_t id_recibido;
	void *msg;
	switch(codigo_operacion){
	case COLA_VACIA:;
		id_recibido = rcv_msj_cola_vacia(socket_cliente, logger);
		if (id_recibido != g_config_team->id_suscriptor) {
			log_error(logger,"MSG_ERROR");
		}
		break;
	case APPEARED_TEAM:;
		t_msg_appeared_team *msg_appeared = (t_msg_appeared_team*) msg;
		msg_appeared = rcv_msj_appeared_team(socket_cliente, logger);
		g_cnt_msjs_appeared ++;
		//TODO Agregar la función que corresponda y tome al msg
		id_recibido = msg_appeared->id_mensaje;
		// está de prueba, no tiene una lógica - Igual no está funcionando bien
		/*if (id_recibido > 5) {
				enviar_catch_de_appeared(msg_appeared);
		}*/
		break;
	case CAUGHT_TEAM:;
		t_msg_caught_team *msg_caught = (t_msg_caught_team*) msg;
		msg_caught = rcv_msj_caught_team(socket_cliente, logger);
		g_cnt_msjs_caught ++;
		//TODO Agregar la función que corresponda y tome al msg
		id_recibido = msg_caught->id_mensaje;
		free(msg_caught);
		break;
	case LOCALIZED_TEAM:;
		t_msg_localized_team *msg_localized = (t_msg_localized_team*) msg;
		msg_localized = rcv_msj_localized_team(socket_cliente, logger);
		g_cnt_msjs_localized ++;
		//TODO Agregar la función que corresponda y tome al msg
		id_recibido = msg_localized->id_mensaje;
		eliminar_msg_localized_team(msg_localized);
		break;
	case 0:
		pthread_exit(NULL);
	case -1:
		pthread_exit(NULL);
	}
	return id_recibido;
}

void atender_gameboy(int *cliente_fd) {
	op_code cod_op;
	if (recv(*cliente_fd, &cod_op, sizeof(op_code), MSG_WAITALL) == -1) {
		cod_op = -1;
	}
	if (cod_op == APPEARED_TEAM) {
		process_msjs_gameboy(cod_op, *cliente_fd, g_logger);
	} else {
		log_error(g_logger,"RECIBI MSJ EQUIVOCADO DEL GAMEBOY");
	}
}

// RECIBE MENSAJES QUE ENVIA el GAMEBOY: APPEARED_POKEMON
void process_msjs_gameboy(op_code cod_op, int  cliente_fd, t_log *logger) {
	void* msg;
	switch (cod_op) {
	case APPEARED_TEAM:;
		t_msg_appeared_team *msg_appeared = (t_msg_appeared_team*) msg;
		msg_appeared = rcv_msj_appeared_team(cliente_fd, logger);
		g_cnt_msjs_appeared ++;
		//TODO Hacer lo que corresponda con msg_appeared
		enviar_msg_confirmed(cliente_fd,logger);
		eliminar_msg_appeared_team(msg_appeared);
		break;
	}
}

//Función de prueba - Envía un mensaje CATCH_POKEMON para probar la función.
void enviar_catch_de_appeared(t_msg_appeared_team *msg_appeared)
{
	t_msg_catch_broker *msg_catch = malloc(sizeof(t_msg_catch_broker));
	msg_catch->coordenada = malloc(sizeof(t_coordenada));
	msg_catch->coordenada->pos_x = msg_appeared->coord->pos_x;
	msg_catch->coordenada->pos_y = msg_appeared->coord->pos_y;
	msg_catch->size_pokemon = msg_appeared->size_pokemon;
	msg_catch->pokemon = malloc(sizeof(msg_catch->size_pokemon));
	memcpy(msg_catch->pokemon, msg_appeared->pokemon, msg_catch->size_pokemon);
	enviar_catch_pokemon_broker(msg_catch, g_logger);
}

void iniciar_cnt_msjs(void)
{
	g_cnt_msjs_appeared = 0;
	g_cnt_msjs_caught = 0;
	g_cnt_msjs_localized = 0;
}

void iniciar_log_team(void) {
	g_logger = log_create(g_config_team->ruta_log, "SERVER", 1,
			LOG_LEVEL_INFO);
}

void leer_config_team(char *path) {
	g_config = config_create(path);
	g_config_team = malloc(sizeof(t_config_team));
	g_config_team->ip_broker = config_get_string_value(g_config, "IP_BROKER");
	g_config_team->puerto_broker = config_get_string_value(g_config, "PUERTO_BROKER");
	g_config_team->ip_team = config_get_string_value(g_config, "IP_TEAM");
	g_config_team->puerto_team = config_get_string_value(g_config, "PUERTO_TEAM");
	g_config_team->tiempo_reconexion = config_get_int_value(g_config, "TIEMPO_RECONEXION");
	g_config_team->retardo_ciclo_cpu = config_get_int_value(g_config, "RETARDO_CICLO_CPU");
	g_config_team->algoritmo_planificion = config_get_string_value(g_config, "ALGORITMO_PLANIFICACION");
	g_config_team->quantum = config_get_int_value(g_config, "QUANTUM");
	g_config_team->estimacion_inicial = config_get_int_value(g_config, "ESTIMACION_INICIAL");
	g_config_team->ruta_log = config_get_string_value(g_config, "RUTA_LOG");
	g_config_team->id_suscriptor = config_get_int_value(g_config, "ID_SUSCRIPTOR");
}


void liberar_lista_posiciones(t_list* lista){
	for(int i = 0; i< list_size(lista); i++){
		free(list_get(lista,i));
	}

	list_destroy(lista);
}

void liberar_listas(char** lista) {

	int contador = 0;
	while (lista[contador] != NULL) {
		free(lista[contador]);
		contador++;
	}

	free(lista);
}

int contador_msjs_cola(t_tipo_mensaje cola_suscripcion)
{
	int contador_global;
	switch(cola_suscripcion) {
		case APPEARED_POKEMON:;
			contador_global = g_cnt_msjs_appeared;
			break;
		case CAUGHT_POKEMON:;
			contador_global = g_cnt_msjs_caught;
			break;
		case LOCALIZED_POKEMON:;
			contador_global = g_cnt_msjs_localized;
			break;
	}
	return contador_global;
}
