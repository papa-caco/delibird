/*
 * tallGrass.c
 *
 *
 *      Author: utnso
 */

#include "tall_grass.h"

char *serializar_lista_posiciones_pokemon(t_list *lista_posiciones, t_log *logger)
{
	int size_posicion = 0, cant_posiciones = lista_posiciones->elements_count;
	char *string_posiciones;
	if (cant_posiciones > 0){
		for(int i = 0; i < lista_posiciones->elements_count; i ++) {
			t_posicion_pokemon  *posicion = list_get(lista_posiciones, i);
			char *pos_x = string_itoa(posicion->pos_x);
			char *pos_y = string_itoa(posicion->pos_y);
			char *cantidad = string_itoa(posicion->cantidad);
			string_append(&pos_x,"-");
			string_append(&pos_y,"=");
			string_append(&cantidad,"\n");
			size_posicion += string_length(pos_x) + string_length(pos_y)+string_length(cantidad);
			if (i == 0) {
				string_posiciones = (char*) calloc(size_posicion, sizeof(char));
			} else {
				string_posiciones = realloc(string_posiciones, size_posicion);
			}
			string_append(&string_posiciones, pos_x);
			string_append(&string_posiciones, pos_y);
			string_append(&string_posiciones, cantidad);
			free(pos_x);
			free(pos_y);
			free(cantidad);
			}
			char *concatenada = concatenar_posiciones_pokemon(lista_posiciones);
			log_info(logger,"(Guardando Posiciones: %s)", concatenada);
			free(concatenada);
	} else {
		string_posiciones = "";
	}
	return string_posiciones;
}

char *serializar_posicion_pokemon(t_coordenada *coordenada, uint32_t cant)
{
	char *string_posicion, *pos_x, *pos_y, *cantidad;
	int size_posicion =0;
	pos_x = string_itoa(coordenada->pos_x);
	pos_y = string_itoa(coordenada->pos_y);
	cantidad = string_itoa(cant);
	string_append(&pos_x,"-");
	string_append(&pos_y,"=");
	string_append(&cantidad,"\n");
	size_posicion += string_length(pos_x) + string_length(pos_y)+string_length(cantidad);
	string_posicion = (char*) calloc(size_posicion, sizeof(char));
	string_append(&string_posicion, pos_x);
	string_append(&string_posicion, pos_y);
	string_append(&string_posicion, cantidad);
	free(pos_x);
	free(pos_y);
	free(cantidad);
	return string_posicion;
}

t_list *armar_guardar_data_bloques_file_pokemon(char *string_posiciones)
{
	int tamano_bloque = g_config_tg->block_size, comienzo = 0, excedente = 0, cant_bloques = 1,
		tamano_string = string_length(string_posiciones), tamano_grabar = 0;
	t_list *blocks = list_create();
	if (tamano_string >= tamano_bloque) {
		cant_bloques = (int) tamano_string / tamano_bloque;
		int resto = (int) (tamano_string % tamano_bloque);
		if (resto > 0) {
			cant_bloques ++;
		}
		for (int i = 0; i < cant_bloques; i ++) {
			char *grabar;
			if (i < cant_bloques - 1) {
				grabar = string_substring(string_posiciones, comienzo, tamano_bloque);
				tamano_grabar = tamano_bloque;
			} else {
				excedente =  tamano_string - comienzo;
				grabar = string_substring(string_posiciones, comienzo, excedente);
				tamano_grabar = excedente;
			}
			int *bloque_nro = malloc(sizeof(int));
			(*bloque_nro) = grabar_bloque(grabar, tamano_grabar, g_logdebug);
			list_add(blocks, bloque_nro);
			comienzo += tamano_bloque;
		}
		free(string_posiciones);
	} else {
		int *bloque_nro = malloc(sizeof(int));
		(*bloque_nro) = grabar_bloque(string_posiciones, tamano_string, g_logdebug);
		list_add(blocks,bloque_nro);
	}
	return blocks;
}

int grabar_bloque(char *block_buffer, size_t size, t_log *logger)
{	//Graba un bloque en el file system
	int block_nro = obtener_ultimo_nro_bloque();
	if (block_nro != -1) {
		char *file_block = string_itoa(block_nro);
		char *path_blocks = obtengo_path_bloque(file_block);
		FILE* fd = fopen(path_blocks,"w");
		fwrite(block_buffer,sizeof(char), size, fd);
		fclose(fd);
		log_info(logger,"SAVE BLOCK | FILE %d.bin", block_nro);
		free(file_block);
		free(path_blocks);
	} else {
		puts("");
		log_error(logger, "-->>!!Sin Bloques Disponibles en File System TALL_GRASS - Cerrar GAMECARD!! <<--");
		pthread_mutex_lock(&g_mutex_envio);
		pthread_exit(NULL);
	}
	free(block_buffer);
	return block_nro;
}

void eliminar_bloques_archivo(t_archivo_pokemon *archivo)
{
	int cant_bloques = archivo->metadata->blocks->elements_count;
	for (int i = 0; i < cant_bloques; i ++) {
		char *elem = (char*)list_get(archivo->metadata->blocks, i);
		eliminar_bloque(elem);
	}
	list_destroy_and_destroy_elements(archivo->metadata->blocks, (void*) free);
	archivo->metadata->blocks = list_create();
}

