/*
 * utils.c
 *
 *  Created on: 20 abr. 2020
 *      Author: utnso
 */
#include "utils_gc.h"

void iniciar_gamecard(void)
{
	leer_config();
	iniciar_log_gamecard();
	iniciar_estructuras_gamecard();
	manejo_senial_externa_gc();
	iniciar_suscripciones_broker_gc(g_logger);
	lanzar_reconexion_broker_gc(g_logger);
	dispatcher_operaciones_pendientes(g_logger);
}

void iniciar_log_gamecard(void)
{
	bool log_habilitado = false;
	if (g_config_gc->show_logs_on_screen) {
		log_habilitado = true;
	}
	g_logger = log_create(g_config_gc->ruta_log, "GAME_CARD", log_habilitado, LOG_LEVEL_TRACE);
	g_logdebug = log_create(PATH_LOG, "GAMECARD_DBG", log_habilitado, LOG_LEVEL_TRACE);
	puts("");
	log_info(g_logger, "INICIO_LOG_SUCESS");
}

void leer_config(void)
{
	gc_config = config_create(PATH_CONFIG);
	g_config_gc = malloc(sizeof(t_config_gamecard));
	g_config_gc->ip_gamecard = config_get_string_value(gc_config, "IP_GAMECARD");
	g_config_gc->puerto_gamecard = config_get_string_value(gc_config, "PUERTO_GAMECARD");
	g_config_gc->ip_broker = config_get_string_value(gc_config, "IP_BROKER");
	g_config_gc->puerto_broker = config_get_string_value(gc_config, "PUERTO_BROKER");
	g_config_gc->path_tall_grass = config_get_string_value(gc_config, "PUNTO_MONTAJE_TALLGRASS");
	g_config_gc->id_suscriptor = config_get_int_value(gc_config, "ID_SUSCRIPTOR");
	g_config_gc->tiempo_reconexion = config_get_int_value(gc_config, "TIEMPO_DE_REINTENTO_CONEXION");
	g_config_gc->tmp_reintento_oper = config_get_int_value(gc_config, "TIEMPO_DE_REINTENTO_OPERACION");
	g_config_gc->ruta_log = config_get_string_value(gc_config, "RUTA_LOG");
	g_config_gc->show_logs_on_screen = verdadero_falso(config_get_string_value(gc_config,"SHOW_LOGS_ON_SCREEN"));
	g_config_gc->tg_block_size = config_get_string_value(gc_config, "BLOCK_SIZE");
	g_config_gc->tg_blocks = config_get_string_value(gc_config, "BLOCKS");
	g_config_gc->magic_number = config_get_string_value(gc_config, "MAGIC_NUMBER");
}

void inicio_server_gamecard(void)
{
	char *ip = g_config_gc->ip_gamecard;
	char *puerto = g_config_gc->puerto_gamecard;
	puts("");
	iniciar_servidor_gc(ip, puerto, g_mutex_recepcion, g_logger);
}

void iniciar_estructuras_gamecard(void)
{
	sem_init(&sem_mutex_suscripcion, 0, 1);
	sem_init(&sem_mutex_semaforos, 0, 1);
	pthread_mutex_init(&g_mutex_cnt_blocks, 0);
	pthread_mutex_init(&g_mutex_tallgrass, 0);
	pthread_mutex_init(&g_mutex_envio, 0);
	pthread_mutex_init(&g_mutex_recepcion, 0);
	pthread_mutex_init(&g_mutex_open_files_list, 0);
	pthread_mutex_init(&g_mutex_reintentos, 0);
	g_reintentos = queue_create();
	semaforos_pokemon = list_create();
	leer_metadata_tall_grass(g_logdebug);
	inicializar_bitmap_tallgrass(g_logdebug);
	iniciar_cnt_msjs_gc();
	status_conexion_broker = true;
	sem_init(&mutex_msjs_gc, 0, 1);

}

void atender_gameboy_gc(int *cliente_fd)
{
	pthread_mutex_lock(&g_mutex_recepcion);
	log_trace(g_logger,"(NEW CLIENT CONNECTED | SOCKET#:%d)", *cliente_fd);
	op_code cod_op;
	if (recv(*cliente_fd, &cod_op, sizeof(op_code), MSG_WAITALL) == -1) {
		cod_op = -1;
	}
	if (es_cod_oper_mensaje_gamecard(cod_op)) {
		recibir_msjs_gameboy(cod_op, cliente_fd, g_logger);
	} else {
		log_error(g_logger, "RECIBI MSJ EQUIVOCADO DEL GAMEBOY");
	}
}

