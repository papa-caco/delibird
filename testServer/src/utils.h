#ifndef CONEXIONES_H_
#define CONEXIONES_H_

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
#include<string.h>
#include<pthread.h>
#include<stdbool.h>

#define IP "127.0.0.1"
#define PUERTO "5001"
#define ID_MSG_RTA 5535
#define RESPUESTA_OK 1001
#define HANDSHAKE_SUSCRIPTOR 255

typedef enum Resultado_Caught{
	FAIL, OK,
} t_result_caught;

typedef enum Tipo_Mensaje{
	UNKNOWN_QUEUE = 9,
	NEW_POKEMON,
	APPEARED_POKEMON,
	CATCH_POKEMON,
	CAUGHT_POKEMON,
	GET_POKEMON,
	LOCALIZED_POKEMON,
} t_tipo_mensaje;

typedef enum Codigo_Operacion
{
	ID_MENSAJE = 10,
	MSG_CONFIRMED,
	MSG_ERROR,
	FIN_SUSCRIPCION,
	NEW_BROKER = 20,
	APPEARED_BROKER,
	CATCH_BROKER,
	CAUGHT_BROKER,
	GET_BROKER,
	LOCALIZED_BROKER,
	NEW_GAMECARD = 40,
	CATCH_GAMECARD,
	GET_GAMECARD,
	APPEARED_TEAM = 80,
	CAUGHT_TEAM,
	LOCALIZED_TEAM,
	SUSCRIP_NEW = 120,
	SUSCRIP_APPEARED,
	SUSCRIP_CATCH,
	SUSCRIP_CAUGHT,
	SUSCRIP_GET,
	SUSCRIP_LOCALIZED,
	SUSCRIP_END,
	COLA_VACIA,
} op_code;

typedef struct Coordenada{
	int pos_x;
	int pos_y;
} t_coordenada;

typedef struct Posiciones_Localized{
	int cantidad;
	t_list *coordenadas;
} t_posiciones_localized;

typedef struct Posicion_Pokemon{
	int pos_x;
	int pos_y;
	int cantidad;
} t_posicion_pokemon;

typedef struct Handshake_Suscriptor{
	int id_suscriptor;
	int msjs_recibidos;
} t_handsake_suscript;

typedef struct Msg_Appeared_Pokemon{
	t_coordenada *coord;
	char *pokemon;
} t_msg_appeared;

typedef struct Msg_Caught_Pokemon{
	int id_correlativo;
	t_result_caught resultado;
} t_msg_caught;

typedef struct Msg_Localized_Pokemon{
	t_posiciones_localized *posiciones;
	char *pokemon;
} t_msg_localized;

typedef struct Msg_New_Pokemon{
	int id_mensaje;
	t_coordenada *coord;
	int cantidad;
	char *pokemon;
} t_msg_new;

typedef struct Msg_Catch_Pokemon{
	int id_mensaje;
	t_coordenada *coord;
	char *pokemon;
} t_msg_catch;

typedef struct Msg_Get_Pokemon{
	int id_mensaje;
	char *pokemon;
} t_msg_get;


typedef struct Stream{
	int size;
	void* data;
} t_stream;

typedef struct Paquete{
	op_code codigo_operacion;
	t_stream* buffer;
} t_paquete;

typedef struct Socket_Cliente{
	int cliente_fd;
	int cant_msg_enviados;
} t_socket_cliente;

typedef struct Suscriptor_Broker{
	int id_suscriptor;
	bool is_active;
	int cant_msg;
} t_suscriptor;


//-----------------Variables Globales----------------------------

pthread_t thread;


t_log *g_logger;

t_list *g_suscriptores;

//-----------------Firma de Funciones----------------------------

void* recibir_buffer(int*, int);

int recibir_operacion(int socket);

void iniciar_servidor(void);

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

t_handsake_suscript *rcv_handshake_suscripcion(t_socket_cliente *socket, int *size);

t_handsake_suscript *rcv_fin_suscripcion(int socket_cliente, int *size);

void start_suscription(t_list *suscriptores,t_handsake_suscript *suscrpitor);

void stop_suscription(t_list *suscriptores, t_handsake_suscript *suscrpitor);

bool is_active_suscriptor(t_list *suscriptores, int id_suscriptor);

t_suscriptor *obtengo_suscriptor(t_list *suscriptores, int id_suscriptor);

void remove_ended_suscriptor(t_list *suscriptores, int id_suscriptor);

void delete_suscriptor(t_list *suscriptores, void *suscriptor);

void *serializar_paquete(t_paquete* paquete, int bytes);

void devolver_id_mensaje_propio(int socket_cliente);

void liberar_listas(char** lista);

void devolver_recepcion_ok(int socket_cliente);

void devolver_recepcion_fail(int socket_cliente, char* mensajeError);

void devolver_caught_broker(void *msg, int socket_cliente);

void devolver_appeared_broker(void *msg, int size, int socket_cliente);

void devolver_id_mensaje(void *msg,int socket_cliente);

void devolver_posiciones(int socket_cliente, char* pokemon, int* encontroPokemon); //Hace un send de la lista de posiciones y cantidad de un pokemon

void enviar_cola_vacia(t_socket_cliente *socket, int id_suscriptor);

void enviar_msjs_get(t_socket_cliente *socket);

void enviar_msjs_new(t_socket_cliente *socket);

void enviar_msjs_appeared(t_socket_cliente *socket);

void enviar_msjs_catch(t_socket_cliente *socket);

void enviar_msjs_caught(t_socket_cliente *socket);

void enviar_msjs_localized(t_socket_cliente *socket);

t_posiciones_localized *generar_posiciones_localized(int cantidad);

int coordenada_posiciones(t_posiciones_localized* posiciones, int indice);

char *concat_coord(t_posiciones_localized *posiciones);

void eliminar_msg_localized(t_msg_localized *msg_localized);

void enviar_suscript_end(t_socket_cliente *socket, int id_suscriptor);

void esperar_cliente(int socket);

void liberar_lista_posiciones(t_list* list);

char *result_caught(t_result_caught resultado);

int tamano_recibido(int bytes);

int cant_coordenadas(int valor);

void eliminar_paquete (t_paquete *paquete);

void iniciar_estructuras(void);

void eliminar_estructuras(void);

void iniciar_logger(void);

#endif /* CONEXIONES_H_ */
