/*
 * planificadorAMedianoPlazo.c
 *
 *  Created on: 7 jul. 2020
 *      Author: utnso
 */
#include "planificadorAMedianoPlazo.h"

void planificadorMedianoPlazo(){
	while(1){

		//AGREGAR EL SIGNAL AL RECIBIR EL POKEMON A UBICAR EN EL MAPA
		//VERIFICAR EL MAPA DE POKEMONES LIBRES, SI HAY COMIENZA A EJECUTAR
		sem_wait(&sem_planificador_mplazo);

		//RECORRER TODA LA COLA DE BLOCKED, VERIFICANDO LOS QUE ESTEN EN ESTADO ACABO_INTERCAMBIAR,
		//QUE NOS QUEDARON COLGADOS DEL INTERCAMBIO, Y FIJARSE SI DEBEN PASAR A ESTADO EXIT Y A LA COLA EXIT.

		//PREGUNTAR SI TODOS LOS ENTRENADORES ESTAN EN EXIT, ENTONCES:
		//1.AVISAR AL PLANIFICADOR DEL CORTO PLAZO
		//2. MATARME A MI MISMO

		//NO OLVIDARSE DE CORTAR LAS SUSCRIPCIONES A LAS COLAS DEL BROKER.
		//VERIFICAR CUANDO LLEGA EL CAUGTH SI YA ESTOY EN CONDICIONES DE CORTAR LA SUSCRIPCION
		//ESO QUIERE DECIR (LOS OBTENIDOS SEAN LOS MISMO QUE LOS GLOBALES), IDEM LOCALIZED Y APPEARED.


		//IR SACANDO DE UN ENTRENADOR DE LA COLA BLOCKED
		//1.VERIFICAR SI ESTA EN DEADLOCK: SI OCURRE ESTO, SE DESCARTA Y SE COLACA NUEVAMENTE EN LA COLA
		//2. VERIFICAR SI ESTA EN ESPERANDO_RESPUESTA:  SI OCURRE ESTO, SE DESCARTA Y SE COLACA NUEVAMENTE EN LA COLA
		//3.CUALQUIER OTRO CASO, LO PONGO EN LA COLA DE READY.

		//variable local que me indique si ENCONTRE UNO LISTA PARA PASAR A READY
		//Si es FALSE, ahi recien recorre la lista de NEW.

		//AGREGAR UN SIGNLA POR CADA POKEOMON LIBRE QUE LLEGA EN LA RECEPCION DE MENSAJES DE POKEMONES, MAS
		//UN SIGNAL EN EL CASO DE FAIL. SIEMPRE Y CUANDO TENGAMOS POKEMONES LIBRES DE ESA ESPECIE.

	}
}