void eliminar_bloque(char *bloque)
{
	int block_nro = atoi(bloque);
	char *path_bloque = obtengo_path_bloque(bloque);
	remove(path_bloque);
	liberar_nro_bloque_bitmap(block_nro);
	free(path_bloque);
}

char *obtengo_path_bloque(char * file_block)
{
	char *path_block = string_new();
	string_append(&path_block, g_config_gc->dirname_blocks);
	string_append(&path_block, file_block);
	string_append(&path_block, ".bin");
	return path_block;
}

void grabar_metadata_pokemon(t_list *blocks, char *pokemon, int file_size, char *status,  t_log *logger)
{	//Graba la metadata del pokemon(Metadata.bin)
	crear_dirname_pokemon(pokemon, g_logger);
	int long_dir = string_length(g_config_gc->dirname_files) + string_length(pokemon) + string_length("/Metadata.bin");
	char* dirname_metatada_pokemon = (char*) calloc(long_dir, sizeof(char));
	string_append(&dirname_metatada_pokemon,g_config_gc->dirname_files);
	string_append(&dirname_metatada_pokemon,pokemon);
	string_append(&dirname_metatada_pokemon,"/Metadata.bin");
	FILE* fd = fopen(dirname_metatada_pokemon,"wb");
	fprintf(fd, "DIRECTORY=N\n");
	fprintf(fd, "SIZE=%d\n", file_size);
	char *string_lista = generar_string_bloques_metadata(blocks);
	int size_string = string_length("BLOCKS=") + string_length(string_lista) + 1;
	char *string_blocks = calloc(size_string, sizeof(char));
	string_append(&string_blocks, "BLOCKS=");
	string_append(&string_blocks, string_lista);
	string_append(&string_blocks, "\n");
	fprintf(fd, "%s", string_blocks);
	fprintf(fd, "OPEN=%s\n", status);
	fclose(fd);
	log_info(logger,"SAVE METADATA  %s",dirname_metatada_pokemon);
	free(string_lista);
	free(string_blocks);
	free(dirname_metatada_pokemon);
}

char *generar_string_bloques_metadata(t_list * blocks)
{
	int size_string = string_length("["), cantidad_bloques = blocks->elements_count;
	char *string_bloques = calloc(size_string, sizeof(char));
	string_append(&string_bloques,"[");
	for(int i = 0 ; i < cantidad_bloques;  i++ ){
		int *nro_bloque = (int*)list_get(blocks, i);
		char *bloque = string_itoa(*nro_bloque);
		size_string += string_length(bloque);
		string_bloques = realloc(string_bloques, size_string + 1);
		string_append(&string_bloques, bloque);
		free(bloque);
		if(i < cantidad_bloques - 1) {
			string_append(&string_bloques, ",");
		} else {
			string_append(&string_bloques,"]");
		}
	}
	return string_bloques;
}

void crear_dirname_pokemon(char *pokemon, t_log *logger)
{
	char* pathPokemon = string_new();
	string_append(&pathPokemon, g_config_gc->dirname_files);
	string_append(&pathPokemon, pokemon);
	struct stat st = { 0 };
	if (stat(pathPokemon, &st) == -1) {
		log_info(g_logger, "CREATE_DIR Pokemon %s",pathPokemon);
		mkdir(pathPokemon, 0774);
	}
	free(pathPokemon);
}

void leer_metadata_tall_grass(t_log *logger)
{
	crear_estructura_tallgrass(logger);
	if (!estructura_file_system_tg_valida(logger)) {
		exit(EXIT_FAILURE);
	}
	int size_path_metadata = strlen(g_config_gc->path_tall_grass) + strlen("metadata/Metadata.bin");
	char *ruta_metadata_bin = (char*) calloc(size_path_metadata, sizeof(char));
	string_append(&ruta_metadata_bin, g_config_gc->path_tall_grass);
	string_append(&ruta_metadata_bin,"metadata/Metadata.bin");
	t_config*config = config_create(ruta_metadata_bin);
	g_config_tg = malloc(sizeof(t_config_tall_grass));
	g_config_tg->block_size = config_get_int_value(config,"BLOCK_SIZE");
	g_config_tg->blocks = config_get_int_value(config,"BLOCKS");
	g_config_tg->magic_number = config_get_string_value(config,"MAGIC_NUMBER");
	g_config_tg->tamano_fs = g_config_tg->blocks * g_config_tg->block_size;
	int tamano = (int) g_config_tg->tamano_fs / 1024;
	log_debug(logger, "(FileSystem TallGrass| Tam.Bloque:%d Bytes| Cant.Bloques:%d| Tamaño FS:%d Kbytes| Magic_Number:%s)\n",
			g_config_tg->block_size, g_config_tg->blocks, tamano, g_config_tg->magic_number );
	config_destroy(config);
	free(ruta_metadata_bin);
}

