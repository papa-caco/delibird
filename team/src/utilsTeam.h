#ifndef SRC_UTILSTEAM_H_
#define SRC_UTILSTEAM_H_

#include "teamInitializer.h"

//-----------------Firma de Funciones----------------------------

void inicio_server_team(void);

int enviar_mensaje_get(t_msg_get_broker *msg_get);

void enviar_mensaje_catch(t_msg_catch_broker *msg_catch);

int enviar_catch_pokemon_broker(t_msg_catch_broker *msg_catch, t_log *logger);

void inicio_suscripcion(t_tipo_mensaje *cola);

uint32_t rcv_msjs_broker_publish(op_code codigo_operacion, int socket_cliente, t_log *logger);

void enviar_catch_de_appeared(t_msg_appeared_team *msg_appeared);

void iniciar_cnt_msjs(void);

void iniciar_log_team(void);

void leer_config_team(char *path);

void process_msjs_gameboy(op_code cod_op, int cliente_fd, t_log *logger);

void liberar_listas(char** lista);

void liberar_lista_posiciones(t_list* list);

int contador_msjs_cola(t_tipo_mensaje cola_suscripcion);

#endif /* SRC_UTILSTEAM_H_ */
