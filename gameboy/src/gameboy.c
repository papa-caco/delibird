/*
 *
 * 		gameboy.c
 *
 *  	Created on: 10 apr. 2020
 *
 *      Author:  Grupo "tp-2020-1C Los Que Aprueban"
 *
 *
 */
#include "gameboy.h"

pthread_t ender_suscript;

int main(int argcnt, char *argval[])
{
	int i;
	// Utilizamos una lista para cargar los argumentos ingresados por linea de comando en la consola
	t_list *argumentos_consola = list_create();
	for(i = 1; i < argcnt; i++){
		list_add(argumentos_consola, argval[i]);
	}
	if (argcnt == 1) {
		puts("No se ingresaron Argumentos!!");
		return EXIT_FAILURE;
	}

	// Leer config -- variable global g_config
	leer_config("/home/utnso/config/gameboy.config");
	// Iniciar logger -- variable global g_logger
	iniciar_log();
	// Inicializamos la variable con la estructura para los argumentos ingresados
	t_mensaje_gameboy *msg_gameboy = malloc(sizeof(t_mensaje_gameboy));
	msg_gameboy->argumentos = list_create(); // Inicializamos con lista vacia
	// Construimos el mensaje en la estructura definida
	construir_mensaje(msg_gameboy, argumentos_consola);
	// Validamos los Argumentos para los distintos Procesos y Tipos de Mensaje (segun casuistica)


	if (validar_argumentos(msg_gameboy) != 1) {
		puts("Los Argumentos Ingresados son incorrectos");
		return EXIT_FAILURE;
	}
	int conexion;
	int tiempo_suscripcion = 0;
	g_rcv_msg_qty = 0;
	// DIVIDIMOS GAMEBOY: MODOS PUBLISHER - SUSCRIBER
	if (msg_gameboy -> proceso != SUSCRIPTOR) {
		// Modo PUBLISHER
		// Establecemos conexion con PROCESO REMOTO correspondiente
		// Seleccionamos la direccion IP y el puerto del proceso remoto
		// -- obtenemos el Socket Cliente "conexion"
		 conexion = crear_conexion(msg_gameboy);
		// Logueamos conexion con PROCESO REMOTO: Satisfactoria o Fallida.
		if (conexion < 0) {
			return EXIT_FAILURE;
		}
		// Empaquetamos mensaje
		// Serializamos Paquete
		// Logueamos Mensaje a Enviar
		// Enviamos mensaje a proceso Consumidor
		enviar_mensaje(msg_gameboy, conexion);
		// GAMEBOY SIEMPRE ESPERA UNA RTA DEL SERVIDOR
		// Imprimir Respuesta del Proceso Consumidor por log.
		esperar_respuesta(conexion);
		close(conexion);
	}
	else {
		//Modo SUSCRIBER - ENVIA el HANDSHAKE y espera Recibir MENSAJES del BROKER
		// Establecemos conexion con BROKER
		// Otenemos el Socket Cliente "conexion"
		tiempo_suscripcion = get_time_suscripcion(msg_gameboy);
		conexion = crear_conexion(msg_gameboy);
		// Logueamos conexion con PROCESO REMOTO: Satisfactoria o Fallida.
		if (conexion < 0) {
			return EXIT_FAILURE;
		}
		char *cola = nombre_cola(g_suscript_queue);
		log_info(g_logger, "(SENDING SUSCRIPTION_TO = %s | ID_SUSCRIPTOR = %d )",cola, g_config_gameboy->id_suscriptor);
		enviar_msj_suscriptor(msg_gameboy, conexion);
		//manejo el final de la suscripcion desde otro hilo
		pthread_create(&ender_suscript, NULL,(void*) end_suscript,&(tiempo_suscripcion));
		pthread_detach(ender_suscript);

		op_code cod_oper_mensaje = 0;
		int flag_salida =1;

		while(flag_salida) {
			cod_oper_mensaje = recibir_op_code(conexion);
			if (cod_oper_mensaje != SUSCRIP_END) {
				rcv_msj_publisher(cod_oper_mensaje, conexion);
				g_rcv_msg_qty += 1;
				send_get_next_msg(g_suscript_queue, conexion);
			}
			else if (rcv_suscrip_end(conexion) != g_config_gameboy->id_suscriptor) {
				return EXIT_FAILURE;
			}
			else {
				flag_salida = 0;
			}
		}
		log_info(g_logger,"(END TIME_SUSCRIPTION: %d Sec|RECVD_MSGs: %d)",tiempo_suscripcion, g_rcv_msg_qty);
		close(conexion);
		sleep(1);
		pthread_exit(&ender_suscript);
		}
	terminar_programa(g_config_gameboy, g_logger, g_config);

	return EXIT_SUCCESS;
}

void end_suscript(int *tiempo)
{
	sleep(*tiempo);

	int cliente_fd = conexion_broker();

	if (cliente_fd < 0) {
		exit(EXIT_FAILURE);
	}

	enviar_fin_suscripcion(cliente_fd);
	if(recibir_op_code(cliente_fd) != MSG_CONFIRMED) {
		exit(EXIT_FAILURE);
	}
	if (rcv_msg_confirmed(cliente_fd) != RESPUESTA_OK) {
		exit(EXIT_FAILURE);
	}

}
