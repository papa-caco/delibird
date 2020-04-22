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

int main(int argcnt, char *argval[])
{
	int i;
	// Utilizamos una lista para cargar los argumentos ingresados por linea de comando en la consola
	t_list *argumentos_consola	= list_create();
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

	msg_gameboy ->  argumentos = list_create(); // Inicializamos con lista vacia
	// Construimos el mensaje en la estructura definida
	construir_mensaje(msg_gameboy, argumentos_consola);
	// Validamos los Argumentos para los distintos Procesos y Tipos de Mensaje (segun casuistica)
	if (validar_argumentos(msg_gameboy) != 1) {
		puts("Los Argumentos Ingresados son incorrectos");
		return EXIT_FAILURE;
	}

	int conexion;

	// DIVIDIMOS GAMEBOY: MODOS PUBLISHER - SUSCRIBER
	if (msg_gameboy -> proceso != SUSCRIPTOR) {
		// Establecemos conexion con PROCESO REMOTO correspondiente
		// Seleccionamos la direccion IP y el puerto del proceso remoto
		// -- obtenemos el Socket Cliente "conexion"
		 conexion = crear_conexion(msg_gameboy);
		// Logueamos conexion con PROCESO REMOTO: Satisfactoria o Fallida.
		if (conexion < 0) {
			return EXIT_FAILURE;
		}
		// Modo PUBLISHER - SOLO ENVIA
		// Empaquetamos mensaje
		// Serializamos Paquete
		// Logueamos Mensaje a Enviar
		// Enviamos mensaje a proceso Consumidor
		enviar_mensaje(msg_gameboy, conexion);
		// GAMEBOY SIEMPRE ESPERA UNA RTA DEL SERVIDOR
		// Imprimir Respuesta del Proceso Consumidor por log.
		esperar_respuesta(conexion);
	}
	else {
		//TODO Modo SUSCRIBER - ENVIA el HANDSHAKE y espera Recibir MENSAJES del BROKER

		// Establecemos conexion con PROCESO REMOTO correspondiente
		// Seleccionamos la direccion IP y el puerto del proceso remoto
		// -- obtenemos el Socket Cliente "conexion"
		 /*conexion = crear_conexion(msg_gameboy);
		// Logueamos conexion con PROCESO REMOTO: Satisfactoria o Fallida.
		if (conexion < 0) {
			return EXIT_FAILURE;
		}*/

		//TODO enviar_msj_suscriptor(msg_gameboy, conexion);
		int tiempo_suscripcion = get_time_suscripcion(msg_gameboy);

		//TODO recibir_msjs_suscripcion(tiempo_suscripcion,conexion);

		// Las 3 lineas de abajo van de prueba para ir viendo resultados
		log_info(g_logger,"Proceso = %d", msg_gameboy -> proceso);
		log_info(g_logger,"Tipo_Mensaje = %d", msg_gameboy -> tipo_mensaje);
		log_info(g_logger,"tiempo_suscripcion %d", tiempo_suscripcion); //TODO Borrar linea
	}

	terminar_programa(msg_gameboy, g_config_gameboy, argumentos_consola, g_logger, g_config, conexion);
	// Salida
	return EXIT_SUCCESS;
}

void list_mostrar(t_list* lista)
{
	int i = 0;
	char* elemento;
	for(i=0;i<lista->elements_count;i++){
		elemento = list_get(lista,i);
		puts(elemento);
	}
}

//TODO  Modo GAMEBOY SUSCRIBER

//TODO Enviar Handshake

/* TODO WHILE TIEMPO < = TIEMPO LIMITE
 *
 * Loguear Mensajes Recibidos
 *
 */

// FIN TIEMPO LIMITE