void recibir_msjs_gameboy(op_code cod_op, int *cliente_fd, t_log *logger)
{	// Procesa los diferentes mensajes que recibe del GAMEBOY
	switch (cod_op) {
	case GET_GAMECARD:;
		t_msg_get_gamecard *msg_get = rcv_msj_get_gamecard(*cliente_fd, logger);
		enviar_msg_confirmed(*cliente_fd, logger);
		pthread_mutex_unlock(&g_mutex_recepcion);
		recibir_msg_get_pokemon(msg_get, logger);
		puts("");
		break;
	case CATCH_GAMECARD:;
		t_msg_catch_gamecard *msg_catch = rcv_msj_catch_gamecard(*cliente_fd, logger);
		enviar_msg_confirmed(*cliente_fd, logger);
		pthread_mutex_unlock(&g_mutex_recepcion);
		recibir_msg_catch_pokemon(msg_catch, logger);
		puts("");
		break;
	case NEW_GAMECARD:;
		t_msg_new_gamecard *msg_new = rcv_msj_new_gamecard(*cliente_fd, logger);
		enviar_msg_confirmed(*cliente_fd, logger);
		pthread_mutex_unlock(&g_mutex_recepcion);
		recibir_msg_new_pokemon(msg_new, logger);
		puts("");
		break;
	}
	close(*cliente_fd);
	free(cliente_fd);
}

int recibir_msg_get_pokemon(t_msg_get_gamecard *msg_get, t_log *logger)
{
	pthread_t tid;
	int thread_status = pthread_create(&tid, NULL, (void*) procesar_msg_get_pokemon, (void*) msg_get);
	if (thread_status != 0) {
		log_error(logger, "Thread create returned %d | %s", thread_status, strerror(thread_status));
	} else {
		pthread_detach(tid);
	}
	return thread_status;
}

void procesar_msg_get_pokemon(t_msg_get_gamecard *msg_get)
{
	uint32_t id_correlativo = msg_get->id_mensaje;
	char *pokemon = malloc(msg_get->size_pokemon);
	memcpy(pokemon, msg_get->pokemon, msg_get->size_pokemon);
	t_posicion_pokemon *posicion = malloc(sizeof(t_posicion_pokemon));
	posicion->pos_x = 0, posicion->pos_y = 0, posicion->cantidad = 0;
	if (!existe_archivo(pokemon)) {
		log_warning(g_logger,"(-->> El archivo Pokemon %s no existe en el File System <<--)", pokemon);
	} else if (esta_abierto_archivo_pokemon(pokemon)) {
		log_warning(g_logger,"(-->> Archivo Pokemon %s Abierto por otro mensaje <<--)\n", pokemon);
			encolar_operacion_tallgrass(id_correlativo, pokemon, posicion, LOCALIZED_POKEMON, g_logdebug);
	} else {
		pthread_mutex_lock(&g_mutex_tallgrass);
		t_archivo_pokemon *archivo = abrir_archivo_pokemon(pokemon, g_logger);
		t_list *coordenadas = obtener_coordenadas_archivo_pokemon(archivo, g_logger);
		cerrar_archivo_pokemon(archivo, g_logger);
		puts("");
		pthread_mutex_unlock(&g_mutex_tallgrass);
		enviar_localized_pokemon_broker(id_correlativo, pokemon, coordenadas, g_logger);
	}
	free(posicion);
	eliminar_msg_get_gamecard(msg_get);
	pthread_exit(NULL);
}

int recibir_msg_new_pokemon(t_msg_new_gamecard *msg_new, t_log *logger)
{
	pthread_t tid;
	int thread_status = pthread_create(&tid, NULL, (void*) procesar_msg_new_pokemon, (void*) msg_new);
	if (thread_status != 0) {
		log_error(logger, "Thread create returned %d | %s", thread_status, strerror(thread_status));
	} else {
		pthread_detach(tid);
	}
	return thread_status;
}

