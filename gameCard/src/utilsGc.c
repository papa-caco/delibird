/*
 * utils.c
 *
 *  Created on: 20 abr. 2020
 *      Author: utnso
 */
#include "utilsGc.h"

void iniciar_log(void) {
	//------------ Ver de tener un modo de inicio que indique que imprima por pantalla o no! -------//
	logger = log_create("log/gameCard.log", "GAME_CARD", 1, LOG_LEVEL_INFO);
	log_info(logger,"INICIO_LOG_SUCESS");
}
t_config* leer_config(void){
	return config_create("config/gameCard.config");
}

void finalizar_log(void){
	log_destroy(logger);
}
void destruir_config(void){
	//config_destroy(config);
}

/**
 * Se suscribe a una cola de mensaje

void suscribirse(int conexion){
	enviar_mensaje("Mensaje",conexion);
}
*/




