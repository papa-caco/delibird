/*
 * tests_tall_grass.c
 *
 *  Created on: 19 jul. 2020
 *      Author: utnso
 */

#include "tall_grass.h"

int leer_bloques(char *pokemon, t_log *logger);

void prueba_file_system(char* pokemon, int cant_posiciones);

void prueba_leer_bloques_pokemon(char* pokemon);

void file_system_pokemon(char *pokemon, int cant_posiciones);

void rcv_new_pokemon(t_msg_new_gamecard *msg);

void devolver_posiciones(int socket_cliente, char* pokemon,	int* encontroPokemon);

void prueba_semaforo(void);

void verificarPokemon(char* pathPokemon,t_posicion_pokemon* posicion);

//-------------------------------------------------------------------//


int leer_bloques(char *pokemon, t_log *logger)
{	//lee los bloques que se corresponden a un archivo pokemon y lo retorna en una lista de posiciones
	pthread_mutex_lock(&g_mutex_envio); //lee los bloques que se corresponden a un archivo pokemon
	log_info(logger,"READ FILE %s",pokemon); // y lo retorna en una lista de posiciones
	int data_size = 0, id_msj = 0;
	t_pokemon_medatada *pokemon_metadata = leer_metadata_pokemon(pokemon, logger);
	if (pokemon_metadata != NULL) {
	char *string_posiciones = get_contenido_bloques(pokemon_metadata);
	t_list *lista_posiciones = obtener_posiciones(string_posiciones, pokemon_metadata->size);
	pthread_mutex_unlock(&g_mutex_envio);
	t_list *coordenadas = list_create();
	for( int i = 0; i< list_size(lista_posiciones); i ++){
		t_posicion_pokemon *posicion = (t_posicion_pokemon*) list_get(lista_posiciones,i);
		id_msj = enviar_appeared_pokemon_broker(pokemon, posicion->cantidad, posicion->pos_x, posicion->pos_y, logger);
		if (nro_par(posicion->pos_x)) {
			id_msj = enviar_caught_pokemon_broker(posicion->pos_x, OK, logger);
		} else {
			id_msj = enviar_caught_pokemon_broker(posicion->pos_x, FAIL, logger);
		}
		t_coordenada *coordenada = malloc(sizeof(t_coordenada));
		coordenada->pos_x = posicion->pos_x;
		coordenada->pos_y = posicion->pos_y;
		void *coord = (t_coordenada*) coordenada;
		list_add(coordenadas, coord);
	}
	pthread_mutex_lock(&g_mutex_envio);
	log_trace(logger,"END READ FILE POKEMON %s", pokemon);
	pthread_mutex_unlock(&g_mutex_envio);
	data_size = pokemon_metadata->size;
	list_destroy_and_destroy_elements(pokemon_metadata->blocks,(void*) free);
	free(pokemon_metadata);
	free(string_posiciones);
	list_destroy_and_destroy_elements(lista_posiciones,(void*) free);
	return data_size;
	} else {
		return -1;
	}
}

void prueba_file_system(char* pokemon, int cant_posiciones)
{
	pthread_mutex_lock(&g_mutex_envio);
	file_system_pokemon(pokemon, cant_posiciones);
	log_info(g_logger,"FIN PRUEBA GUARDAR ARCHIVO\n");
	pthread_mutex_unlock(&g_mutex_envio);
}

void prueba_leer_bloques_pokemon(char* pokemon)
{
	 leer_bloques(pokemon, g_logger);
}


/**
 *crea el filesystem del pokeno: metadata.bin y bloque.bin
 * 1 Armar un string, del tamanio del bloque con todas las lista de posiciones que entren
 * 2 Conforme se llena string se guarda en el bloque
 */

