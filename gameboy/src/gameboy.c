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

int main(int argcnt, char* argval[])
{
	int i;
	// Leer argumentos cargados desde linea de comando por consola
	t_list *argumentos_consola	= list_create();

	for(i = 1; i < argcnt; i++){

		list_add(argumentos_consola, argval[i]);
	}

	/*list_add(argumentos_consola,"SUSCRIPTOR");
	list_add(argumentos_consola,"GET_POKEMON");
	list_add(argumentos_consola,"20");*/

	// Iniciar logger
	iniciar_log();

	// Leer config
	leer_config("/home/utnso/workspace/tp-2020-1c-Los-Que-Aprueban/gameboy/src/config/gameboy.config");

	t_args_msg_gameboy *argumentos_mensaje = malloc(sizeof(t_args_msg_gameboy));
	argumentos_mensaje ->  argumentos = list_create();

	// Cargar Argumentos en estructura
	cargar_argumentos(argumentos_mensaje, argumentos_consola);

	char *ip = config_get_string_value(g_config,"IP_BROKER");

	log_info(g_logger,"IP_BROKER = %s",ip);
	log_info(g_logger,"Proceso = %d", argumentos_mensaje -> proceso);
	log_info(g_logger,"Tipo_Mensaje = %d", argumentos_mensaje -> tipo_mensaje);
	list_mostrar(argumentos_mensaje -> argumentos);


	terminar_programa(argumentos_mensaje, argumentos_consola, g_logger, g_config);
	//log_destroy(g_logger);
	//config_destroy(g_config);
	//list_destroy(argumentos_consola);

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

void terminar_programa(t_args_msg_gameboy *argumentos, t_list *lista, t_log *log, t_config *config)
{
	log_destroy(log);
	config_destroy(config);
	list_destroy(lista);
	list_destroy(argumentos->argumentos);
	free(argumentos);
	//liberar_conexion(conexion);
}


//TODO validar y seleccionar argumento tipo_mensaje

//TODO Validar Argumentos restantes (segun casuistica)

/* Salida por Validar Argumentos = Invalidos
 *
 * Imprimer error en Consola
 *
 * Libera memoria
 *
 * Finaliza main()
 *
*/
// Continuacion por Validar Argumentos = Ok.

//TODO Establecer conexion con PROCESO REMOTO

//TODO Validar conexion.

// Si falla deberia imprimir error por consola

//TODO Loguear conexion

//TODO SELECCION MODO GAMEBOY

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
