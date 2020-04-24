/*
 * utils.h
 *
 *  Created on: 21 abr. 2020
 *      Author: utnso
 */

#ifndef UTILS_H_
#define UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<commons/collections/list.h>

// #include "servidor.h"
/**  definicion de estructuras */
typedef struct
{
	int suscriptor[20];
}t_suscriptors;
/*
typedef enum
{
	MENSAJE=1
}op_code;

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
*/

/* ---  DEFINICION DE VARIABLES GLOBALES ---*/

t_log *logger;
t_suscriptors  suscriptores;


/* ---  DEFINICION DE LAS INTERFACES ---*/

//t_config* leer_config(void);
void iniciar_log(void);
t_config* leer_config(void);







#endif /* UTILS_H_ */
