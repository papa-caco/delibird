/*
 * utils.h
 *
 *  Created on: 20 abr. 2020
 *      Author: utnso
 */
#ifndef UTILS_H_
#define UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<string.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<commons/collections/list.h>
#include<readline/readline.h>

#include<unistd.h>


/*** ESTRUCTURAS **/

typedef enum
{
	SUSCRIBE,
	MENSAJE,
} op_code; //codigo de operacion

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;


typedef enum {
	UNKNOWN_QUEUE = 9,
	APPEARED_POKEMON,
	LOCALIZED_POKEMON,
	CAUGHT_POKEMON,
} t_tipo_mensaje;


/* ---  DEFINICION DE VARIABLES GLOBALES ---*/
t_log *logger;


/* ---  DEFINICION DE LAS INTERFACES ---*/
t_config* leer_config(void);
void iniciar_log(void);
void finalizar_log(void);
void destruir_config(void);

//void suscribirse(int conexion);
//t_config* config = leer_config();
//


#endif /* UTILS_H_ */
