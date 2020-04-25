/*
 * utils.c
 *
 *  Created on: 21 abr. 2020
 *      Author: utnso
 */
#include "utils.h"

void iniciar_log(void) {
	//------------ Ver de tener un modo de inicio que indique que imprima por pantalla o no! -------//
	logger = log_create("log/broker.log", "BROKER", 1, 0);

	log_info(logger,"INICIO_LOG_SUCESS");
}
t_config* leer_config(void){
	return config_create("config/broker.config");
}

void finalizar_log(void){
	log_destroy(logger);
}



