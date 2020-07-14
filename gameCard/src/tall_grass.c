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
	log_info(g_logger,"FIN PRUEBA GUARDAR ARCHIVO\n");
}

void prueba_leer_bloques_pokemon(char* pokemon)
{
	 leer_bloques(pokemon);
}

/**
 *crea el filesystem del pokeno: metadata.bin y bloque.bin
 * 1 Armar un string, del tamanio del bloque con todas las lista de posiciones que entren
 * 2 Conforme se llena string se guarda en el bloque
 */
void file_system_pokemon(char *pokemon, int cant_posiciones)  // -->>Esta vuela las posiciones las tiene que cargar/actualizar de a una <<--
{
	log_info(g_logger,"CREATE FILESYSTEM %s",pokemon);
	t_list* lista_posiciones = list_create();
	for (int i = 0; i < cant_posiciones; i ++) {
		t_posicion_pokemon *posicion = malloc(sizeof(t_posicion_pokemon));
		posicion->pos_x = 15 + i * 3;
		posicion->pos_y = 20 * i;
		posicion->cantidad = (i +1) * 100;
		void *posi = (t_posicion_pokemon*) posicion;
		list_add(lista_posiciones, posicion);
	}
	int tamano_bloque = g_config_tg->block_size, x = 1, size_posicion =0;
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
		free(pos_x);
		free(pos_y);
		free(cantidad);
	}  // -->> con las dos funciones de abajo es suficiente para generar un archivo en el FS. -- Faltaría una que le pases una posicion y te concatene el String.
	t_list *blocks = armar_guardar_data_bloques_file_pokemon(string_posiciones);
	grabar_metadata_pokemon(blocks, pokemon, size_posicion,"N");
	list_destroy(blocks);
	list_destroy_and_destroy_elements(lista_posiciones,(void*) free);
}

char *serializar_posicion_pokemon(t_coordenada *coordenada, uint32_t cantidad)
{
	return "hola"; //TODO mañana
}

t_list *armar_guardar_data_bloques_file_pokemon(char *string_posiciones)
{
	int tamano_bloque = g_config_tg->block_size, comienzo = 0, excedente = 0, cant_bloques = 1,
		tamano_string = string_length(string_posiciones);
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
		free(string_posiciones);
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
	free(file_block);
	free(path_blocks);
	free(block_buffer);
}

void grabar_metadata_pokemon(t_list *blocks, char *pokemon, int file_size, char *status)
{	//Graba la metadata del pokemon(Metadata.bin)
	crear_dirname_pokemon(pokemon);
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
	log_info(g_logger,"SAVE METADATA  %s",dirname_metatada_pokemon);
	free(dirname_metatada_pokemon);
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

void leer_bloques(char *pokemon)
{	//lee los bloques que se corresponden a un archivo pokemon y lo retorna en una lista de posiciones
	log_info(g_logger,"READ FILE %s",pokemon); // cargar todos los bloques en un string de posiciones
	t_pokemon_medatada *pokemon_metadata = leer_metadata_pokemon(pokemon);
	char *string_posiciones = get_contenido_bloques(pokemon_metadata);
	t_list *lista_posiciones = obtener_posiciones(string_posiciones, pokemon_metadata->size);
	log_info(g_logger,"POSITIONS POKEMON ");
	for( int i = 0; i< list_size(lista_posiciones); i ++){
		t_posicion_pokemon *posicion = (t_posicion_pokemon*) list_get(lista_posiciones,i);
		log_info(g_logger," pos_x %d | pos_y %d | cantidad %d ",posicion->pos_x, posicion->pos_y, posicion->cantidad);
	}
	log_info(g_logger,"END READ FILE POKEMON %s", pokemon);
	list_destroy_and_destroy_elements(pokemon_metadata->blocks,(void*) free);
	free(pokemon_metadata);
	free(string_posiciones);
	list_destroy_and_destroy_elements(lista_posiciones,(void*) free);
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
	log_info(g_logger,"CONTENT FILE POKEMON:\n%s", str_posiciones);
	return str_posiciones;
}

char *get_contenido_bloque(int block_size,char *block)
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

t_pokemon_medatada *leer_metadata_pokemon(char *pokemon)
{	// Lee la metadata del pokemon y la carga en la estructura pokemon_metadata
	t_pokemon_medatada *pokemon_metadata = malloc(sizeof(t_pokemon_medatada));
	int cant_bloques = 0, tamano_bloque = g_config_tg->block_size, long_dir = 0, fragmentacion = 0, long_bloques = 0;
	t_config* metadata_file_info = obtengo_info_metadata(pokemon);
	char *string_bloques = config_get_string_value(metadata_file_info, "BLOCKS");
	pokemon_metadata->archivo_pokemon = pokemon;
	pokemon_metadata->directory = si_no(config_get_string_value(metadata_file_info, "DIRECTORY"));
	pokemon_metadata->open = si_no(config_get_string_value(metadata_file_info, "DIRECTORY"));
	pokemon_metadata->size = config_get_int_value(metadata_file_info, "SIZE");
	pokemon_metadata->blocks = obtengo_lista_bloques(string_bloques);
	log_info(g_logger,"METADATADA: %s | DIRECTORY: %s | SIZE: %d | OPEN: %s | BLOCKS_AMOUNT: %d",
			pokemon_metadata->archivo_pokemon, print_si_no(pokemon_metadata->directory), pokemon_metadata->size,
			print_si_no(pokemon_metadata->open),pokemon_metadata->blocks->elements_count);
	config_destroy(metadata_file_info);
	return pokemon_metadata;
}

t_config *obtengo_info_metadata(char *pokemon)
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

	/* *
	 * Buenas!
Revisando la función veo que lo que te falto es el msync() para forzar que se actualice el archivo :)

La idea de munmap() es la de terminar el mapeo del archivo en memoria, con lo cual, efectivamente no deberías ejecutarlo hasta el final del gamecard 😉

Cualquier cosa que no se entienda, avisa.
Saludos.-
	 */