bool estructura_file_system_tg_valida(t_log *logger)
{
	bool resultado = false;
	int size_path_metadata = strlen(g_config_gc->path_tall_grass) + strlen("metadata/metadata.bin");
	char *ruta_metadata_bin = (char*) calloc(size_path_metadata, sizeof(char));
	string_append(&ruta_metadata_bin, g_config_gc->path_tall_grass);
	string_append(&ruta_metadata_bin,"metadata/Metadata.bin");
	t_config *metadata_bin = config_create(ruta_metadata_bin);
	if (config_has_property(metadata_bin, "MAGIC_NUMBER")) {
		char *directory = config_get_string_value(metadata_bin, "DIRECTORY");
		char *magic_number = config_get_string_value(metadata_bin, "MAGIC_NUMBER");
		char *block_size = config_get_string_value(metadata_bin, "BLOCK_SIZE");
		char *blocks = config_get_string_value(metadata_bin, "BLOCKS");
		int chk_magic_number = strcmp(magic_number, g_config_gc->magic_number);
		int chk_dir = strcmp(directory, "N");
		int chk_block_size = strcmp(block_size, g_config_gc->tg_block_size);
		int chk_blocks = strcmp(blocks, g_config_gc->tg_blocks);
		bool check_parametros = chk_magic_number == 0 && chk_dir == 0 && chk_block_size == 0 && chk_blocks == 0;
		if (check_parametros) {
			puts("");
			log_info(logger, "(Configuración File System Tall_Grass Consistente OK)\n");
			resultado = true;
		} else {
			puts("");
			log_error(logger, "(--->>Configuración File System Tall_Grass Inconsistente -- Revisar!!<<---)\n");
		}
	} else {
		puts("");
		log_error(logger, "(--->>Configuración File System Tall_Grass Erronea -- Se debe Reconstruir <<---)\n");
	}
	config_destroy(metadata_bin);
	free(ruta_metadata_bin);
	return resultado; //TODO
}

