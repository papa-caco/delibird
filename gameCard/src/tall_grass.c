/*
 * tallGrass.c
 *
 *
 *      Author: utnso
 */

#include "tall_grass.h"

void prueba_file_system(char* pokemon, int cant_posiciones)
{
	file_system_pokemon(pokemon, cant_posiciones);
}

void prueba_leer_bloques_pokemon(char* pokemon)
{
	t_list* lista_posiciones = list_create();
	lista_posiciones = leer_bloques(pokemon);
}

/**
 *crea el filesystem del pokeno: metadata.bin y bloque.bin
 * 1 Armar un string, del tamanio del bloque con todas las lista de posiciones que entren
 * 2 Conforme se llena string se guarda en el bloque
 */
void file_system_pokemon(char *pokemon, int cant_posiciones)
{
	log_info(g_logger,"CREATE FILESYSTEM %s",pokemon);
	t_list* lista_posiciones = list_create();
	for (int i = 0; i < cant_posiciones; i ++) {
		t_posicion_pokemon *posicion = malloc(sizeof(t_posicion_pokemon));
		posicion->pos_x = 15 + i * 3;
		posicion->pos_y = 20 * i;
		posicion->cantidad = i +1;
		void *posi = (t_posicion_pokemon*) posicion;
		list_add(lista_posiciones, posicion);
	}
	int tamano_bloque = g_config_tg->block_size, x = 1, size_posicion =0;
	t_list *blocks = list_create();
	char *string_posiciones, *string_posicion;
	for(int i = 0; i < lista_posiciones->elements_count; i++ ){
		t_posicion_pokemon  *posicion = list_get(lista_posiciones,i);
		log_info(g_logger,"PROCESS POSICION %d-%d=%d",posicion->pos_x,posicion->pos_y,posicion->cantidad);
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
	}
	blocks = armar_guardar_data_bloques_file_pokemon(string_posiciones);
	grabar_metadata_pokemon(blocks, pokemon, size_posicion,"N");
	free(string_posiciones);
	list_destroy_and_destroy_elements(lista_posiciones,(void*) free);
}

t_list *armar_guardar_data_bloques_file_pokemon(char *string_posiciones)
{
	int tamano_bloque = g_config_tg->block_size, comienzo = 0, excedente = 0, cant_bloques = 1,
		tamano_string = string_length(string_posiciones);
	char *grabar;
	t_list *blocks = list_create();
	if (tamano_string >= tamano_bloque) {
		cant_bloques = (int) tamano_string / tamano_bloque;
		double resto = (double) (tamano_string % tamano_bloque);
		if (resto > 0) {
			cant_bloques ++;
		}
		for (int i = 0; i < cant_bloques; i ++) {
			if (i < cant_bloques - 1) {
				grabar = string_substring(string_posiciones, comienzo, tamano_bloque);
			} else {
				excedente =  tamano_string - comienzo;
				grabar = string_substring(string_posiciones, comienzo, excedente);
			}
			int *bloque_nro = malloc(sizeof(int));
			(*bloque_nro) = g_bitmap_bloques->nro_bloque;
			list_add(blocks, bloque_nro);
			grabar_bloque(g_bitmap_bloques->nro_bloque, grabar);
			comienzo += tamano_bloque;
		}
	} else {
		int *bloque_nro = malloc(sizeof(int));
		(*bloque_nro) = g_bitmap_bloques->nro_bloque;
		list_add(blocks,bloque_nro);
		grabar_bloque(g_bitmap_bloques->nro_bloque, string_posiciones);
	}
	return blocks;
}

void grabar_bloque(int block_nro, char *block_buffer)
{	//Graba un bloque en el file system
	char *file_block = string_itoa(block_nro);
	char *path_blocks = string_new();
	string_append(&path_blocks, g_config_gc->dirname_blocks);
	string_append(&path_blocks, file_block);
	string_append(&path_blocks, ".bin");
	FILE* fd = fopen(path_blocks,"w");
	fwrite(block_buffer,sizeof(char),g_config_tg->block_size,fd);
	fclose(fd);
	log_info(g_logger,"SAVE BLOCK | FILE %d.bin", block_nro);
	incremento_contador_bloques();
	free(block_buffer);
}