void procesar_msg_new_pokemon(t_msg_new_gamecard *msg_new)
{
	uint32_t id_correlativo = msg_new->id_mensaje;
	bool encolado = false;
	char *pokemon = malloc(msg_new->size_pokemon);
	memcpy(pokemon, msg_new->pokemon, msg_new->size_pokemon);
	t_posicion_pokemon *posicion = malloc(sizeof(t_posicion_pokemon));
	memcpy(&posicion->pos_x, &msg_new->coord->pos_x, sizeof(uint32_t));
	memcpy(&posicion->pos_y, &msg_new->coord->pos_y, sizeof(uint32_t));
	memcpy(&posicion->cantidad, &msg_new->cantidad, sizeof(uint32_t));
	if (!existe_archivo(pokemon)) {
		log_info(g_logger,"(El archivo Pokemon %s no existe en el File System - Creando Nuevo)", pokemon);
		int file_size = crear_archivo_pokemon(pokemon, posicion, g_logger);
		free(posicion);
	} else if (esta_abierto_archivo_pokemon(pokemon)) {
		log_warning(g_logger,"(-->> Archivo Pokemon %s bloqueado por otro mensaje <<--)", pokemon);
		encolar_operacion_tallgrass(id_correlativo, pokemon, posicion, APPEARED_POKEMON, g_logdebug);
		encolado = true;
	} else {
		pthread_mutex_lock(&g_mutex_tallgrass);
		t_archivo_pokemon *archivo = abrir_archivo_pokemon(pokemon, g_logger);
		actualizar_posiciones_archivo(archivo, posicion, g_logger);
		cerrar_archivo_pokemon(archivo, g_logger);
		puts("");
		pthread_mutex_unlock(&g_mutex_tallgrass);
	}
	if (!encolado){
		t_msg_appeared_broker *msj_appeared = malloc(sizeof(t_msg_appeared_broker));
		msj_appeared->coordenada = malloc(sizeof(t_coordenada));
		memcpy(&(msj_appeared->coordenada->pos_x), &(msg_new->coord->pos_x), sizeof(uint32_t));
		memcpy(&(msj_appeared->coordenada->pos_y), &(msg_new->coord->pos_y), sizeof(uint32_t));
		memcpy(&(msj_appeared->id_correlativo), &(msg_new->id_mensaje), sizeof(uint32_t));
		memcpy(&(msj_appeared->size_pokemon), &(msg_new->size_pokemon), sizeof(uint32_t));
		msj_appeared->pokemon = malloc(msj_appeared->size_pokemon);
		memcpy(msj_appeared->pokemon, msg_new->pokemon, msj_appeared->size_pokemon);
		enviar_appeared_pokemon_broker(msj_appeared, g_logger);
	}
	eliminar_msg_new_gamecard(msg_new);
	free(pokemon);//TODO
}

int recibir_msg_catch_pokemon(t_msg_catch_gamecard *msg_catch, t_log *logger)
{
	pthread_t tid;
	int thread_status = pthread_create(&tid, NULL, (void*) procesar_msg_catch_pokemon, (void*) msg_catch);
	if (thread_status != 0) {
		log_error(logger, "Thread create returned %d | %s", thread_status, strerror(thread_status));
	} else {
		pthread_detach(tid);
	}
	return thread_status;
}

void procesar_msg_catch_pokemon(t_msg_catch_gamecard *msg_catch)
{
	uint32_t id_correlativo = msg_catch->id_mensaje;
	t_result_caught resultado = FAIL;
	char *pokemon = malloc(msg_catch->size_pokemon);
	memcpy(pokemon, msg_catch->pokemon, msg_catch->size_pokemon);
	t_posicion_pokemon *posicion = malloc(sizeof(t_posicion_pokemon));
	posicion->pos_x = msg_catch->coord->pos_x;
	posicion->pos_y = msg_catch->coord->pos_y;
	posicion->cantidad = 1;
	if (!existe_archivo(pokemon)) {
		log_warning(g_logger,"(-->> El archivo Pokemon %s no existe en el File System <<--)", pokemon);
	} else if (esta_abierto_archivo_pokemon(pokemon)) {
		log_warning(g_logger,"(-->> Archivo Pokemon %s bloqueado por otro mensaje <<--)", pokemon);
		encolar_operacion_tallgrass(id_correlativo, pokemon, posicion, CAUGHT_POKEMON, g_logdebug);
	} else {
		pthread_mutex_lock(&g_mutex_tallgrass);
		t_archivo_pokemon *archivo = abrir_archivo_pokemon(pokemon, g_logger);
		if (!existe_posicion_en_archivo(archivo, posicion)) {
			log_warning(g_logger,"(-->>No existe Posición [X=%d|Y=%d] en Archivo Pokemon %s <<--)",
				posicion->pos_x, posicion->pos_y, archivo->pokemon);
		} else {
			eliminar_posicion_en_archivo(archivo, posicion, g_logger);
			resultado = OK;
		}
		cerrar_archivo_pokemon(archivo, g_logger);
		puts("");
		pthread_mutex_unlock(&g_mutex_tallgrass);
		enviar_caught_pokemon_broker(id_correlativo, resultado, g_logger);
		free(pokemon);
	}
	free(posicion);
	eliminar_msg_catch_gamecard(msg_catch);
	pthread_exit(NULL);
}

