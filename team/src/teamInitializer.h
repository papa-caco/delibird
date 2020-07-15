#ifndef SRC_TEAMINITIALIZER_H_
#define SRC_TEAMINITIALIZER_H_

#include<delibird/estructuras.h>
#include<delibird/mensajeria.h>
#include<delibird/serializaciones.h>
#include<delibird/conexiones.h>
//#include "utilsTeam.h"




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

typedef struct Posicion_Entrenador{
	int pos_x;
	int pos_y;
} t_posicion_entrenador;

typedef struct Pokemon_Entrenador{
	int cantidad;
	char* pokemon;
	t_posicion_entrenador* posicion;
} t_pokemon_entrenador;




typedef enum Estado_Entrenador{
	MOVERSE_A_POKEMON,
	MOVERSE_A_ENTRENADOR,
	ATRAPAR,
	INTERCAMBIAR,
	ACABO_INTERCAMBIO,
	RECIBIO_RESPUESTA_OK,
	ESPERAR_CAUGHT,
	DEADLOCK,
	EXIT,
} t_estado_entrenador;

typedef struct Entrenador{
	t_posicion_entrenador* posicion;
	t_list* objetivoEntrenador;
	t_list* pokemonesObtenidos;
	t_estado_entrenador estado_entrenador;
	sem_t sem_entrenador;
	sem_t mutex_entrenador;
	pthread_t hilo_entrenador;
	int id;

	//FIJARSE SI HAY QUE AGREGAR UN SEMAFORO MUTEX PARA EL ENTRENADOR CUANDO SE MODIFIQUE O SE LEA.
} t_entrenador;

typedef struct id_Correlativo_and_Entrenador{
	int id_Correlativo;
	int id_Entrenador;
} t_id_Correlativo_and_Entrenador;


typedef struct Pokemon_Entrenador_Reservado{
	int id_entrenadorReserva;
	int cantidad;
	char* pokemon;
	t_posicion_entrenador* posicion;
} t_pokemon_entrenador_reservado;





//-----------------Variables Globales----------------------------

pthread_t threadPlanificadorCP;

pthread_t threadPlanificadorMP;

int cantidadDeEntrenadores;

char finalizarProceso;

t_queue* colaNewEntrenadores;

t_queue* colaReadyEntrenadores;

t_queue* colaBlockedEntrenadores;

t_queue* colaExitEntrenadores;

t_list* objetivoGlobalEntrenadores;

t_list* pokemonesLibresEnElMapa;

t_list* pokemonesReservadosEnElMapa;

t_list* pokemonesAtrapadosGlobal;

t_config *g_config;

t_config_team *g_config_team;

t_log *g_logger;

pthread_t tid_send_get;

pthread_t tid_send_catch;

pthread_mutex_t mutex_reconexion;

pthread_t tid_reconexion;

sem_t sem_mutex_msjs;

//--------------SEMAFOROS LISTAS DE POKEMONES------------------------------

sem_t sem_pokemonesGlobalesAtrapados;
sem_t sem_pokemonesReservados;
sem_t sem_pokemonesLibresEnElMapa;
sem_t sem_pokemonesObjetivoGlobal;

//---------------SEMAFOROS COLAS DE ENTRENADORES---------------------------

sem_t sem_cola_blocked;
sem_t sem_cola_new;
sem_t sem_cola_ready;
sem_t sem_cola_exit;


//--------------SEMAFOROS PLANIFICADORES-----------------------------------



sem_t sem_planificador_cplazoReady;

sem_t sem_planificador_cplazoEntrenador;

sem_t sem_planificador_mplazo;

sem_t sem_hay_pokemones_mapa;

sem_t sem_terminar_todo;

int g_cnt_msjs_caught;

int g_cnt_msjs_appeared;

int g_cnt_msjs_localized;

bool status_conn_broker;

//-----------------Firma de Funciones----------------------------

void iniciar_team(void);

void iniciar_variables_globales();

t_list * extraer_posiciones_entrenadores();

t_pokemon_entrenador* list_buscar(t_list* lista, char* elementoAbuscar);

t_list *extraer_pokemones_entrenadores(char* configKey);

void iniciar_entrenadores_and_objetivoGlobal();

void cargar_objetivo_global(t_list* objetivosDeTodosEntrenadores);

void cargar_obtenidos_global(t_list* pokemonesObtenidosGlobal);

void print_pokemones_objetivo(t_pokemon_entrenador *poke);

void enviar_msjs_get_por_clase_de_pokemon(t_pokemon_entrenador *poke);

void liberar_lista_de_pokemones(t_list* lista);

//void liberar_lista_de_entrenadores(t_list* lista);

void liberar_lista(t_list* lista);

void liberar_cola(t_queue* cola);

#endif /* SRC_TEAMINITIALIZER_H_ */