void grabar_metadata_pokemon(t_list *blocks, char *pokemon, int size_pokemon, char *status)
{	//Graba la metadata del pokemon(Metadata.bin)
	crear_dirname_pokemon(pokemon);
	int cantidad_bloques = blocks->elements_count;
	char* dirname_metatada_pokemon = string_new();
	string_append(&dirname_metatada_pokemon,g_config_gc->dirname_files);
	string_append(&dirname_metatada_pokemon,pokemon);
	string_append(&dirname_metatada_pokemon,"/Metadata.bin");
	FILE* fd = fopen(dirname_metatada_pokemon,"wb");
	fprintf(fd, "DIRECTORY=N\n");
	fprintf(fd, "SIZE=%d\n", size_pokemon);
	fprintf(fd, "BLOCKS=[");
	for(int i = 0 ; i < cantidad_bloques;  i++ ){
		int *bloque_actual = list_get(blocks, i);
		fprintf(fd, "%d", *bloque_actual);
		if(i < cantidad_bloques - 1) {
			fprintf(fd,",");
		} else {
			fprintf(fd, "]\n");
		}
	}
	fprintf(fd, "OPEN=%s\n", status);
	fclose(fd);
	log_info(g_logger,"SAVE METADATA  %s",dirname_metatada_pokemon);
	list_destroy_and_destroy_elements(blocks,(void*) free);
}

void crear_dirname_pokemon(char *pokemon)
{
	char* pathPokemon = string_new();
	string_append(&pathPokemon, g_config_gc->dirname_files);
	string_append(&pathPokemon, pokemon);
	struct stat st = { 0 };
	if (stat(pathPokemon, &st) == -1) {
		log_info(g_logger, "CREATE_DIR Pokemon %s",pathPokemon);
		mkdir(pathPokemon, 0774);
	}
}

void leer_metadata_tall_grass(t_log *logger)
{
	t_config* g_config;
	g_config = config_create(g_config_gc->file_metadata);
	g_config_tg = malloc(sizeof(t_config_tall_grass));
	g_config_tg->block_size = config_get_int_value(g_config,"BLOCK_SIZE");
	g_config_tg->blocks = config_get_int_value(g_config,"BLOCKS");
	g_config_tg->magic_number = valor_magic_number(config_get_string_value(g_config,"MAGIC_NUMBER"));
	g_config_tg->tamano_fs = g_config_tg->blocks * g_config_tg->block_size;
	int tamano = (int) g_config_tg->tamano_fs / 1024;
	log_debug(logger, "(FileSystem TallGrass| Tam.Bloque:%d Bytes| Cant.Bloques:%d| Tamaño FS:%d Kbytes| Magic_Number:%d)\n",
			g_config_tg->block_size, g_config_tg->blocks, tamano, g_config_tg->magic_number );
}

int valor_magic_number(char *string_fijo)
{
	int magic_number = 0 , comparador = 0;
	comparador = strcmp(string_fijo, "TALL_GRASS");
	if (comparador == 0) {
		magic_number = TALL_GRASS;
	}
	return magic_number;
}

t_list * leer_bloques(char *pokemon)
{	//lee los bloques que se corresponden a un archivo pokemon y lo retorna en una lista de posiciones
	log_info(g_logger,"READ FILE %s",pokemon); // cargar todos los bloques en un string de posiciones
	t_list *lista_posiciones = list_create(); // pasar del string de posiciones a un lista con la estructura de posiciones
	t_pokemon_medatada *pokemon_metadata = leer_metadata_pokemon(pokemon);
	char *string_posiciones = get_contenido_bloques(pokemon_metadata);
	lista_posiciones = obtener_posiciones(string_posiciones);
	log_info(g_logger,"POSITIONS POKEMON ");
	for( int i=0; i< list_size(lista_posiciones); i++){
		t_posicion_pokemon *posicion = (t_posicion_pokemon*) list_get(lista_posiciones,i);
		log_info(g_logger," pos_x %d | pos_y %d | cantidad %d ",posicion->pos_x, posicion->pos_y, posicion->cantidad);
	}
	log_info(g_logger,"END READ FILE POKEMON %s",pokemon);
	return lista_posiciones;
}

