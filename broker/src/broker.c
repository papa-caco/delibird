/*
 ============================================================================
 Name        : broker.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "utils.h"
#include "servidor.h"

int main(void) {
	puts("!!!Hello Broker!!!"); /* prints !!!Hello Broker!!! */
	iniciar_log();
	iniciar_servidor();
	return EXIT_SUCCESS;

	/*
	 *
	 * Conexión de un proceso al broker.
Llegada de un nuevo mensaje a una cola de mensajes.
Envío de un mensaje a un suscriptor específico.
Confirmación de recepción de un suscripción al envío de un mensaje previo.
Almacenado de un mensaje dentro de la memoria (indicando posición de inicio de su partición).
Eliminado de una partición de memoria (indicado la posición de inicio de la misma).
Ejecución de compactación (para particiones dinámicas) o asociación de bloques (para buddy system). En este último, indicar que particiones se asociaron (indicar posición inicio de ambas particiones).
	 *
	 */
}
