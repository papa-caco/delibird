/*
 * utils.c
 *
 *  Created on: 20 abr. 2020
 *      Author: utnso
 */
#include "utils_gc.h"
#include "suscripcion.h"
#include "tall_grass.h"

void iniciar_gamecard(void)
{
	leer_config();
	iniciar_log_gamecard();
	iniciar_estructuras_gamecard();

	crear_semaforos_pokemon();
	//prueba_semaforo();
	//prueba_file_system("Chorizo",50);
	//prueba_leer_bloques_pokemon("Choripa");
	iniciar_suscripciones_broker_gc(g_logger);
	lanzar_reconexion_broker_gc(g_logger);


		//prueba_semaforo();
		//prueba_file_system("Charizard",40);
		//prueba_leer_bloques_pokemon("Charizard");
		//iniciar_suscripcion();
		//inicio_server_game_card();
}

void iniciar_log_gamecard(void)
{
	bool log_habilitado = false;
	if (g_config_gc->show_logs_on_screen) {
		log_habilitado = true;
	}
	g_logger = log_create(g_config_gc->ruta_log, "GAME_CARD", log_habilitado, LOG_LEVEL_DEBUG);
	g_logdebug = log_create(PATH_LOG, "GAMECARD_DBG", log_habilitado, LOG_LEVEL_DEBUG);
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
	g_config_gc->dirname_blocks = config_get_string_value(gc_config, "DIRNAME_BLOCKS");
	g_config_gc->dirname_files = config_get_string_value(gc_config, "DIRNAME_FILES");
	g_config_gc->file_metadata = config_get_string_value(gc_config, "FILE_METADATA");
	g_config_gc->tiempo_reconexion = config_get_int_value(gc_config, "TIEMPO_DE_REINTENTO_CONEXION");
	g_config_gc->tmp_reintento_oper = config_get_int_value(gc_config, "TIEMPO_DE_REINTENTO_OPERACION");
	g_config_gc->ruta_log = config_get_string_value(gc_config, "RUTA_LOG");
	g_config_gc->show_logs_on_screen = verdadero_falso(config_get_string_value(gc_config,"SHOW_LOGS_ON_SCREEN"));
	g_config_gc->ruta_bitmap = config_get_string_value(gc_config, "RUTA_BITMAP");
}

void finalizar_log(void) {
	log_destroy(g_logger);
}
void destruir_config(void) {
	//config_destroy(config);
}

void inicio_server_gamecard(void) {
	char *ip = g_config_gc->ip_gamecard;
	char *puerto = g_config_gc->puerto_gamecard;
	puts("");
	iniciar_servidor(ip, puerto, g_logger);
}

void iniciar_estructuras_gamecard(void)
{
	sem_init(&sem_mutex_suscripcion, 0, 1);
	sem_init(&sem_mutex_semaforos, 0, 1);
	pthread_mutex_init(&g_mutex_cnt_blocks, 0);
	semaforos_pokemon = list_create();
	leer_metadata_tall_grass(g_logdebug);
	inicializar_bitmap_tallgrass(g_logdebug);
	iniciar_cnt_msjs_gc();
	status_conexion_broker = true;
	sem_init(&mutex_msjs_gc, 0, 1);
}

void atender_gameboy(int *cliente_fd) {
	op_code cod_op;
	if (recv(*cliente_fd, &cod_op, sizeof(op_code), MSG_WAITALL) == -1) {
		cod_op = -1;
	}
	if (es_cod_oper_mensaje_gamecard(cod_op)) {
		procesar_msjs_gameboy(cod_op, *cliente_fd, g_logger);
	} else {
		log_error(g_logger, "RECIBI MSJ EQUIVOCADO DEL GAMEBOY");
	}
}

