/*
 * tallGrass.c
 *
 *
 *      Author: utnso
 */

#include "tall_grass.h"

void prueba_file_system(char* pokemon, int cant_posiciones)
{
	t_list* lista_posiciones = list_create();
	for (int i = 0; i < cant_posiciones; i ++) {
		t_posicion_pokemon *posicion = malloc(sizeof(t_posicion_pokemon));
		posicion->pos_x = 15 + i * 3;
		posicion->pos_y = 20 * i;
		posicion->cantidad = (i +1) * 100;
		void *posi = (t_posicion_pokemon*) posicion;
		list_add(lista_posiciones, posicion);
	}
	file_system_pokemon(pokemon, lista_posiciones);
	log_info(g_logger,"FIN PRUEBA GUARDAR ARCHIVO\n");
}

void prueba_leer_bloques_pokemon(char* pokemon)
{
	t_list* lista_posiciones_bloques = list_create();
	lista_posiciones_bloques = leer_bloques(pokemon);
	destruir_lista_posiciones_bloques(lista_posiciones_bloques);
}

/**
 *crea el filesystem del pokeno: metadata.bin y bloque.bin
 * 1 Armar un string, del tamanio del bloque con todas las lista de posiciones que entren
 * 2 Conforme se llena string se guarda en el bloque
 */
void file_system_pokemon(char *pokemon, t_list *lista_posiciones){
	log_info(g_logger,"CREATE FILESYSTEM %s",pokemon);
	int lng_str_posiciones = 0;
	char *string_posiciones;
	t_list*  blocks;
	/*for (int i = 0; i < lista_posiciones->elements_count ; i ++) {
		t_posicion_pokemon *posicion = malloc(sizeof(t_posicion_pokemon));
		posicion->pos_x = 15 + i * 3;
		posicion->pos_y = 20 * i;
		posicion->cantidad = (i +1) * 100;
		void *posi = (t_posicion_pokemon*) posicion;
		list_add(lista_posiciones, posicion);
	}*/
	string_posiciones = serializar_lista_posiciones_pokemon(lista_posiciones, g_logger);
	lng_str_posiciones = string_length(string_posiciones);
	blocks = armar_guardar_data_bloques_file_pokemon(string_posiciones);
	grabar_metadata_pokemon(blocks, pokemon, lng_str_posiciones,"N", g_logger);
	list_destroy(blocks);
	list_destroy_and_destroy_elements(lista_posiciones,(void*) free);
}

char *serializar_lista_posiciones_pokemon(t_list *lista_posiciones, t_log *logger)
{
	int size_posicion =0;
	char *string_posiciones;
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
	char *file_block = string_itoa(block_nro);
	char *path_blocks = string_new();
	string_append(&path_blocks, g_config_gc->dirname_blocks);
	string_append(&path_blocks, file_block);
	string_append(&path_blocks, ".bin");
	FILE* fd = fopen(path_blocks,"w");
	fwrite(block_buffer,sizeof(char), size, fd);
	fclose(fd);
	log_info(logger,"SAVE BLOCK | FILE %d.bin", block_nro);
	pthread_mutex_lock(&g_mutex_cnt_blocks);
	if (!incremento_contador_bloques()) {
		log_error(logger, "-->>!!Sin Espacio en File System - Cerrar GAMECARD!! <<--");
		pthread_mutex_lock(&g_mutex_cnt_blocks);
	}
	pthread_mutex_unlock(&g_mutex_cnt_blocks);
	free(file_block);
	free(path_blocks);
	free(block_buffer);
	return block_nro;
}