void encolar_operacion_tallgrass(int32_t id_correlativo, char *pokemon, t_posicion_pokemon *posicion, t_tipo_mensaje cola, t_log* logger)
{
	t_operacion_tallgrass *operacion = malloc(sizeof(t_operacion_tallgrass));
	operacion->posicion = malloc(sizeof(t_posicion_pokemon));
	int size_pokemon =  strlen(pokemon)+1;
	operacion->pokemon = malloc(size_pokemon);
	operacion->id_correlativo = id_correlativo;
	operacion->posicion->pos_x = posicion->pos_x;
	operacion->posicion->pos_y = posicion->pos_y;
	operacion->posicion->cantidad = posicion->cantidad;
	operacion->id_cola = cola;
	memcpy(operacion->pokemon, pokemon, size_pokemon);
	pthread_mutex_lock(&g_mutex_reintentos);
	log_trace(logger, "(Mensaje %s con ID_Correlativo %d pendiente de envío|Archivo Pokemon %s Abierto)",
			nombre_cola(cola), id_correlativo, operacion->pokemon);
	queue_push(g_reintentos, (t_operacion_tallgrass*) operacion);
	pthread_mutex_unlock(&g_mutex_reintentos);
	if (cola != APPEARED_POKEMON) {
		free(pokemon); //TODO
	}
	if (cola == APPEARED_POKEMON) {
		free(posicion);
	}

}

int dispatcher_operaciones_pendientes(t_log *logger)
{
	pthread_t tid;
	int tid_status = pthread_create(&tid, NULL, (void*) inicio_dispatcher_operaciones_pendientes, NULL);
	if (tid_status != 0) {
		log_error(logger, "Thread create returned %d | %s", tid_status,	strerror(tid_status));
	} else {
		pthread_detach(tid);
	}
	return tid_status;
}

void inicio_dispatcher_operaciones_pendientes(void)
{
	int intervalo_reintento = g_config_gc->tmp_reintento_oper;
	while (1) {
		sleep(intervalo_reintento);
		int cant_operaciones = queue_size(g_reintentos);
		if(cant_operaciones > 0) {
			log_info(g_logger,"(Procesando %d Mensajes pendientes)", cant_operaciones);
			completar_operaciones_pendientes(g_logger);
		}

	}
}

void completar_operaciones_pendientes(t_log *logger)
{
	pthread_mutex_lock(&g_mutex_reintentos);
	while(queue_size(g_reintentos) > 0) {
		t_operacion_tallgrass *operacion = (t_operacion_tallgrass*) queue_pop(g_reintentos);
		quitar_de_lista_archivos_abiertos(operacion->pokemon);
		switch (operacion->id_cola) {
		case LOCALIZED_POKEMON:;
			t_msg_get_gamecard *msg_get = malloc(sizeof(t_msg_get_gamecard));
			msg_get->id_mensaje = operacion->id_correlativo;
			msg_get->size_pokemon = strlen(operacion->pokemon) + 1;
			msg_get->pokemon = malloc(msg_get->size_pokemon);
			memcpy(msg_get->pokemon, operacion->pokemon, msg_get->size_pokemon);
			recibir_msg_get_pokemon(msg_get, logger);
			break;
		case APPEARED_POKEMON:;
			t_msg_new_gamecard *msg_new = malloc(sizeof(t_msg_new_gamecard));
			msg_new->id_mensaje = operacion->id_correlativo;
			msg_new->size_pokemon = strlen(operacion->pokemon) + 1;
			msg_new->pokemon = malloc(msg_new->size_pokemon);
			memcpy(msg_new->pokemon, operacion->pokemon, msg_new->size_pokemon);
			msg_new->coord = malloc(sizeof(t_coordenada));
			msg_new->coord->pos_x = operacion->posicion->pos_x;
			msg_new->coord->pos_y = operacion->posicion->pos_y;
			msg_new->cantidad = operacion->posicion->cantidad;
			recibir_msg_new_pokemon(msg_new, logger);
			break;
		case CAUGHT_POKEMON:;
			t_msg_catch_gamecard *msg_catch = malloc(sizeof(t_msg_catch_gamecard));
			msg_catch->id_mensaje = operacion->id_correlativo;
			msg_catch->size_pokemon = strlen(operacion->pokemon) + 1;
			msg_catch->pokemon = malloc(msg_catch->size_pokemon);
			memcpy(msg_catch->pokemon, operacion->pokemon, msg_catch->size_pokemon);
			msg_catch->coord = malloc(sizeof(t_coordenada));
			msg_catch->coord->pos_x = operacion->posicion->pos_x;
			msg_catch->coord->pos_y = operacion->posicion->pos_y;
			recibir_msg_catch_pokemon(msg_catch, logger);
			break;
		}
		eliminar_operacion_tallgrass(operacion);
	}
	pthread_mutex_unlock(&g_mutex_reintentos);
}