void procesar_msjs_gameboy(op_code cod_op, int cliente_fd, t_log *logger)
{	// Procesa los diferentes mensajes que recibe del GAMEBOY
	switch (cod_op) {
	case GET_GAMECARD:;
		t_msg_get_gamecard *msg_get = rcv_msj_get_gamecard(cliente_fd, logger);
		//TODO Hacer lo que corresponda con el msg_get (funcion que dispare un nuevo hilo)
		enviar_msg_confirmed(cliente_fd, logger);
		eliminar_msg_get_gamecard(msg_get);
		break;
	case CATCH_GAMECARD:;
		t_msg_catch_gamecard *msg_catch = rcv_msj_catch_gamecard(cliente_fd, logger);
		//TODO Hacer lo que corresponda con el msg_catch (funcion que dispare un nuevo hilo)
		enviar_msg_confirmed(cliente_fd, logger);
		eliminar_msg_catch_gamecard(msg_catch);
		break;
	case NEW_GAMECARD:;
		t_msg_new_gamecard *msg_new = rcv_msj_new_gamecard(cliente_fd, logger);
		//TODO Hacer lo que corresponda con el msg_new (funcion que dispare un nuevo hilo)
		enviar_msg_confirmed(cliente_fd, logger);
		eliminar_msg_new_gamecard(msg_new);
		break;
	}
	close(cliente_fd);
}