t_list *obtener_posiciones(char *string_posiciones)
{	//Retorna una estructura de posiciones a partir de un string de posiciones
	t_list *lista_posiciones = list_create();
	int long_string = string_length(string_posiciones), nro_linea = 0, long_linea = 0;
	char *separador = malloc(1);
	separador = "\n";
	char **lineas = string_split(string_posiciones, separador);
	do {
		if(lineas[nro_linea] != NULL ){
			long_linea = string_length(lineas[nro_linea]) + 1;
			long_string -= long_linea ;
			//TODO printf("(%s), long:%d,long_str:%d\n",lineas[nro_linea], long_linea, long_string);
			t_posicion_pokemon *posicion = string_to_posicion(lineas[nro_linea]);
			list_add(lista_posiciones,posicion);
			nro_linea ++;
		}
	} while (long_string > 0);
	return lista_posiciones;
}

char *get_contenido_bloques(t_pokemon_medatada *pokemon_metadata)
{	//Retorna un string con el contenido de los bloques del pokemon
	char *str_posiciones = string_new();
	int leer_size = 0, cant_bloques = pokemon_metadata->blocks->elements_count,
			tamano_bloque = g_config_tg->block_size;
	for(int i=0 ; i <= cant_bloques ; i++ ){ //Cargar todos los bloques en un string de posiciones
		if(list_get(pokemon_metadata->blocks, i) != NULL){
			char *str_bloque = string_new();
			if( (i+1)* tamano_bloque < pokemon_metadata->size){
				leer_size = tamano_bloque;
			} else {
				leer_size = (i+1)* tamano_bloque - pokemon_metadata->size ;
			}
			str_bloque = get_contenido_bloque( leer_size,list_get(pokemon_metadata->blocks,i) );
			string_append(&str_posiciones,str_bloque);
			free(str_bloque);
		}
	}
	log_info(g_logger,"CONTENT FILE POKEMON:\n%s",str_posiciones);
	return str_posiciones;
}

char *get_contenido_bloque(int block_size,char *block)
{	//Retorna el contenido de un bloque en un string
	char *contenido = (char*) calloc(block_size,sizeof(char));
	char *file_bloque = string_new();
	string_append(&file_bloque, g_config_gc->dirname_blocks);
	string_append(&file_bloque, block);
	string_append(&file_bloque, ".bin");
	FILE* fd = fopen(file_bloque, "r");
	fread(contenido, sizeof(char),block_size , fd);
	fclose(fd);
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
	t_posicion_pokemon *posicion = malloc( sizeof(t_posicion_pokemon) );
	posicion->cantidad = cantidad;
	posicion->pos_x = posicionX;
	posicion->pos_y = posicionY;
	liberar_listas(keyValue);
	return posicion;
}

