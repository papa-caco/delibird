#ifndef SRC_TEAMINITIALIZER_H_
#define SRC_TEAMINITIALIZER_H_

#include<delibird/estructuras.h>
#include<delibird/mensajeria.h>
#include<delibird/serializaciones.h>
#include<delibird/conexiones.h>
#include "utilsTeam.h"
#include "entrenador.h"

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




//-----------------Variables Globales----------------------------

t_queue* colaNewEntrenadores;

t_list* objetivoGlobalEntrenadores;

t_config *g_config;

t_config_team *g_config_team;

t_log *g_logger;

pthread_t tid_send_get;

pthread_t tid_send_catch;

pthread_mutex_t mutex_reconexion;

pthread_t tid_reconexion;

sem_t sem_mutex_msjs;

int g_cnt_msjs_caught;

int g_cnt_msjs_appeared;

int g_cnt_msjs_localized;

bool status_conn_broker;

//-----------------Firma de Funciones----------------------------

void iniciar_team(void);

t_list * extraer_posiciones_entrenadores();

t_pokemon_entrenador* list_buscar(t_list* lista, char* elementoAbuscar);

t_list *extraer_pokemones_entrenadores(char* configKey);

void iniciar_entrenadores_and_objetivoGlobal();

void cargar_objetivo_global(t_list* objetivosDeTodosEntrenadores);

void print_pokemones_objetivo(t_pokemon_entrenador *poke);

void enviar_msjs_get_por_clase_de_pokemon(t_pokemon_entrenador *poke);

void liberar_lista(t_list* lista);

void liberar_cola(t_queue* cola);

#endif /* SRC_TEAMINITIALIZER_H_ */
