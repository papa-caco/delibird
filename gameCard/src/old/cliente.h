/*
 * cliente.h
 *
 *  Created on: 23 abr. 2020
 *      Author: utnso
 */

#ifndef SRC_CLIENTE_H_
#define SRC_CLIENTE_H_

typedef enum {
	UNKNOWN_PROC = 9, SUSCRIPTOR, GAMECARD, BROKER, TEAM, GAMEBOY,
} t_proceso;

typedef struct {
	t_proceso proceso;
	t_tipo_mensaje tipo_mensaje;
	t_list *argumentos;
} t_mensaje_gameboy;

void enviar_mensaje(char* mensaje, int socket_cliente);

char* recibir_mensaje_broken(int socket_cliente);
void* serializar_paquete_cliente(t_paquete* paquete, int *bytes);

void enviar_mensaje_al_canal(char* mensaje,int canal,int conexion);
void empaquetar_mensaje(char* mensaje,int canal,t_paquete *paquete);
int tamano_paquete(t_paquete *paquete);

int recibir_id_mensaje(int socket_cliente);
#endif /* SRC_CLIENTE_H_ */