t_pokemon_medatada *leer_metadata_pokemon(char *pokemon)
{	// Lee la metadata del pokemon y la carga en la estructura pokemon_metadata
	t_pokemon_medatada *pokemon_metadata = NULL;
	t_config* g_config = NULL;
	int cant_bloques = 0, tamano_bloque = g_config_tg->block_size;
	double fragmentacion = 0;
	char* dirname_metatada_pokemon = string_new();
	string_append(&dirname_metatada_pokemon,g_config_gc->dirname_files);
	string_append(&dirname_metatada_pokemon,pokemon);
	string_append(&dirname_metatada_pokemon,"/Metadata.bin");
	g_config = config_create(dirname_metatada_pokemon);
	pokemon_metadata = malloc(sizeof(t_pokemon_medatada));
	pokemon_metadata->directory = config_get_string_value(g_config, "DIRECTORY");
	pokemon_metadata->size = config_get_int_value(g_config, "SIZE");
	pokemon_metadata->open = config_get_string_value(g_config, "OPEN");
	pokemon_metadata->file_metatada_pokemon = dirname_metatada_pokemon;
	if (pokemon_metadata->size > tamano_bloque) {
		cant_bloques = (int) pokemon_metadata->size / tamano_bloque;
		fragmentacion = (double) (pokemon_metadata->size % tamano_bloque);
		if (fragmentacion > 0) {
			cant_bloques ++;
		}
		pokemon_metadata->blocks = list_create();
		char **nodeBlockTupleAsArray = string_get_string_as_array(config_get_string_value(g_config, "BLOCKS"));
		for(int bloque = 0; bloque <  cant_bloques; bloque ++) {
			if(nodeBlockTupleAsArray[bloque] != NULL){
				void *block = (char*) nodeBlockTupleAsArray[bloque];
				list_add(pokemon_metadata->blocks, nodeBlockTupleAsArray[bloque]);
			}
		}
	} else {
		cant_bloques = 1;
		int long_bloque = string_length(config_get_string_value(g_config, "BLOCKS"));
		char *bloque = string_substring(config_get_string_value(g_config, "BLOCKS"), 1, long_bloque - 1);
		void *block = (char*) bloque;
		list_add(pokemon_metadata->blocks, bloque);
	}
	log_info(g_logger,"METADATADA POKEMON  DIRECTORY: %s | SYZE: %d | OPEN: %s | BLOCKS: %s",
			pokemon_metadata->directory, pokemon_metadata->size, pokemon_metadata->open ,
			config_get_string_value(g_config, "BLOCKS") );
	//config_destroy(g_config);
	return pokemon_metadata;
}

void inicializar_bitmap_tallgrass(t_log *logger)
{
	g_bitmap_bloques = malloc(sizeof(t_bitmap_fs));
	char *path_arhivo = g_config_gc->ruta_bitmap;
	int cant_bloques = g_config_tg->blocks, tamano_bitmap = (int)(cant_bloques / 8), tamano_fs = 0;
	g_bitmap_bloques->bits_mmap_file = abrir_archivo_bitmap(path_arhivo , cant_bloques, g_logdebug);
	g_bitmap_bloques->bitmap = bitarray_create_with_mode(g_bitmap_bloques->bits_mmap_file, tamano_bitmap, MSB_FIRST);
	g_bitmap_bloques->total_blocks = cant_bloques;
	g_bitmap_bloques->used_blocks = 0;
	g_bitmap_bloques->nro_bloque = 0;
	leer_contador_bloques();
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
	}							// Habilito Lectura/Escritura -- Comparto Cambios //
	void * dataArchivo = mmap( NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	msync(dataArchivo, size, MS_SYNC); // Sincroniza Memoria de forma Sincrónica //
	if (new_file) {
		int size_bitarray = (int)(size / 8);
		t_bitarray *bitarray_vacio =  bitarray_create_with_mode(dataArchivo, size_bitarray, MSB_FIRST);
		bitarray_set_bit(bitarray_vacio, 0);
		bitarray_destroy(bitarray_vacio);
	}
	return dataArchivo;
}

void leer_contador_bloques(void)
{
	pthread_mutex_lock(&g_mutex_cnt_blocks);
	int used_blocks = 0;
	for (int i = 0; i < g_bitmap_bloques->total_blocks; i ++) {
		if (!bitarray_test_bit(g_bitmap_bloques->bitmap, i)) {
			g_bitmap_bloques->nro_bloque = i;
			i = g_bitmap_bloques->total_blocks;
		} else {
			used_blocks ++;
		}
	}
	g_bitmap_bloques->used_blocks += used_blocks - 1;
	pthread_mutex_unlock(&g_mutex_cnt_blocks);
}

void incremento_contador_bloques(void)
{
	pthread_mutex_lock(&g_mutex_cnt_blocks);
	for (int i = 0; i < g_bitmap_bloques->total_blocks; i ++) {
		if (!bitarray_test_bit(g_bitmap_bloques->bitmap, i)) {
			bitarray_set_bit(g_bitmap_bloques->bitmap, i);
			g_bitmap_bloques->nro_bloque = i + 1;
			i = g_bitmap_bloques->total_blocks;
		}
		g_bitmap_bloques->used_blocks ++;
	}
	pthread_mutex_unlock(&g_mutex_cnt_blocks);
}
