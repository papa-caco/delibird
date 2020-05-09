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
	leer_config_gameboy("/home/utnso/config/gameboy.config");
	// Iniciar logger -- variable global g_logger
	iniciar_log_gameboy();
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
		 conexion = realizar_conexion(msg_gameboy);
		// Logueamos conexion con PROCESO REMOTO: Satisfactoria o Fallida.
		if (conexion < 0) {
			return EXIT_FAILURE;
		}
		// Empaquetamos mensaje
		// Serializamos Paquete
		// Logueamos Mensaje a Enviar
		// Enviamos mensaje a proceso Consumidor
		enviar_mensaje_gameboy(msg_gameboy, conexion);
		// GAMEBOY SIEMPRE ESPERA UNA RTA DEL SERVIDOR
		// Imprimir Respuesta del Proceso Consumidor por log.
		esperar_rta_servidor(conexion);
		close(conexion);
	}
	else {
		//Modo SUSCRIBER - ENVIA el HANDSHAKE y espera Recibir MENSAJES del BROKER
		// Establecemos conexion con BROKER
		// Otenemos el Socket Cliente "conexion"
		tiempo_suscripcion = get_time_suscripcion(msg_gameboy);
		conexion = realizar_conexion(msg_gameboy);
		// Logueamos conexion con PROCESO REMOTO: Satisfactoria o Fallida.
		if (conexion < 0) {
			return EXIT_FAILURE;
		}
		send_msg_gameboy_suscriptor(msg_gameboy, conexion);
		//manejo el final de la suscripcion desde otro hilo
		pthread_create(&ender_suscript, NULL,(void*) end_suscript,&(tiempo_suscripcion));
		pthread_detach(ender_suscript);

		op_code cod_oper_mensaje = 0;
		int flag_salida =1;

		while(flag_salida) {
			cod_oper_mensaje = rcv_codigo_operacion(conexion);
			if (cod_oper_mensaje != SUSCRIP_END) {
				rcv_mensaje_publisher(cod_oper_mensaje, conexion);
				g_rcv_msg_qty += 1;
				enviar_msj_suscripcion_broker(g_suscript_queue, conexion);
			}
			else if (rcv_msg_suscrip_end(conexion) != g_config_gameboy->id_suscriptor) {
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
	finalizar_gameboy(g_config_gameboy, g_logger, g_config);

	return EXIT_SUCCESS;
}

void end_suscript(int *tiempo)
{
	sleep(*tiempo);

	int cliente_fd = conexion_broker();

	if (cliente_fd < 0) {
		exit(EXIT_FAILURE);
	}

	enviar_msj_fin_suscripcion(cliente_fd, g_logger, g_config_gameboy->id_suscriptor, g_suscript_queue);
	if(rcv_codigo_operacion(cliente_fd) != MSG_CONFIRMED) {
		exit(EXIT_FAILURE);
	}
	if (rcv_msg_confirmed(cliente_fd, g_logger) != RESPUESTA_OK) {
		exit(EXIT_FAILURE);
	}

}