void crear_estructura_tallgrass(t_log *logger)
{
	int size_path_metadata = strlen(g_config_gc->path_tall_grass) + strlen("metadata/metadata.bin");
	char *ruta_metadata_bin = (char*) calloc(size_path_metadata, sizeof(char));
	string_append(&ruta_metadata_bin, g_config_gc->path_tall_grass);
	int size_path_config = strlen(g_config_gc->path_tall_grass) + strlen("Config_tg/Metadata.bin");
	char *ruta_config_tg = (char*) calloc(size_path_config, sizeof(char));
	string_append(&ruta_config_tg, g_config_gc->path_tall_grass);
	string_append(&ruta_config_tg, "Config_tg/Metadata.bin");
	int status = mkdir(ruta_metadata_bin, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	puts("");
	log_info(logger, "(Verificando Configuración File System Tall_Grass...)\n");
	if (status == -1) {
		log_info(logger,"(Mount Point Directory (%s)	-%s)", ruta_metadata_bin, strerror(errno));
	} else {
		log_info(logger,"(Creating Mount Point Directory (%s)	-%s)", ruta_metadata_bin, strerror(errno));
	}
	crear_directorios_estructura_tall_grass("metadata", logger);
	crear_directorios_estructura_tall_grass("Files", logger);
	crear_directorios_estructura_tall_grass("Blocks", logger);
	crear_directorios_estructura_tall_grass("Config_tg", logger);
	string_append(&ruta_metadata_bin,"metadata/Metadata.bin");
	t_config *metadata_bin = config_create(ruta_metadata_bin);
	if (!config_has_property(metadata_bin, "MAGIC_NUMBER")) {
		config_set_value(metadata_bin, "DIRECTORY", "N");
		config_set_value(metadata_bin, "BLOCK_SIZE", g_config_gc->tg_block_size);
		config_set_value(metadata_bin, "BLOCKS", g_config_gc->tg_blocks);
		config_set_value(metadata_bin, "MAGIC_NUMBER", g_config_gc->magic_number);
		config_save(metadata_bin);
	}
	int size_path_files = strlen(g_config_gc->path_tall_grass) + strlen("Files/");
	char *ruta_files = (char*) calloc(size_path_files, sizeof(char));
	string_append(&ruta_files, g_config_gc->path_tall_grass);
	string_append(&ruta_files, "Files/");
	int size_path_blocks = strlen(g_config_gc->path_tall_grass) + strlen("Blocks/");
	char *ruta_blocks = (char*) calloc(size_path_blocks, sizeof(char));
	string_append(&ruta_blocks, g_config_gc->path_tall_grass);
	string_append(&ruta_blocks, "Blocks/");
	int size_path_bitmap = strlen(g_config_gc->path_tall_grass) + strlen("metadata/bitmap.bin");
	char *ruta_bitmap = (char*) calloc(size_path_bitmap, sizeof(char));
	string_append(&ruta_bitmap, g_config_gc->path_tall_grass);
	string_append(&ruta_bitmap, "metadata/bitmap.bin");
	g_config_paths = config_create(ruta_config_tg);
	if (config_has_property(g_config_paths, "DIRECTORY")) {
		config_set_value(g_config_paths, "DIRECTORY", "N");
		config_set_value(g_config_paths, "PATH_BLOCKS", ruta_blocks);
		config_set_value(g_config_paths, "PATH_FILES", ruta_files);
		config_set_value(g_config_paths, "PATH_BITMAP", ruta_bitmap);
		config_save(g_config_paths);
	}
	g_config_gc->dirname_blocks = config_get_string_value(g_config_paths, "PATH_BLOCKS");
	g_config_gc->dirname_files = config_get_string_value(g_config_paths, "PATH_FILES");
	g_config_gc->ruta_bitmap = config_get_string_value(g_config_paths, "PATH_BITMAP");
	config_destroy(metadata_bin);
	free(ruta_metadata_bin);
	free(ruta_files);
	free(ruta_blocks);
	free(ruta_bitmap);
	free(ruta_config_tg);
}

void crear_directorios_estructura_tall_grass(char *path, t_log *logger)
{
	int size_path_files = strlen(g_config_gc->path_tall_grass)  + strlen(path) + strlen("/Metadata.bin")  ;
	char *ruta_files = (char*) calloc(size_path_files, sizeof(char));
		string_append(&ruta_files, g_config_gc->path_tall_grass);
	string_append(&ruta_files, path);
	int status = mkdir(ruta_files, 0774);
	if (status == -1) {
		log_info(logger,"(Directory (%s)	-%s)", ruta_files, strerror(errno));
	} else {
		log_info(logger,"(Creating Directory (%s))", ruta_files);
	}
	string_append(&ruta_files, "/Metadata.bin");
	int fd = open(ruta_files, O_RDONLY);
	if (fd < 0) {
		void *data = malloc(1);
		memset(data, 0, 1);
		FILE* metadata_file = fopen(ruta_files,"w");
		fwrite(data,sizeof(char), 1, metadata_file);
		fclose(metadata_file);
		free(data);
		log_info(logger,"(Creating File: %s	-Success)", ruta_files);
	} else {
		log_info(logger,"(File: %s	-File Exists)", ruta_files);
	}
	close(fd);
	t_config* metadata_files = config_create(ruta_files);
	if (!config_has_property(metadata_files, "DIRECTORY")) {
		config_set_value(metadata_files, "DIRECTORY", "Y");
		config_save(metadata_files);
	}
	config_destroy(metadata_files);
	free(ruta_files);
}

int crear_archivo_pokemon(char *pokemon, t_posicion_pokemon *posicion, t_log *logger)
{
	t_coordenada *coordenada = malloc(sizeof(t_coordenada));
	coordenada->pos_x = posicion->pos_x;
	coordenada->pos_y = posicion->pos_y;
	char *string_posicion = serializar_posicion_pokemon(coordenada, posicion->cantidad);
	int file_size = string_length(string_posicion);
	t_list *blocks = armar_guardar_data_bloques_file_pokemon(string_posicion);
	grabar_metadata_pokemon(blocks, pokemon, file_size, "N", logger);
	list_destroy_and_destroy_elements(blocks, (void*) free);
	free(coordenada);
	//free(posicion);//TODO
	//free(pokemon);
	return file_size;
}

t_archivo_pokemon *abrir_archivo_pokemon(char *pokemon, t_log *logger)
{
	t_archivo_pokemon *archivo = malloc(sizeof(t_archivo_pokemon));
	int size_pokemon = strlen(pokemon) + 1;
	archivo->pokemon = malloc(size_pokemon);
	memcpy(archivo->pokemon, pokemon, size_pokemon);
	modificar_metadata_pokemon(pokemon, "OPEN", "Y");
	pthread_mutex_lock(&g_mutex_open_files_list);
	list_add(g_archivos_abiertos, (void*) pokemon);
	pthread_mutex_unlock(&g_mutex_open_files_list);
	log_info(logger,"(Archivo Pokemon %s Abierto)", pokemon);
	archivo->metadata = leer_metadata_pokemon(pokemon, logger);
	if (archivo->metadata->size > 0) {
		char *string_posiciones = get_contenido_bloques(archivo->metadata);
		archivo->posiciones = obtener_posiciones(string_posiciones, archivo->metadata->size);
		free(string_posiciones);
	} else {
		archivo->posiciones = list_create();
	}
	return archivo;
}

void cerrar_archivo_pokemon(t_archivo_pokemon *archivo, t_log *logger)
{
	quitar_de_lista_archivos_abiertos(archivo->pokemon);
	modificar_metadata_pokemon(archivo->pokemon, "OPEN", "N");
	log_info(g_logger,"(Archivo Pokemon %s Cerrado)", archivo->metadata->archivo_pokemon);
	if (archivo->metadata->size > 0) {
	list_destroy_and_destroy_elements(archivo->posiciones, (void*) free);
	list_destroy_and_destroy_elements(archivo->metadata->blocks, (void*) free);
	} else {
		list_destroy(archivo->posiciones);
		list_destroy(archivo->metadata->blocks);
	}
	free(archivo->metadata->archivo_pokemon);
	free(archivo->metadata);
	free(archivo->pokemon);
	free(archivo);
}

int modificar_archivo_pokemon(t_archivo_pokemon *archivo, t_log *logger)
{
	eliminar_bloques_archivo(archivo);
	char* string_blocks, *tamano;
	char *string_posiciones = serializar_lista_posiciones_pokemon(archivo->posiciones, logger);
	archivo->metadata->size = string_length(string_posiciones);
	if (archivo->metadata->size > 0) {
		t_list *bloques = armar_guardar_data_bloques_file_pokemon(string_posiciones);
		list_add_all(archivo->metadata->blocks, bloques);
		list_destroy(bloques);
		string_blocks = generar_string_bloques_metadata(archivo->metadata->blocks);
		tamano = string_itoa(archivo->metadata->size);
		modificar_metadata_pokemon(archivo->pokemon, "BLOCKS",string_blocks);
		modificar_metadata_pokemon(archivo->pokemon, "SIZE", tamano);
		free(tamano);
		free(string_blocks);
	} else {
		modificar_metadata_pokemon(archivo->pokemon, "BLOCKS","[]");
		modificar_metadata_pokemon(archivo->pokemon, "SIZE", "0");
	}
	return archivo->posiciones->elements_count;
}

t_list *obtener_coordenadas_archivo_pokemon(t_archivo_pokemon *archivo, t_log *logger)
{	// Función utilizada para enviar mensajes LOCALIZED_POKEMON
	t_list *coordenadas = list_create();
	int cant_elem = archivo->posiciones->elements_count;
	for( int i = 0; i < cant_elem; i ++){
		t_posicion_pokemon *posicion = (t_posicion_pokemon*) list_get(archivo->posiciones, i);
		t_coordenada *coordenada = malloc(sizeof(t_coordenada));
		coordenada->pos_x = posicion->pos_x;
		coordenada->pos_y = posicion->pos_y;
		void *coord = (t_coordenada*) coordenada;
		list_add(coordenadas, coord);
	}
	log_info(logger, "(READING FILE CONTENT: %s | POSITIONS OBTAINED: %d )", archivo->metadata->archivo_pokemon, cant_elem);
	return coordenadas;
}

t_list *obtener_posiciones(char *string_posiciones, int long_string)
{	//Retorna una estructura de posiciones a partir de un string de posiciones
	t_list *lista_posiciones = list_create();
	int nro_linea = 0, long_linea = 0;
	char *separador = "\n";
	char **lineas = string_split(string_posiciones, separador);
	do {
		if(lineas[nro_linea] != NULL ){
			long_linea = string_length(lineas[nro_linea]) + 1;
			long_string -= long_linea ;
			t_posicion_pokemon *posicion = string_to_posicion(lineas[nro_linea]);
			void *posic = (t_posicion_pokemon*) posicion;
			list_add(lista_posiciones, posic);
			nro_linea ++;
		}
	} while (long_string > 0);
	liberar_listas(lineas);
	return lista_posiciones;
}

bool existe_posicion_en_archivo(t_archivo_pokemon *archivo, t_posicion_pokemon *posicion)
{
	bool misma_posicion(void *posi)
	{
		t_posicion_pokemon *position = (t_posicion_pokemon*) posi;
		bool condition = position->pos_x == posicion->pos_x && position->pos_y == posicion->pos_y;
		return condition;
	}
	bool existe_posicion = false;
	int cant_posiciones = archivo->posiciones->elements_count;
	if (cant_posiciones > 0) {
		existe_posicion = list_any_satisfy(archivo->posiciones, misma_posicion);
	}
	return existe_posicion;
}

int agregar_nueva_posicion_en_archivo(t_archivo_pokemon *archivo, t_posicion_pokemon *posicion, t_log *logger)
{
	list_add(archivo->posiciones, posicion);
	int cant_posiciones = modificar_archivo_pokemon(archivo, logger);
	return cant_posiciones;
}

int modificar_posicion_en_archivo(t_archivo_pokemon *archivo, t_posicion_pokemon *posicion, t_log *logger)
{
	bool misma_posicion(void *posi)
	{
		t_posicion_pokemon *position = (t_posicion_pokemon*) posi;
		bool condition = position->pos_x == posicion->pos_x && position->pos_y == posicion->pos_y;
		return condition;
	}
	t_posicion_pokemon *position = (t_posicion_pokemon*) list_find(archivo->posiciones,(void*) misma_posicion);
	position->cantidad += posicion->cantidad;
	int cant_posiciones = modificar_archivo_pokemon(archivo, logger);
	free(posicion);
	return cant_posiciones;
}

int eliminar_posicion_en_archivo(t_archivo_pokemon *archivo, t_posicion_pokemon *posicion, t_log *logger)
{
	bool misma_posicion(void *posi)
	{
		t_posicion_pokemon *position = (t_posicion_pokemon*) posi;
		bool condition = position->pos_x == posicion->pos_x && position->pos_y == posicion->pos_y;
		return condition;
	}
	t_posicion_pokemon *position = (t_posicion_pokemon*) list_find(archivo->posiciones,(void*) misma_posicion);
	position->cantidad --;
	if (position->cantidad == 0) {
		list_remove_and_destroy_by_condition(archivo->posiciones, misma_posicion,(void*) free);
	}
	int cant_posiciones = modificar_archivo_pokemon(archivo, logger);
	return cant_posiciones;
}

int actualizar_posiciones_archivo(t_archivo_pokemon *archivo, t_posicion_pokemon *posicion, t_log *logger)
{
	if (!existe_posicion_en_archivo(archivo, posicion)){
		agregar_nueva_posicion_en_archivo(archivo, posicion, g_logger);
	} else {
		modificar_posicion_en_archivo(archivo, posicion, g_logger);
	}
	int cant_posiciones = archivo->posiciones->elements_count;
	return cant_posiciones;
}

char *get_contenido_bloques(t_pokemon_medatada *pokemon_metadata)
{	//Retorna un string con el contenido de los bloques del pokemon
	char *str_posiciones = string_new();//Cargar todos los bloques en un string de posiciones
	int leer_size = 0, cant_bloques = pokemon_metadata->blocks->elements_count, iter = 0,
		tamano_bloque = g_config_tg->block_size, total_bytes = pokemon_metadata->size;
	do {
		char *block = (char*) list_get(pokemon_metadata->blocks, iter);
		char *str_bloque;
		if (total_bytes < tamano_bloque) {
			str_bloque =  get_contenido_bloque(total_bytes, block);
			total_bytes = 0;
		} else {
			str_bloque =  get_contenido_bloque(tamano_bloque, block);
			total_bytes -= tamano_bloque;
		}
		string_append(&str_posiciones,str_bloque);
		free(str_bloque);
		iter ++;
	} while (total_bytes > 0);
	return str_posiciones;
}

char *get_contenido_bloque(int block_size, char *block)
{	//Retorna el contenido de un bloque en un string
	char *contenido = (char*) calloc(block_size, sizeof(char)), *extension = ".bin", *cola = "\0" ;
	int lng_block = string_length(block), lng_dir_file = string_length(g_config_gc->dirname_blocks), lng_bin = string_length(extension);
	int lng_full_path = lng_block + lng_dir_file + lng_bin + 1, offset = 0;
	char *file_bloque = malloc(lng_full_path);
	memcpy(file_bloque + offset, g_config_gc->dirname_blocks, lng_dir_file);
	offset += lng_dir_file;
	memcpy(file_bloque + offset, block, lng_block);
	offset += lng_block;
	memcpy(file_bloque + offset, extension, lng_bin);
	offset += lng_bin;
	memcpy(file_bloque + offset, cola, 1);
	FILE* fd = fopen(file_bloque, "r");
	fread(contenido, sizeof(char),block_size , fd);
	fclose(fd);
	free(file_bloque);
	return contenido;
}

t_posicion_pokemon *string_to_posicion(char* str_posicion)
{	//hace un split del string de la posicion y retorna la estructura de la posicion
	char** keyValue = string_split(str_posicion, "=");
	char* key = keyValue[0];
	int cantidad = atoi(keyValue[1]);
	char** posiciones = string_split(key, "-");
	int posicionX = atoi(posiciones[0]);
	int posicionY = atoi(posiciones[1]);
	t_posicion_pokemon *posicion = malloc(sizeof(t_posicion_pokemon));
	posicion->cantidad = cantidad;
	posicion->pos_x = posicionX;
	posicion->pos_y = posicionY;
	liberar_listas(posiciones);
	liberar_listas(keyValue);
	return posicion;
}

t_pokemon_medatada *leer_metadata_pokemon(char *pokemon,  t_log *logger)
{	// Lee la metadata del pokemon y la carga en la estructura pokemon_metadata
	t_pokemon_medatada *pokemon_metadata = NULL;
	int cant_bloques = 0, tamano_bloque = g_config_tg->block_size, long_dir = 0, size_pokemon = 0, fragmentacion = 0, long_bloques = 0;
	char *path_metadata = obtengo_ruta_metadata_pokemon(pokemon);
	if (path_metadata != NULL ) {
		pokemon_metadata = malloc(sizeof(t_pokemon_medatada));
		t_config* metadata_file_info = config_create(path_metadata);
		free(path_metadata);
		char *string_bloques = config_get_string_value(metadata_file_info, "BLOCKS");
		size_pokemon = strlen(pokemon) + 1;
		pokemon_metadata->archivo_pokemon = malloc(size_pokemon);
		memcpy(pokemon_metadata->archivo_pokemon, pokemon, size_pokemon);
		pokemon_metadata->directory = si_no(config_get_string_value(metadata_file_info, "DIRECTORY"));
		pokemon_metadata->open = si_no(config_get_string_value(metadata_file_info, "DIRECTORY"));
		pokemon_metadata->size = config_get_int_value(metadata_file_info, "SIZE");
		if (pokemon_metadata->size > 0) {
			pokemon_metadata->blocks = obtengo_lista_bloques(string_bloques);
		} else {
			pokemon_metadata->blocks = list_create();
		}
		config_destroy(metadata_file_info);
		log_info(logger,"(READING METADATADA FILE: %s | DIRECTORY: %s | SIZE: %d | OPEN: %s | USED_BLOCKS: %d)",
			pokemon_metadata->archivo_pokemon, print_si_no(pokemon_metadata->directory), pokemon_metadata->size,
			print_si_no(pokemon_metadata->open),pokemon_metadata->blocks->elements_count);
	} else {
		log_warning(logger, "(-->>Pokemon %s no existe en File System<<--)", pokemon);
	}
	return pokemon_metadata;
}

void modificar_metadata_pokemon(char *pokemon, char *clave, char *valor)
{
	char *path_metadata = obtengo_ruta_metadata_pokemon(pokemon);
	t_config* metadata_file_info = config_create(path_metadata);
	config_set_value(metadata_file_info, clave, valor);
	config_save(metadata_file_info);
	config_destroy(metadata_file_info);
	free(path_metadata);
}

char *obtengo_ruta_metadata_pokemon(char *pokemon)
{
	char *dir_file = g_config_gc->dirname_files, *bin_file = "/Metadata.bin", *cola = "\0";
	int lng_pokemon = string_length(pokemon), lng_dir_file = string_length(dir_file), lng_bin_file = string_length(bin_file);
	int lng_full_path = lng_pokemon + lng_dir_file + lng_bin_file + 1, offset =0;
	char *metadata_path = malloc(lng_full_path);
	memcpy(metadata_path + offset, dir_file, lng_dir_file);
	offset += lng_dir_file;
	memcpy(metadata_path + offset, pokemon, lng_pokemon);
	offset += lng_pokemon;
	memcpy(metadata_path + offset, bin_file, lng_bin_file);
	offset += lng_bin_file;
	memcpy(metadata_path + offset, cola, 1);
	return metadata_path;
}


t_list *obtengo_lista_bloques(char *string_bloques)
{
	t_list *lista_bloques = list_create(), *lista_chars = list_create();
	int long_string = string_length(string_bloques), l = 1;
	for (int i = 0; i < long_string; i++) {
		char *aux = malloc(2);
		memcpy(aux, string_bloques + i, 1);
		char *cola = "\0";
		memcpy(aux + 1, cola, 1);
		list_add(lista_chars, aux);
	}
	bool flag = false, new = true;
	char *bloque;
	int offset = 0;
	do {
		if (new) {
			bloque = malloc(1);
			offset = 0;
		}
		char *x = (char*) list_get(lista_chars, l);
		if (strcmp(x,",") == 0) {
			char *cola = "\0";
			memcpy(bloque + offset, cola, 1);
			void *block = (char*) bloque;
			list_add(lista_bloques, block);
			new = true;
		} else {
			bloque = realloc(bloque, (offset + 2));
			memcpy(bloque + offset,x,1);
			new = false;
			offset ++;
		}
		l ++;
		if (l == (long_string - 1)) {
			char *cola = "\0";
			memcpy(bloque + offset, cola, 1);
			void *block = (char*) bloque;
			list_add(lista_bloques, block);
			flag = true;
		}
	} while(flag == false);
	list_destroy_and_destroy_elements(lista_chars,(void*) free);
	return lista_bloques;
}

bool existe_archivo(char *pokemon)
{
	bool resultado = false;
	char *ruta_metadata = obtengo_ruta_metadata_pokemon(pokemon);
	int fd = open(ruta_metadata, O_RDONLY);
	if (fd != -1) {
		resultado = true;
	}
	close(fd);
	free(ruta_metadata);
	return resultado;
}

bool esta_abierto_archivo_pokemon(char *pokemon)
{
	bool resultado = false;
	bool archivo_en_lista(void* elem) {
		char *archivo = (char*) elem;
		bool resultado = strcmp(archivo, pokemon) == 0;
		return resultado;
	}
	pthread_mutex_lock(&g_mutex_open_files_list);
	resultado = list_any_satisfy(g_archivos_abiertos, archivo_en_lista);
	for (int i = 0; i < g_archivos_abiertos->elements_count; i ++) {
		char *abierto = (char*) list_get(g_archivos_abiertos, i);
		puts(abierto);
	}
	pthread_mutex_unlock(&g_mutex_open_files_list);
	if(!resultado) {
		char *path_metadata = obtengo_ruta_metadata_pokemon(pokemon);
		t_config* metadata_file_info = config_create(path_metadata);
		resultado = si_no(config_get_string_value(metadata_file_info, "OPEN"));
		config_destroy(metadata_file_info);
		free(path_metadata);
	}
	return resultado;
}

void quitar_de_lista_archivos_abiertos(char *pokemon)
{
	bool archivo_en_lista(void* elem) {
		char *archivo = (char*) elem;
		bool resultado = strcmp(archivo, pokemon) == 0;
		return resultado;
	}
	pthread_mutex_lock(&g_mutex_open_files_list);
	if (g_archivos_abiertos->elements_count > 0) {
		list_remove_by_condition(g_archivos_abiertos, archivo_en_lista);
		//puts("a ver si quedan archivos abiertos");
		for (int i = 0; i < g_archivos_abiertos->elements_count; i ++) {
			char *abierto = (char*) list_get(g_archivos_abiertos, i);
			//puts(abierto);
		}
	}
	pthread_mutex_unlock(&g_mutex_open_files_list);
}

void inicializar_bitmap_tallgrass(t_log *logger)
{
	g_bitmap_bloques = malloc(sizeof(t_bitmap_fs));
	char *path_arhivo = g_config_gc->ruta_bitmap;
	int cant_bloques = g_config_tg->blocks, tamano_bitmap = (int)(cant_bloques / 8), tamano_fs = 0;
	g_bitmap_bloques->bits_mmap_file = abrir_archivo_bitmap(path_arhivo , cant_bloques, g_logdebug);
	g_bitmap_bloques->bitmap = bitarray_create_with_mode(g_bitmap_bloques->bits_mmap_file, tamano_bitmap, LSB_FIRST);
	g_bitmap_bloques->total_blocks = cant_bloques;
	g_bitmap_bloques->used_blocks = 0;
	g_bitmap_bloques->nro_bloque = 0;
	leer_contador_bloques();
	g_archivos_abiertos = list_create();
	cant_bloques -= g_bitmap_bloques->used_blocks;
	tamano_fs = (int) (g_config_tg->tamano_fs - (g_config_tg->block_size * g_bitmap_bloques->used_blocks))/1024;
	log_debug(logger,"(TallGrass Iniciado  | Bloques Libres:%d| Espacio Disponible:%d Kb.)\n",cant_bloques, tamano_fs);
}

void *abrir_archivo_bitmap(char *path, int size, t_log *logger)
{	// Abro el archivo bitmap.
	int  fd = open(path, O_RDWR);
	bool new_file = false;
	if (fd == -1) { // Si no existe, se crea el archivo bitmap vacío.
		new_file = true;
		log_warning(logger,"(%s -->>%s<<-- Creando Archivo Bitmap)", strerror(errno), path);
		fd = open(path, O_CREAT | O_RDWR, 0664);
	}
	if (fd == -1) {
		log_error(logger,"(%s -->>%s<<--)", strerror(errno), path);
		return NULL;
	} // Le doy el tamanio de la config
	int truncate_result = ftruncate(fd, size);
	if( truncate_result != 0 ) {
		log_error(logger,"(%s -- Bitmap_File_Not_Truncated:%d)",strerror(errno), truncate_result);
	}									// Habilito Lectura/Escritura -- Comparto Cambios //
	void * dataArchivo = mmap( NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	msync(dataArchivo, size, MS_SYNC); 	// Sincroniza Memoria de forma Sincrónica //
	if (new_file) {
		int size_bitarray = (int)(size / 8);
		t_bitarray *bitarray_vacio =  bitarray_create_with_mode(dataArchivo, size_bitarray, LSB_FIRST);
		bitarray_destroy(bitarray_vacio);
	}
	return dataArchivo;
}

void leer_contador_bloques(void)
{
	pthread_mutex_lock(&g_mutex_cnt_blocks);
	int used_blocks = 0;
	for (int i = 0; i < g_bitmap_bloques->total_blocks; i ++) {
		if (bitarray_test_bit(g_bitmap_bloques->bitmap, i)) {
			used_blocks ++;
		}
	}
	g_bitmap_bloques->used_blocks += used_blocks;
	pthread_mutex_unlock(&g_mutex_cnt_blocks);
}

bool bloques_disponibles(void)
{
	bool resultado = false;
	for (int i = 0; i < g_bitmap_bloques->total_blocks; i ++) {
		if (!bitarray_test_bit(g_bitmap_bloques->bitmap, i)) {
			bitarray_set_bit(g_bitmap_bloques->bitmap, i);
			g_bitmap_bloques->nro_bloque = i ;
			i = g_bitmap_bloques->total_blocks;
			resultado = true;
		}
	}
	g_bitmap_bloques->used_blocks ++;
	return resultado;
}

void incremento_cont_bloques(void)
{
	for (int i = 0; i < g_bitmap_bloques->total_blocks; i ++) {
		if (!bitarray_test_bit(g_bitmap_bloques->bitmap, i)) {
			bitarray_set_bit(g_bitmap_bloques->bitmap, i);
			g_bitmap_bloques->nro_bloque = i ;
			i = g_bitmap_bloques->total_blocks;
		}
	}
	g_bitmap_bloques->used_blocks ++;
}

void liberar_nro_bloque_bitmap(int block_nro)
{	//cuando libero un bloque no hace falta decrementar g_bitmap_bloques->used_blocks
	pthread_mutex_lock(&g_mutex_cnt_blocks);// se le resta 1 porque el bitmap arranca en cero
	bitarray_clean_bit(g_bitmap_bloques->bitmap, block_nro);
	g_bitmap_bloques->used_blocks --;
	pthread_mutex_unlock(&g_mutex_cnt_blocks);
}

int obtener_ultimo_nro_bloque(void)
{
	pthread_mutex_lock(&g_mutex_cnt_blocks);
	for (int i = 1; i <= g_bitmap_bloques->total_blocks; i ++) {
		if (!(bitarray_test_bit(g_bitmap_bloques->bitmap, i))) {
			bitarray_set_bit(g_bitmap_bloques->bitmap, i);
			g_bitmap_bloques->nro_bloque = i;
			g_bitmap_bloques->used_blocks ++;
			pthread_mutex_unlock(&g_mutex_cnt_blocks);
			return i;
		}
	}
	pthread_mutex_unlock(&g_mutex_cnt_blocks);
	return -1;
}

bool si_no(char *valor)
{
	bool resultado;
	if(strcmp(valor,"S") == 0) {
		resultado = true;
	}
	else if (strcmp(valor,"N") == 0) {
		resultado = false;
	}
	return resultado;
}

char *print_si_no(bool valor)
{
	char *resultado;
	if(valor) {
		resultado = "S";
	} else {
		resultado = "N";
	}
	return resultado;
}
