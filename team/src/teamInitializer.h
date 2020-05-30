#ifndef SRC_TEAMINITIALIZER_H_
#define SRC_TEAMINITIALIZER_H_

#include<delibird/estructuras.h>
#include<delibird/mensajeria.h>
#include<delibird/serializaciones.h>
#include<delibird/conexiones.h>
#include "utilsTeam.h"

#define ID_MSG_RTA 65535
#define RUTA_CONFIG_TEAM "config/team.config"
#define HANDSHAKE_SUSCRIPTOR 255

typedef struct Configuracion_Team
{
	char *ip_broker;
	char *puerto_broker;
	char *ip_team;
	char *puerto_team;
	int tiempo_reconexion;
	int retardo_ciclo_cpu;
	char *algoritmo_planificion;
	int quantum;
	int estimacion_inicial;
	char *ruta_log;
	int id_suscriptor;
} t_config_team;


/*t_queue* new;
//t_list* objetivoGlobal;*/

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

t_queue* colaNewEntrenadores;

t_list* objetivoGlobalEntrenadores;

t_config *g_config;

t_config_team *g_config_team;

t_log *g_logger;

pthread_t tid_localized;

pthread_t tid_appeared;

pthread_t tid_caught;

pthread_t tid_send_msjs;

int g_cnt_msjs_caught;

int g_cnt_msjs_appeared;

int g_cnt_msjs_localized;


//-----------------Firma de Funciones----------------------------

void iniciar_team(void);

t_list * extraer_posiciones_entrenadores();

t_pokemon_entrenador* list_buscar(t_list* lista, char* elementoAbuscar);

t_list *extraer_pokemones_entrenadores(char* configKey);

void iniciar_entrenadores_and_objetivoGlobal();

void cargar_objetivo_global(t_list* objetivosDeTodosEntrenadores);

void liberar_lista(t_list* lista);

void liberar_cola(t_queue* cola);

#endif /* SRC_TEAMINITIALIZER_H_ */
