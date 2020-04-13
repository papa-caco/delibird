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

	// Iniciar logger -- Se definio como variable global g_logger
	iniciar_log();

	// Leer config -- Se definio como variable global g_config
	leer_config("/home/utnso/workspace/tp-2020-1c-Los-Que-Aprueban/gameboy/bin/config/gameboy.config");

	// Inicializamos la variable conla estructura para los argumentos ingresados
	t_mensaje_gameboy *msg_gameboy = malloc(sizeof(t_mensaje_gameboy));

	msg_gameboy ->  argumentos = list_create(); // Inicializamos con lista vacia

	// Construimos el mensaje en la estructura definida
	construir_mensaje(msg_gameboy, argumentos_consola);

	// Validar Argumentos para los distintos Procesos y Tipos de Mensaje (segun casuistica)
	if (validar_argumentos(msg_gameboy) != 1) {
		puts("Los Argumentos Ingresados son incorrectos");
		return EXIT_FAILURE;
	}

	// Establecer conexion con PROCESO REMOTO -- obtenemos el Socket Cliente
	int conexion = crear_conexion(msg_gameboy);

	char *proceso = obtengo_proceso(msg_gameboy);
	char *cola = obtengo_cola(msg_gameboy);

	// Loguear conexion con PROCESO REMOTO
	log_info(g_logger,"GAMEBOY_SUCCESSFULL_CONNECTED - SOCKET: %d PROCESS: %s - QUEUE: %s  - REMOTE_IP: %s PORT: %s",
				conexion, proceso, cola, select_ip_proceso(msg_gameboy),select_puerto_proceso(msg_gameboy));

	//TODO Validar conexion.

	// Si falla deberia imprimir error por consola

	//TODO SELECCION MODO GAMEBOY


	// Las 4 lineas de abajo van de prueba para ir viendo resultados
	log_info(g_logger,"IP_BROKER = %s", g_config_gameboy->ip_broker);
	log_info(g_logger,"PUERTO_GAMECARD = %s", g_config_gameboy->puerto_gamecard);
	log_info(g_logger,"Proceso = %d", msg_gameboy -> proceso);
	log_info(g_logger,"Tipo_Mensaje = %d", msg_gameboy -> tipo_mensaje);
	list_mostrar(msg_gameboy -> argumentos);


	// Liberamos memoria reservada para las variables y estructuras
	terminar_programa(msg_gameboy, g_config_gameboy, argumentos_consola, g_logger, g_config, conexion);

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

void terminar_programa(t_mensaje_gameboy *msg_gameboy, t_config_gameboy *config_gameboy, t_list *lista, t_log *log, t_config *config, int conexion)
{
	log_destroy(log);
	config_destroy(config);
	list_destroy(lista);
	list_destroy(msg_gameboy->argumentos);
	free(config_gameboy);
	free(msg_gameboy);
	liberar_conexion(conexion);
}


// CASE Modo GAMEBOY PRODUCTOR

//TODO Empaquetar mensaje

//TODO Serializar Paquete

//TODO Loguear Mensaje a Enviar

//TODO Enviar mensaje a proceso Consumidor

//TODO Imprimir Respuesta del Proceso Consumidor (por log aparte o consola)

//TODO Cerrar conexion proceso consumidor

//TODO Liberar memoria

//TODO Salida

//CASE  Modo GAMEBOY CONSUMIDOR

//TODO Enviar Handshake

/* TODO WHILE TIEMPO <= TIEMPO LIMITE
 *
 * Loguear Mensajes Recibidos
 *
 */
// FIN TIEMPO LIMITE

//TODO Cerrar conexion Proceso Productor

//TODO Liberar memoria

//TODO Salida