void devolver_recepcion_fail(int socket_cliente, char* mensajeError) {
	t_paquete* paquete = malloc(sizeof(t_paquete));

	log_info(g_logger, "(RESPUESTA = %s)", mensajeError);

	paquete->codigo_operacion = MSG_ERROR;
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = strlen(mensajeError) + 1;
	paquete->buffer->data = malloc(paquete->buffer->size);

	memcpy(paquete->buffer->data, mensajeError, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2 * sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);
	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
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
/*
void devolver_posiciones(int socket_cliente, char* pokemon, int* encontroPokemon) {

	printf("El socket es : %d \n", socket_cliente);

	//Por como funciona la funcion strcat, guardo espacio para la ruta completa en el primer argumento
	char* ruta = malloc(20 + strlen(pokemon) + 1);

	strcpy(ruta, "/home/utnso/config/");

	//Creo la ruta segun el pokemon
	char* path = strcat(ruta, pokemon);

	printf("El tamanio del path es: %d \n", strlen(path));

	printf("El path del pokemon es: %s \n", path);

	FILE* posiciones = fopen(path, "r");

	//Verifico si el archivo existe (fopen devuelve null si no existe)
	if (posiciones != NULL) {
		//Asigno 1 haciendo referencia a que encontro el archivo
		*encontroPokemon = 1;

		char* line = NULL;
		size_t len = 0;
		ssize_t read = getline(&line, &len, posiciones);

		//Verifico que si el archivo esta vacio
		if (read == -1) {
			devolver_recepcion_fail(socket_cliente,
					"SE ENCONTRO EL ARCHIVO PERO ESTA VACIO");
			free(line);
			//Si tiene contenido, hago el flujo de carga
		} else {

			printf("Se encontró el archivo con contenido\n");
			t_config* config = config_create(
					"/home/utnso/config/gameboy.config");

			int idMensajeUnico = config_get_int_value(config,
					"ID_MENSAJE_UNICO");

			t_list* listaPosiciones = list_create();

			rewind(posiciones);

			while ((read = getline(&line, &len, posiciones)) != -1) {
				printf("Entro a leer lista\n");

				char** keyValue = string_split(line, "=");

				printf("Luego de primer split\n");

				char* key = keyValue[0];

				int cantidad = atoi(keyValue[1]);

				char** posiciones = string_split(key, "-");

				printf("Luego de segundo split\n");

				int posicionX = atoi(posiciones[0]);
				int posicionY = atoi(posiciones[1]);

				t_posicion_pokemon *posicion = malloc(
						sizeof(t_posicion_pokemon));

				posicion->cantidad = cantidad;
				posicion->pos_x = posicionX;
				posicion->pos_y = posicionY;

				printf("Pokemon %s : \n", pokemon);
				printf("Posicion X: %d \n", posicion->pos_x);
				printf("Posicion Y: %d \n", posicion->pos_y);
				printf("Cantidad: %d \n", posicion->cantidad);
				printf("------------------------------ \n");

				list_add(listaPosiciones, posicion);

				liberar_listas(keyValue);
				liberar_listas(posiciones);

			}
			printf("Salgo de leer lista\n");

			int cantidadDePosiciones = list_size(listaPosiciones);

			printf("La cantidad de posiciones en la lista es %d: \n",
					cantidadDePosiciones);

			int totalBytes = sizeof(int)
					+ cantidadDePosiciones * 3 * sizeof(int);

			printf("La cantidad de bytes son :%d \n", totalBytes);

			void* stream = malloc(totalBytes);

			int offset = 0;

			memcpy(stream + offset, &(idMensajeUnico), sizeof(int));
			offset += sizeof(int);

			printf("Comienzo a serializar\n");

			for (int procesados = 0; procesados < cantidadDePosiciones;
					procesados++) {

				t_posicion_pokemon* posicionActual = list_get(listaPosiciones,
						procesados);

				memcpy(stream + offset, &(posicionActual->cantidad),
						sizeof(int));
				offset += sizeof(int);

				memcpy(stream + offset, &(posicionActual->pos_x), sizeof(int));
				offset += sizeof(int);

				printf("la posicion x es : %d \n", posicionActual->pos_x);

				memcpy(stream + offset, &(posicionActual->pos_y), sizeof(int));
				offset += sizeof(int);

				printf("la posicion y es : %d \n", posicionActual->pos_y);

				printf("El offset es: %d \n", offset);

			}

			printf("Termine de armar el stream \n");

			t_paquete* paquete = malloc(sizeof(t_paquete));
			paquete->codigo_operacion = LOCALIZED_POKEMON;
			paquete->buffer = malloc(sizeof(t_stream));
			paquete->buffer->size = totalBytes;
			paquete->buffer->data = stream;

			printf("Termine de armar el paquete \n");

			int totalBuffer = paquete->buffer->size + (2 * (sizeof(int)));

			printf("El total del tamanio de a_enviar es: %d \n", totalBuffer);

			//	void* a_enviar = serializar_paquete(paquete, totalBuffer);

			printf("Termine de serializar paquete \n");

			//send(socket_cliente, a_enviar, totalBuffer, MSG_WAITALL);
			//enviar_mensaje_a_broker(paquete, totalBuffer);
			printf("Enviado el paquete \n");

			//	free(a_enviar);

			printf("Liberado a_enviar \n");

			free(stream);
			free(paquete->buffer);
			free(paquete);

			printf("Liberado el paquete \n");

			liberar_lista_posiciones(listaPosiciones);

			printf("Destruida la lista \n");

			txt_close_file(posiciones);

			config_destroy(config);
		}

		free(line);

	} else {
		//Asigno 0 haciendo referencia a que no encontro el archivo
		*encontroPokemon = 0;
	}

	free(ruta);
}

*/

/*
 void eliminar_paquete(t_paquete* paquete) {
 free(paquete->buffer->data);
 free(paquete->buffer);
 free(paquete);
 }
 */
int tamano_recibido(int bytes) {
	return bytes + 2 * sizeof(int);
}

/**
 * Verificar si las posiciones ya existen dentro del archivo.
 * En caso de existir se deben agregar la cantidad pasada por parámetro a la actual.
 * En caso de no existir se debe agregar al final del archivo una nueva línea indicando
 * la cantidad de pokémon pasadas.
 * Cerrar el archivo.

void verificarPokemon(char* pathPokemon, t_posicion_pokemon* posicion) {

	int encontroPosicion = 0;
	char *line_buf = NULL;
	size_t line_buf_size = 0;
	int line_count = 1, line_size;

	struct stat st = { 0 };
	if (stat(pathPokemon, &st) == -1) {
		//El pokemon no existe

		FILE* fd = fopen(pathPokemon, "w");
		fprintf(fd, "%d-%d=%d\n", posicion->pos_x, posicion->pos_y,
				posicion->cantidad);
		fclose(fd);
	} else {
		FILE* fd = fopen(pathPokemon, "r+");
		if (fd != NULL) {
			line_size = getline(&line_buf, &line_buf_size, fd);

			while (line_size >= 0) {

				char** keyValue = string_split(line_buf, "=");
				char* key = keyValue[0];
				int cantidad = atoi(keyValue[1]);
				char** posiciones = string_split(key, "-");
				int posicionX = atoi(posiciones[0]);
				int posicionY = atoi(posiciones[1]);

				if (posicionX == posicion->pos_x
						&& posicionY == posicion->pos_y) {
					encontroPosicion = 1;
					cantidad += posicion->cantidad;
					if (fseek(fd, -line_size, SEEK_CUR) == 0) {
						fprintf(fd, "%d-%d=%d\n", posicionX, posicionY,
								cantidad);
						log_info(g_logger,
								"(POKEMON_POSITION_FOUND: %d-%d| UPDATE_COUNT= %d )",
								posicionX, posicionY, cantidad);
						fflush(fd);
					}
				}
				line_size = getline(&line_buf, &line_buf_size, fd);

			}
			if (!encontroPosicion) {
				fprintf(fd, "%d-%d=%d\n", posicion->pos_x, posicion->pos_y,
						posicion->cantidad);
				log_info(g_logger,
						"(POKEMON_POSITION_NOT_FOUND| CREATED_POKEMON: %d-%d=%d )",
						posicion->pos_x, posicion->pos_y, posicion->cantidad);
			}
			free(line_buf);

		}
		fclose(fd);
	}

}
*/
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

/**
 * Crea los semaforos para los pokemon que existen en el sistema
 */
void crear_semaforos_pokemon(){
	t_list* lista_pokemon =  get_files_pokemon();
	for( int i=0; i< list_size(lista_pokemon); i++ ){
		crear_semaforo_pokemon(list_get(lista_pokemon,i));
	}
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

void prueba_semaforo() {
	log_info(g_logger, "INICIA LAS PRUEBAS \n");
	int size = list_size(semaforos_pokemon);
	log_info(g_logger, "LA LISTA ARRANCA CON %d POKEMONES \n", size);
	char* pokemon = "Pildachu";
	log_info(g_logger, "SE AGREGA EL POKEMON PILDACHU \n");
	crear_semaforo_pokemon(pokemon);
	size = list_size(semaforos_pokemon);
	log_info(g_logger, " LA LISTA AHORA TIENE %d POKEMONES \n", size);
	char *pokemon2 = "Charmander";
	char *pokemon3 = "Bulbasaur";
	char *pokemon4 = "Ratata";
	crear_semaforo_pokemon(pokemon2);
	crear_semaforo_pokemon(pokemon3);
	crear_semaforo_pokemon(pokemon4);
	int sizeActualizado;
	sizeActualizado = list_size(semaforos_pokemon);
	log_info(g_logger,
			"SE AGREGAN 3 POKEMONS Y AHORA LA LISTA TIENE %d POKEMONES \n",
			sizeActualizado);

	log_info(g_logger, "MUESTRO TODOS LOS POKEMONS EN LA LISTA \n");
	for (int i = 0; i < list_size(semaforos_pokemon); i++) {
		t_pokemon_semaforo *pok = list_get(semaforos_pokemon, i);
		log_info(g_logger, "POKEMON : %s ", pok->pokemon);
	}

	eliminar_semaforo_pokemon(pokemon4);
	log_info(g_logger, "ELIMINO A RATATA Y MUESTRO LOS POKEMONES RESTANTES");
	for (int i = 0; i < list_size(semaforos_pokemon); i++) {
		t_pokemon_semaforo *pok = list_get(semaforos_pokemon, i);
		log_info(g_logger, "POKEMON : %s ", pok->pokemon);
	}
	eliminar_semaforo_pokemon(pokemon);
	eliminar_semaforo_pokemon(pokemon2);
	eliminar_semaforo_pokemon(pokemon3);

	log_info(g_logger, "ELIMINO A TODOS LOS POKEMONES Y QUEDAN %d EN LA LISTA",
			list_size(semaforos_pokemon));
}

char *concatenar_posiciones_pokemon(t_list *posiciones)
{
	int cant_elem = posiciones->elements_count;
	char *cadena = calloc(1000, sizeof(char));
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
		char auxiliar[4];
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
	return codigo_operacion == NEW_GAMECARD || codigo_operacion == CATCH_GAMECARD || codigo_operacion == GET_GAMECARD;
}


void rcv_new_pokemon(t_msg_new_gamecard *msg)
{
	log_info(g_logger,"PROCESANDO NEW POKEMON");
	t_list* lista_posiciones = list_create();
	t_posicion_pokemon *posicion = malloc(sizeof(t_posicion_pokemon));
	posicion->pos_x = msg->coord->pos_x;
	posicion->pos_y = msg->coord->pos_y;
	posicion->cantidad = msg->cantidad;
	int incrementar_posicion = 1;
	lista_posiciones =obtener_posiciones_pokemon(msg->pokemon, posicion,  incrementar_posicion);

//  TODO: SI LA LISTA ESTA VACIA no retorna apparead pokemon

	file_system_pokemon(msg->pokemon, lista_posiciones);
	//destruir_lista_posiciones_bloques(lista_posiciones);

/*
t_pokemon_semaforo *semaforo_pokemon = obtener_semaforo_pokemon(msg->pokemon);
if(semaforo_pokemon == NULL){
	// SI ES NULL, EL POKEMON NO ESTÁ CREADO EN EL FS
	t_posicion_pokemon *posicion = malloc(sizeof(t_posicion_pokemon));
	posicion->pos_x = msg->coord->pos_x;
	posicion->pos_y = msg->coord->pos_y;
	posicion->cantidad = msg->cantidad;
	t_list* lista_posiciones = list_create();
	list_add(lista_posiciones, posicion);
	file_system_pokemon(msg->pokemon, lista_posiciones);
	crear_semaforo_pokemon(msg->pokemon);
} else {
	sem_wait(&semaforo_pokemon->semaforo);
	// SE OBTIENE EL ARCHIVO
	// SE PREGUNTA POR EL "OPEN" Y SE LIBERA EL SEMAFORO
	lista_posiciones = obtener_posiciones_pokemon(msg);
	sem_post(&semaforo_pokemon->semaforo);
	// UNA VEZ SETEADO EL OPEN EN "Y", SE PROCEDE A ACTUALIZAR EL ARCHIVO
}
*/
}

t_result_caught codificar_resultado_caught(char *valor) {
	if (strcmp(valor, "OK") == 0) {
		return OK;
	} else {
		return FAIL;
	}
}

/**
 * Se retorna la respuesta al catch
 *
 */
void devolver_caught_pokemon(t_msg_catch_gamecard *msg, int socket_cliente)
{

	t_list* lista_posiciones = list_create();
	int incrementar_posicion = 0;
	char *arg_resul_caught = (char*) calloc(2, sizeof(char));

	t_posicion_pokemon *posicion = malloc(sizeof(t_posicion_pokemon));
	posicion->pos_x = msg->coord->pos_x;
	posicion->pos_y = msg->coord->pos_y;
	posicion->cantidad = 0;

	lista_posiciones = obtener_posiciones_pokemon(msg->pokemon, posicion,incrementar_posicion);

	if( list_size(lista_posiciones)>0 ){
		arg_resul_caught ="OK";
	}

	t_msg_caught_broker *msg_caught = malloc(sizeof(t_msg_caught_broker));
	msg_caught->id_correlativo = msg->id_mensaje;
	msg_caught->resultado = codificar_resultado_caught(arg_resul_caught);
	enviar_msj_caught_broker(socket_cliente, g_logger, msg_caught);
	file_system_pokemon(msg->pokemon, lista_posiciones);
	free(msg_caught);


	//list_destroy(lista_posiciones);
/*
	t_pokemon_semaforo* sem_pok = obtener_semaforo_pokemon(msg->pokemon);

	if(sem_pok == NULL){
		//SI NO EXISTE, SE DEBE RETORNAR UN MENSAJE DE ERROR
	} else {
		sem_wait(&sem_pok->semaforo);
		// ABRIR ARCHIVO
		// CONSULTAR POSICIONES
		// SI NO EXISTEN, SE RETORNA UN ERROR
		// SI EXISTEN, SE DECREMENTA EN 1 CADA POSICION SOLICITADA
		// SI CANTIDAD = 1 => ELIMINAR LINEA
		sem_post(&sem_pok->semaforo);
	}
	*/
}

void devolver_appeared_pokemon(t_msg_new_gamecard *msg, int socket_cliente)
{
	log_info(g_logger,"INICIO RETORNO APPARED POKEMON");
	t_msg_appeared_broker *msg_appeared = malloc(sizeof(t_msg_appeared_broker));
	msg_appeared->coordenada = malloc(sizeof(t_coordenada));
	char *pokemon = msg->pokemon;
	msg_appeared->coordenada->pos_x = msg->coord->pos_x;
	msg_appeared->coordenada->pos_y = msg->coord->pos_y;
	msg_appeared->id_correlativo = msg->id_mensaje;
	msg_appeared->size_pokemon = strlen(pokemon) + 1;
	msg_appeared->pokemon = malloc(msg->size_pokemon);
	memcpy(msg_appeared->pokemon, pokemon, msg_appeared->size_pokemon);
	enviar_msj_appeared_broker(socket_cliente, g_logger, msg_appeared);
	eliminar_msg_appeared_broker(msg_appeared);
}

/**
 * Retorna las posiciones actualizadas del pokemon.
 * Si es el primer new pokemon, retorna la posicion recibida
 * Caso contrario retorna la lista de posiciones actualizada
 *     La posicion No Existe => se agrega la posicion
 *     La posicion Existe => Se actualiza la cantidad
 *
 */
t_list* obtener_posiciones_pokemon(char* pokemon, t_posicion_pokemon *posicion, int incrementar_posicion)
{


//TODO SI es incrementar_posicion y la cantidad == 0 => retornar lista vacia

	t_list* lista_posiciones = list_create();

	t_pokemon_semaforo *semaforo_pokemon = obtener_semaforo_pokemon(pokemon);
	if(semaforo_pokemon == NULL){
		//No existe  el pokemon
		//	=> Se crea el semaforo
		//     se retorna la unica posicion recibida
		crear_semaforo_pokemon(pokemon);
		list_add(lista_posiciones, posicion);
		return lista_posiciones;
	}
	else {

		sem_wait(&semaforo_pokemon->semaforo);
//TODO
		// SE OBTIENE EL ARCHIVO
		// SE PREGUNTA POR EL "OPEN" Y SE LIBERA EL SEMAFORO

		sem_post(&semaforo_pokemon->semaforo);
		// UNA VEZ SETEADO EL OPEN EN "Y", SE PROCEDE A ACTUALIZAR EL ARCHIVO
		//Leer las posiciones
		//Buscar si la posicion recibida se encuetra
		//Se encuentra => Actualizar
		//No se encuentra => Agregar
		//Retornar posiciones

		t_list* lista_posiciones_bloques = list_create();
		lista_posiciones_bloques = leer_bloques(pokemon);
		int encontrado = 0;
		for( int i = 0; i< list_size(lista_posiciones_bloques); i ++){
			t_posicion_pokemon *posicion_bloque = (t_posicion_pokemon*) list_get(lista_posiciones,i);
			if( posicion_bloque->pos_x == posicion->pos_x &&
				posicion_bloque->pos_y == posicion->pos_y ){
					//Se encontro el pokemon
					encontrado = 1;

					if( incrementar_posicion ){
						posicion_bloque->cantidad += posicion->cantidad ;
						//msg->cantidad += posicion_bloque->cantidad;
						list_add(lista_posiciones_bloques, posicion_bloque);
					}
					else if( posicion_bloque->cantidad > 1){
						posicion_bloque->cantidad -= 1 ;
						list_add(lista_posiciones_bloques, posicion_bloque);
					}
			}
		}
		if( encontrado == 0 ){
			//No se encontro la posicion que se paso
			// => Se agrega al final de la lista
			list_add(lista_posiciones_bloques, posicion);
		}

		return lista_posiciones_bloques;
	}
}


void rcv_get_pokemon(t_msg_get_gamecard *msg, int socket_cliente)
{	// TODO
	/*Al recibir este mensaje se deberán realizar las siguientes operaciones:
	1. 	Verificar si el Pokémon existe dentro de nuestro Filesystem. Para esto se deberá buscar
		dentro del directorio Pokemon, si existe el archivo con el nombre de nuestro pokémon. En
		caso de no existir se deberá informar el mensaje sin posiciones ni cantidades.
	2. 	Verificar si se puede abrir el archivo (si no hay otro proceso que lo esté abriendo). En caso
		que el archivo se encuentre abierto se deberá reintentar la operación luego de un tiempo
		definido por configuración.
	3. 	Obtener todas las posiciones y cantidades de Pokemon requerido.
	4. 	Esperar la cantidad de segundos definidos por archivo de configuración
	5. 	Cerrar el archivo.
	6. 	Conectarse al Broker y enviar el mensaje con todas las posiciones y su cantidad.
		En caso que se encuentre por lo menos una posición para el Pokémon solicitado se deberá enviar un
		mensaje al Broker a la Cola de Mensajes LOCALIZED_POKEMON indicando:
	3. 	ID del mensaje recibido originalmente.
	4. 	El Pokémon solicitado.
	5. 	La lista de posiciones y la cantidad de posiciones X e Y de cada una de ellas en el mapa.
		En caso que no se pueda realizar la conexión con el Broker se debe informar por logs y continuar la
		ejecución.
	 *
	 */

	t_list* lista_posiciones = list_create();

	t_pokemon_semaforo *semaforo_pokemon = obtener_semaforo_pokemon(msg->pokemon);
	t_msg_localized_broker *msg_localized_broker = malloc(sizeof(t_msg_localized_broker));
	msg_localized_broker->id_correlativo = msg->id_mensaje;

	if( semaforo_pokemon != NULL ){
		//SE obtienen todas las posiciones
		sem_wait(&semaforo_pokemon->semaforo);
//TODO
		// SE OBTIENE EL ARCHIVO
		// SE PREGUNTA POR EL "OPEN" Y SE LIBERA EL SEMAFORO
		sem_post(&semaforo_pokemon->semaforo);
		t_list* lista_posiciones_bloques = list_create();
		lista_posiciones_bloques = leer_bloques(msg->pokemon);
		msg_localized_broker->posiciones->coordenadas = lista_posiciones_bloques;
		msg_localized_broker->posiciones->cant_posic = list_size(lista_posiciones_bloques);
	}
	enviar_msj_localized_broker(socket_cliente, g_logger, msg_localized_broker);
	eliminar_msg_localized_broker(msg_localized_broker);
}


/**
 * Se queda esperando para leer el pokemoon
 * Cuando pueda leer
 * Si open = N =>  SETEA EL OPEN EN "Y"
 * Libera el semaforo
 */
void mientras_no_pueda_leer_pokemon(t_pokemon_semaforo *semaforo_pokemon){
	sem_wait(&semaforo_pokemon->semaforo);
//TODO
	// SE OBTIENE EL ARCHIVO
	// SE PREGUNTA POR EL "OPEN" Y SE LIBERA EL SEMAFORO

	sem_post(&semaforo_pokemon->semaforo);
}




