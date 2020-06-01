#ifndef SRC_UTILSTEAM_H_
#define SRC_UTILSTEAM_H_

#include "teamInitializer.h"

//-----------------Firma de Funciones----------------------------

void inicio_server_team(void);

void iniciar_suscripciones_broker(void);

int iniciar_suscripcion_cola(t_tipo_mensaje cola_suscripta, t_log *logger);

int enviar_get_pokemon_broker(char *pokemon, t_log *logger);

int connect_broker_y_enviar_mensaje_get(t_msg_get_broker *msg_get);

int enviar_catch_pokemon_broker(int pos_x, int pos_y, char* pokemon, t_log *logger);

int connect_broker_y_enviar_mensaje_catch(t_msg_catch_broker *msg_catch);

void inicio_suscripcion(t_tipo_mensaje *cola);

uint32_t rcv_msjs_broker_publish(op_code codigo_operacion, int socket_cliente, t_log *logger);

void enviar_catch_de_appeared(t_msg_appeared_team *msg_appeared);

void enviar_get_de_appeared(t_msg_appeared_team *msg_appeared);

void iniciar_cnt_msjs(void);

void enviar_msjs_get_objetivos(void);

void lanzar_reconexion_broker(t_log *logger);

void funciones_reconexion(void);

bool codigo_operacion_valido(op_code code_op);

void iniciar_log_team(void);

void leer_config_team(char *path);

void process_msjs_gameboy(op_code cod_op, int cliente_fd, t_log *logger);

void liberar_listas(char** lista);

void liberar_lista_posiciones(t_list* list);

int contador_msjs_cola(t_tipo_mensaje cola_suscripcion);

#endif /* SRC_UTILSTEAM_H_ */
