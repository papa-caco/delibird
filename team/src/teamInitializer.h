#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<commons/txt.h>
#include<commons/collections/list.h>
#include<commons/collections/queue.h>
#include<string.h>
#include<pthread.h>
#include<stdbool.h>
#include"utilsTeam.h"

#define IP "127.0.0.1"
#define PUERTO "5001"
#define ID_MSG_RTA 65535
#define RESPUESTA_OK "RECIBIDO_OK"
#define HANDSHAKE_SUSCRIPTOR 255
//t_queue* new;
//t_list* objetivoGlobal;


typedef struct Posicion_Entrenador{
	int pos_x;
	int pos_y;
} t_posicion_entrenador;

typedef struct Pokemon_Entrenador{
	char* pokemon;
	int cantidad;
} t_pokemon_entrenador;

typedef enum Estado_Entrenador{
	NEW,
	READY,
	BLOCKED,
	EXEC,
	EXIT,
} t_estado;

typedef struct Entrenador{
	t_posicion_entrenador* posicion;
	t_list* objetivoEntrenador;
	t_list* pokemonesObtenidos;
	//Tal vez como opción podrías agregar una variable hilo acá e ir inicializándoselo a cada uno.
	//La otra es crear los hilos por otro lado e ir manejándolos a tu criterio.
} t_entrenador;

//-----------------Variables Globales----------------------------

pthread_t thread;


t_log *g_logger;

//-----------------Firma de Funciones----------------------------

t_list * extraer_posiciones_entrenadores();

void* recibir_buffer(int*, int);

int recibir_operacion(int socket);

void iniciar_team(void);

void iniciar_servidor(void);

void iniciar_logger(void);

void serve_client(t_socket_cliente *socket);

void process_request(op_code cod_op, t_socket_cliente *socket);

void process_suscript(op_code cod_op, t_socket_cliente *socket);

void* recibir_mensaje(int socket_cliente, int* size);

void* rcv_catch_broker(int socket_cliente, int* size);

void *rcv_caught_broker(int socket_cliente,int *size);

void* rcv_new_broker(int socket_cliente, int* size);

void* rcv_new_gamecard(int socket_cliente, int* size);

void* rcv_get_broker(int socket_cliente, int* size);

void *rcv_get_gamecard(int socket_cliente, int *size);

void* rcv_catch_gamecard(int socket_cliente, int* size);

void* rcv_appeared_broker(int socket_cliente, int* size);

void* rcv_appeared_team(int socket_cliente, int* size);

//t_handsake_suscript *rcv_handshake_suscripcion(t_socket_cliente *socket, int *size);

//t_handsake_suscript *rcv_fin_suscripcion(t_socket_cliente *socket, int *size);

//void *serializar_paquete(t_paquete* paquete, int bytes);

void devolver_id_mensaje_propio(int socket_cliente);

void devolver_recepcion_ok(int socket_cliente);

void devolver_recepcion_fail(int socket_cliente, char* mensajeError);

void devolver_caught_broker(void *msg, int socket_cliente);

void devolver_appeared_broker(void *msg, int size, int socket_cliente);

void devolver_id_mensaje(void *msg,int socket_cliente);

void devolver_posiciones(int socket_cliente, char* pokemon, int* encontroPokemon); //Hace un send de la lista de posiciones y cantidad de un pokemon

void enviar_cola_vacia(int socket_cliente, int id_suscriptor);

void enviar_msjs_get(int socket_cliente,int  id_mensaje);

void enviar_msjs_new(int socket_cliente,int  id_mensaje);

void esperar_cliente(int socket);

void liberar_lista_posiciones(t_list* list);

int tamano_recibido(int bytes);

//void eliminar_paquete (t_paquete *paquete);

t_pokemon_entrenador* list_buscar(t_list* lista, char* elementoAbuscar);

t_list *extraer_pokemones_entrenadores(char* configKey);

void iniciar_entrenadores_and_objetivoGlobal(t_queue* cola, t_list* objetivoGlobal);

void cargar_objetivo_global(t_list* objetivosDeTodosEntrenadores, t_list* objetivoGlobal);

void liberar_lista(t_list* lista);

void liberar_cola(t_queue* cola);