void eliminar_operacion_tallgrass(t_operacion_tallgrass *operacion)
{
	free(operacion->posicion);
	free(operacion->pokemon);
	free(operacion);
}

void liberar_lista_posiciones(t_list* lista) {
	for (int i = 0; i < list_size(lista); i++) {
		free(list_get(lista, i));
	}

	list_destroy(lista);
}

void liberar_listas(char** lista) {

	int contador = 0;
	while (lista[contador] != NULL) {
		free(lista[contador]);
		contador++;
	}

	free(lista);
}

t_pokemon_semaforo *obtener_semaforo_pokemon(char* pokemon) {

	sem_wait(&sem_mutex_semaforos);
	for (int i = 0; i < list_size(semaforos_pokemon); i++) {
		t_pokemon_semaforo *actual = list_get(semaforos_pokemon, i);
		char* nombrePokemon = actual->pokemon;
		if (strcmp(nombrePokemon, pokemon) == 0) {
			sem_post(&sem_mutex_semaforos);
			return actual;
		}
	}
	sem_post(&sem_mutex_semaforos);
	return NULL;
}

void eliminar_semaforo_pokemon(char* pokemon) {

	sem_wait(&sem_mutex_semaforos);
	for (int i = 0; i < list_size(semaforos_pokemon); i++) {
		t_pokemon_semaforo *actual = list_get(semaforos_pokemon, i);
		char* nombrePokemon = actual->pokemon;
		if (strcmp(nombrePokemon, pokemon) == 0) {
			list_remove(semaforos_pokemon, i);
			sem_destroy(&actual->semaforo);
			free(actual);
			sem_post(&sem_mutex_semaforos);
		}
	}
	sem_post(&sem_mutex_semaforos);

}

void crear_semaforo_pokemon(char* pokemon) {

	t_pokemon_semaforo *pok = malloc(sizeof(t_pokemon_semaforo));
	pok->pokemon = pokemon;
	sem_t semaforo;
	sem_init(&semaforo, 0, 1);
	pok->semaforo = semaforo;
	sem_wait(&sem_mutex_semaforos);
	list_add(semaforos_pokemon, pok);
	sem_post(&sem_mutex_semaforos);
}

char *concatenar_posiciones_pokemon(t_list *posiciones)
{
	int cant_elem = posiciones->elements_count;
	char *cadena = calloc(2000, sizeof(char));
	strcpy(cadena, "[");
	for (int i = 0; i < cant_elem; i ++) {
		t_posicion_pokemon *posicion =  list_get(posiciones,i);
		int pos_x,pos_y, cantidad;
		memcpy(&pos_x,&posicion->pos_x, sizeof(int));
		memcpy(&pos_y,&posicion->pos_y, sizeof(int));
		memcpy(&cantidad,&posicion->cantidad, sizeof(int));
		char *posx = string_itoa(pos_x);
		char *posy = string_itoa(pos_y);
		char *cant = string_itoa(cantidad);
		char auxiliar[5];
		strcpy(auxiliar,posx);
		strcat(cadena,auxiliar);
		strcat(cadena,"-");
		strcpy(auxiliar,posy);
		strcat(cadena,auxiliar);
		strcat(cadena,"=");
		strcpy(auxiliar,cant);
		strcat(cadena,auxiliar);
		if (i == cant_elem - 1) {
			strcat(cadena,"]");
		}
		else {
			strcat(cadena,",");
		}
		free(posx);
		free(posy);
		free(cant);
	}
	return cadena;
}

bool es_cod_oper_mensaje_gamecard(op_code codigo_operacion)
{
	return codigo_operacion == NEW_GAMECARD || codigo_operacion == CATCH_GAMECARD
			|| codigo_operacion == GET_GAMECARD || codigo_operacion == COLA_VACIA || codigo_operacion == SUSCRIP_END;
}

bool nro_par(int numero)
{
	return ((int) (numero % 2) == 0);
}