void file_system_pokemon(char *pokemon, int cant_posiciones)
{  // -->>Esta vuela: las posiciones las tiene que cargar/actualizar de a una <<--
	log_trace(g_logger,"CREATE FILESYSTEM %s",pokemon);
	int lng_str_posiciones = 0;
	char *string_posiciones;
	t_list* lista_posiciones = list_create(), *blocks;
	for (int i = 0; i < cant_posiciones; i ++) {
		t_posicion_pokemon *posicion = malloc(sizeof(t_posicion_pokemon));
		posicion->pos_x = 15 + i * 3;
		posicion->pos_y = 20 * i;
		posicion->cantidad = (i +1) * 100;
		void *posi = (t_posicion_pokemon*) posicion;
		list_add(lista_posiciones, posicion);
	}
	string_posiciones = serializar_lista_posiciones_pokemon(lista_posiciones, g_logger);
	lng_str_posiciones = string_length(string_posiciones);
	blocks = armar_guardar_data_bloques_file_pokemon(string_posiciones);
	grabar_metadata_pokemon(blocks, pokemon, lng_str_posiciones,"N", g_logger);
	list_destroy(blocks);
	list_destroy_and_destroy_elements(lista_posiciones,(void*) free);
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

void rcv_new_pokemon(t_msg_new_gamecard *msg)
{/* 1.  Verificar si el Pokémon existe dentro de nuestro Filesystem. Para esto se deberá buscar
		dentro del directorio Pokemon si existe el archivo con el nombre de nuestro pokémon. En
		caso de no existir se deberá crear.
	2.  Verificar si se puede abrir el archivo (si no hay otro proceso que lo esté abriendo). En caso
		que el archivo se encuentre abierto se deberá reintentar la operación luego de un tiempo
		definido en el archivo de configuración.
	3.  Verificar si las posiciones ya existen dentro del archivo. En caso de existir, se deben agregar
		la cantidad pasada por parámetro a la actual. En caso de no existir se debe agregar al final
		del archivo una nueva línea indicando la cantidad de pokémon pasadas.
	4.  Esperar la cantidad de segundos definidos por archivo de configuración
	5.  Cerrar el archivo.
	6.  Conectarse al Broker y enviar el mensaje a la Cola de Mensajes APPEARED_POKEMON con
		los los datos:
		○ ID del mensaje recibido.
		○ Pokemon.
		○ Posición del mapa.
		En caso que no se pueda realizar la conexión con el Broker se debe informar por logs y continuar la
		ejecución.*/
	t_pokemon_semaforo *semaforo_pokemon = obtener_semaforo_pokemon(msg->pokemon);
	// SI ES NULL, EL POKEMON NO ESTÁ CREADO EN EL FS
	if (msg->id_mensaje == GET_POKEMON) {
	if (semaforo_pokemon == NULL) {
			// DEVOLVER MENSAJES SIN POSICIONES NI CANTIDADES
	} else {
			// SI SE ENCUENTRA AL MENOS 1
			sem_wait(&semaforo_pokemon->semaforo);
			// RETORNAR TODAS LAS POSICIONES A LA COLA LOCALIZED_POKEMON DEL BROKER
			sem_post(&semaforo_pokemon->semaforo);
		}
	} else {
			// ESTE ES EL CASO DE SI ES UN NEW POKEMON
		if (semaforo_pokemon == NULL) {
			// LOGICA DE CREACION DE NUEVO ARCHIVO EN EL FS
			crear_semaforo_pokemon(msg->pokemon);
		} else {
			sem_wait(&semaforo_pokemon->semaforo);
			// SE OBTIENE EL ARCHIVO
			// SE PREGUNTA POR EL "OPEN" Y SE LIBERA EL SEMAFORO
			sem_post(&semaforo_pokemon->semaforo);
			// UNA VEZ SETEADO EL OPEN EN "Y", SE PROCEDE A ACTUALIZAR EL ARCHIVO
		}
		t_posicion_pokemon *posicion = malloc(sizeof(t_posicion_pokemon));
		posicion->pos_x = msg->coord->pos_x;
		posicion->pos_y = msg->coord->pos_y;
		posicion->cantidad = msg->cantidad;
		///Guarda la informacion en el FS
		char* pathPokemon = malloc(strlen(g_config_gc->path_tall_grass) + 8); //9= /pokemon/; 3 = .txt
		strcpy(pathPokemon, g_config_gc->path_tall_grass);
		strcat(pathPokemon, "/Pokemon");
		struct stat st = { 0 };
		if (stat(pathPokemon, &st) == -1) {
			log_error(g_logger, "CREATE_DIR Pokemon");
			mkdir(pathPokemon, 0774);
		}
		pathPokemon = realloc(pathPokemon,
				strlen(pathPokemon) + strlen(msg->pokemon) + 4); //9= /pokemon/; 3 = .txt
		strcat(pathPokemon, "/");
		strcat(pathPokemon, msg->pokemon);
		strcat(pathPokemon, ".txt");
		verificarPokemon(pathPokemon, posicion);
		free(pathPokemon);
		free(posicion);
		/*	log_info(g_logger, "(ID-MSG= %d | %s | %d | %d | %d -- SIZE = %d Bytes)",
		 msg->id_mensaje, msg->pokemon, msg->coord->pos_x, msg->coord->pos_y,
		 msg->cantidad);
		 */
	}
}

void devolver_caught_pokemon(t_msg_catch_gamecard *msg, int socket_cliente)
{
	uint32_t id_mensaje = msg->id_mensaje;

	t_pokemon_semaforo* sem_pok = obtener_semaforo_pokemon(msg->pokemon);

	//SI NO EXISTE, SE DEBE RETORNAR UN MENSAJE DE ERROR
	if(sem_pok == NULL){
		//enviar_msj_caught_broker(cliente_fd, logger, msg_caught_broker);
	} else {
		sem_wait(&sem_pok->semaforo);
		// ABRIR ARCHIVO
		// CONSULTAR POSICIONES
		// SI NO EXISTEN, SE RETORNA UN ERROR
		// SI EXISTEN, SE DECREMENTA EN 1 CADA POSICION SOLICITADA
		// SI CANTIDAD = 1 => ELIMINAR LINEA
		sem_post(&sem_pok->semaforo);
	}

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = CAUGHT_BROKER; //Luego debe ser CAUGHT_POKEMON
	paquete->buffer = malloc(sizeof(t_stream));
	paquete->buffer->size = 2 * sizeof(int);
	void *stream = malloc(paquete->buffer->size);

	int offset = 0;
	memcpy(stream + offset, msg, sizeof(int)); // Tomamos el id_mensaje Recibido en CATCH_GAMECARD
	t_result_caught resultado = OK;
	offset += sizeof(t_result_caught);
	memcpy(stream + offset, &(resultado), sizeof(t_result_caught));
	paquete->buffer->data = stream;

	int bytes = paquete->buffer->size + 2 * sizeof(int);
//	void* a_enviar = serializar_paquete(paquete, bytes);

	//enviar_mensaje_a_broker(paquetes
	/*	void* a_enviar = serializar_paquete(paquete, bytes);
	 send(socket_cliente, a_enviar, bytes, 0);
	 free(a_enviar);
	 eliminar_paquete(paquete);*/
}

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
			enviar_mensaje_error(socket_cliente, g_logger,
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

/* Verificar si las posiciones ya existen dentro del archivo.
 * En caso de existir se deben agregar la cantidad pasada por parámetro a la actual.
 * En caso de no existir se debe agregar al final del archivo una nueva línea indicando
 * la cantidad de pokémon pasadas.
 * Cerrar el archivo.
 */
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
				/* printf("line[%06d]: chars=%06zd, buf size=%06zu, contents: %s", line_count,
				 line_size, line_buf_size, line_buf);*/
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