void grabar_metadata_pokemon(t_list *blocks, char *pokemon, int file_size, char *status,  t_log *logger)
{	//Graba la metadata del pokemon(Metadata.bin)
	crear_dirname_pokemon(pokemon, g_logger);
	int cantidad_bloques = blocks->elements_count;
	int long_dir = string_length(g_config_gc->dirname_files) + string_length(pokemon) + string_length("/Metadata.bin");
	char* dirname_metatada_pokemon = (char*) calloc(long_dir, sizeof(char));
	string_append(&dirname_metatada_pokemon,g_config_gc->dirname_files);
	string_append(&dirname_metatada_pokemon,pokemon);
	string_append(&dirname_metatada_pokemon,"/Metadata.bin");
	FILE* fd = fopen(dirname_metatada_pokemon,"wb");
	fprintf(fd, "DIRECTORY=N\n");
	fprintf(fd, "SIZE=%d\n", file_size);
	fprintf(fd, "BLOCKS=[");
	for(int i = 0 ; i < cantidad_bloques;  i++ ){
		int *bloque_actual = list_get(blocks, i);
		fprintf(fd, "%d", *bloque_actual);
		if(i < cantidad_bloques - 1) {
			fprintf(fd,",");
		} else {
			fprintf(fd, "]\n");
		}
		free(bloque_actual);
	}
	fprintf(fd, "OPEN=%s\n", status);
	fclose(fd);
	log_info(logger,"SAVE METADATA  %s",dirname_metatada_pokemon);
	free(dirname_metatada_pokemon);
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
	config_destroy(g_config);
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

t_list* leer_bloques(char *pokemon){
	//lee los bloques que se corresponden a un archivo pokemon y lo retorna en una lista de posiciones
	log_info(g_logger,"READ FILE %s",pokemon); // cargar todos los bloques en un string de posiciones
	t_list *lista_posiciones = list_create();
	int data_size = 0;
	t_pokemon_medatada *pokemon_metadata = leer_metadata_pokemon(pokemon, g_logger);
	if (pokemon_metadata != NULL) {
		char *string_posiciones = get_contenido_bloques(pokemon_metadata, g_logger);
		t_list *lista_posiciones = obtener_posiciones(string_posiciones, pokemon_metadata->size);
		log_info(g_logger,"POSITIONS POKEMON ");
		for( int i = 0; i< list_size(lista_posiciones); i ++){
			t_posicion_pokemon *posicion = (t_posicion_pokemon*) list_get(lista_posiciones,i);
			log_info(g_logger," pos_x %d | pos_y %d | cantidad %d ",posicion->pos_x, posicion->pos_y, posicion->cantidad);
		}
		log_info(g_logger,"END READ FILE POKEMON %s", pokemon);
		data_size = pokemon_metadata->size;
		//list_destroy_and_destroy_elements(pokemon_metadata->blocks,(void*) free);
		free(pokemon_metadata);
		free(string_posiciones);
		list_destroy_and_destroy_elements(lista_posiciones,(void*) free);

//	return data_size;
	}/* else {

		//return -1;
	} */
	return lista_posiciones;
}

void destruir_lista_posiciones_bloques(t_list* lista_posiciones_bloques){
	list_destroy_and_destroy_elements(lista_posiciones_bloques,(void*) free);
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

char *get_contenido_bloques(t_pokemon_medatada *pokemon_metadata, t_log *logger)
{	//Retorna un string con el contenido de los bloques del pokemon
	char *str_posiciones = string_new();//Cargar todos los bloques en un string de posiciones
	int leer_size = 0, cant_bloques = pokemon_metadata->blocks->elements_count, iter = 0,
		tamano_bloque = g_config_tg->block_size, total_bytes = pokemon_metadata->size;
	do {
		char *block = (char*) list_get(pokemon_metadata->blocks, iter);
		char *str_bloque;
		if (total_bytes < tamano_bloque) {
			printf("<<bl# %s size %d\n", block, total_bytes);
			str_bloque =  get_contenido_bloque(total_bytes, block);
			total_bytes = 0;
		} else {
			printf(">=bl# %s size %d\n", block, tamano_bloque);
			str_bloque =  get_contenido_bloque(tamano_bloque, block);
			total_bytes -= tamano_bloque;
		}
		string_append(&str_posiciones,str_bloque);
		free(str_bloque);
		iter ++;
	} while (total_bytes > 0);
	log_info(logger,"CONTENT FILE POKEMON:\n%s", str_posiciones);
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
	t_pokemon_medatada *pokemon_metadata = malloc(sizeof(t_pokemon_medatada));
	int cant_bloques = 0, tamano_bloque = g_config_tg->block_size, long_dir = 0, fragmentacion = 0, long_bloques = 0;
	char *path_metadata = obtengo_ruta_metadata_pokemon(pokemon);
	if (path_metadata != NULL ) {
		t_config* metadata_file_info = obtengo_info_metadata(path_metadata);
		char *string_bloques = config_get_string_value(metadata_file_info, "BLOCKS");
		pokemon_metadata->archivo_pokemon = pokemon;
		pokemon_metadata->directory = si_no(config_get_string_value(metadata_file_info, "DIRECTORY"));
		pokemon_metadata->open = si_no(config_get_string_value(metadata_file_info, "DIRECTORY"));
		pokemon_metadata->size = config_get_int_value(metadata_file_info, "SIZE");
		pokemon_metadata->blocks = obtengo_lista_bloques(string_bloques);
		log_info(logger,"(METADATADA: %s | DIRECTORY: %s | SIZE: %d | OPEN: %s | USED_BLOCKS: %d)",
			pokemon_metadata->archivo_pokemon, print_si_no(pokemon_metadata->directory), pokemon_metadata->size,
			print_si_no(pokemon_metadata->open),pokemon_metadata->blocks->elements_count);
		config_destroy(metadata_file_info);
		return pokemon_metadata;
	} else {
		log_warning(logger, "(-->>Pokemon %s no existe en File System<<--)", pokemon);
		return NULL;
	}
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
	int fd = open(metadata_path, O_RDONLY);
	if (fd == -1) {
		free(metadata_path);
		return NULL;
	} else {
		return metadata_path;
}	}

t_config *obtengo_info_metadata(char *metadata_path)
{
	t_config *metadata_info = config_create(metadata_path);
	free(metadata_path);
	return metadata_info;
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

bool incremento_contador_bloques(void)
{
	bool resultado = false;
	for (int i = 0; i < g_bitmap_bloques->total_blocks; i ++) {
		if (!bitarray_test_bit(g_bitmap_bloques->bitmap, i)) {
			bitarray_set_bit(g_bitmap_bloques->bitmap, i);
			g_bitmap_bloques->nro_bloque = i + 1;
			i = g_bitmap_bloques->total_blocks;
			resultado = true;
			g_bitmap_bloques->used_blocks ++;
		}
	}
	return resultado;
}

int obtener_ultimo_nro_bloque(void)
{
	pthread_mutex_lock(&g_mutex_cnt_blocks);
	int nro_bloque = g_bitmap_bloques->nro_bloque;
	pthread_mutex_unlock(&g_mutex_cnt_blocks);
	return nro_bloque;
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


bool existe_dirname_pokemon(char *pokemon){
	char* pathPokemon = get_dirname_pokemon(pokemon);
	struct stat st = { 0 };
	int ret = 1;
	if (stat(pathPokemon, &st) == -1) {
		//El directorio no existe
		ret  = 0;
	}
	free(pathPokemon);
	return ret;

}

/**
 * Nombre del directorio que debe tener el pokemon
 */
char *get_dirname_pokemon(char *pokemon){
	// Nombre file pokemon
	int long_dir = string_length(g_config_gc->dirname_files) + string_length(pokemon);
	char* dirname_pokemon = (char*) calloc(long_dir, sizeof(char));
	string_append(&dirname_pokemon,g_config_gc->dirname_files);
	string_append(&dirname_pokemon,pokemon);
	return dirname_pokemon;
/*
	char* pathPokemon = string_new();
	string_append(&pathPokemon, g_config_gc->dirname_files);
	string_append(&pathPokemon, pokemon);
	return pathPokemon;
*/
}

/**
 * Retorna los pokemons que se ecuentran en el FileSystem
 */
t_list* get_files_pokemon(){
	t_list* lista_pokemon = list_create();
	DIR* FD;
	struct dirent* file;
    FD = opendir (g_config_gc->dirname_files);
    while( file = readdir(FD) ){
    	if( string_equals_ignore_case(file->d_name, ".") || string_equals_ignore_case(file->d_name, ".") ){
    		continue;
    	}
    	list_add(lista_pokemon, file->d_name);
    }
    closedir(FD);
    return lista_pokemon;
}
