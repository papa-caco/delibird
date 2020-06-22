/*
 * tallGrass.c
 *
 *
 *      Author: utnso
 */

#include "tall_grass.h"

void prueba_file_system(void){
	/** probando filesystem */

		t_posicion_pokemon* posicion;
		t_pokemon_medatada *pokemon_metadata;
		t_list* lista_posiciones = list_create();



		posicion->pos_x = 2;
		posicion->pos_y = 10;
		posicion->cantidad = 5;

		list_add(lista_posiciones, posicion);
		file_system_pokemon(pokemon_metadata, lista_posiciones,"pikachus");
}

/**
 * Escribe los la iformacion del pokemon:
 * 				/files/pokemon/metadata.bin
 * 				/bocks/bloque.bin
 *
 * lista_posiciones tiene todas las posiciones con las cantidades econtradas
 * completa los bloques usados
 */
void file_system_pokemon(t_pokemon_medatada *pokemon_metadata, t_list *lista_posiciones, char *pokemon){

	t_list* blocks = list_create();
	int *block_nro ;

	int tamanio_remanente = 0; //Caracteres remanentes

	//char *buffer;
	char *string_posiciones;//Concatena el string a escribir en los bloques
	t_posicion_pokemon* posicion_actual;

	log_info(g_logger,"FILESYSTEM");
	leer_metadata_tall_grass();

	//t_list* posiciones =  list_map(lista_posiciones,(void*) int_to_string);
	//PasaR la lista de posiciones de enteros a una lista de posiciones de string
	for( int i=0; i<= lista_posiciones->elements_count; i++ ){
		int offset = 0;

		//void *buffer = malloc(sizeof(int));
		char *pos_x,*pos_y,*cantidad;

		posicion_actual = list_get(lista_posiciones,	i);

		int tamanio_buffer = strlen(pos_x) + strlen(pos_y) + strlen(cantidad);

		if( tamanio_remanente > 0 ){
			string_posiciones = realloc(string_posiciones,
						sizeof(tamanio_remanente) + tamanio_buffer * sizeof(char)+2 );
		}
		else{
			string_posiciones = malloc(tamanio_buffer * sizeof(char)+2);
		}

		strcpy(string_posiciones,int_to_string(posicion_actual->pos_x));
		strcat(string_posiciones,"-");
		strcat(string_posiciones,int_to_string(posicion_actual->pos_y));
		strcat(string_posiciones,"=");
		strcat(string_posiciones,int_to_string(posicion_actual->cantidad));

		div_t total_bloques;
		total_bloques.quot = 0;
		total_bloques.rem =0;
		if( tamanio_buffer >= g_config_tg->block_size ){
			total_bloques =  div(tamanio_buffer , g_config_tg->block_size );
		}

		offset =0;
		if( total_bloques.quot > 0){ //Tengo al menos un bloque para grabar
			for( int bloque=0 ; bloque <= total_bloques.quot ; bloque++ ){
				log_info(g_logger,"BLOQUE %d %d",bloque,total_bloques);
				char *block_buffer = malloc(sizeof(g_config_tg->block_size));

				*block_nro = bloque; // TODO cambiar por get_bloque_libre();

				strncpy(block_buffer,  &string_posiciones[offset], g_config_tg->block_size);
				offset += g_config_tg->block_size;

				grabar_bloque(*block_nro,block_buffer);

				free(block_buffer);
				list_add(blocks, block_nro);
			}
			log_info(g_logger,"FIN FOR");
			if( total_bloques.rem > 0 ){ //Solo dejo el remanente que queda en el buffer de posiciones
				log_info(g_logger,"ESTAREMOS ACA rem %d %d %d",total_bloques.rem,tamanio_buffer,offset);

				int tamanio_remanente = total_bloques.rem ; // tamanio_buffer - offset; //caracteres remanentes en la cola del buffer
				char *caracteres_remanentes = malloc(sizeof(tamanio_remanente));
				strncpy(caracteres_remanentes, &string_posiciones[offset], tamanio_remanente);
				log_info(g_logger,"FIN DEL REMANTENTES %s", caracteres_remanentes);
				string_posiciones = realloc(string_posiciones, sizeof(tamanio_remanente) );
				*string_posiciones = *caracteres_remanentes;
				free(caracteres_remanentes);
				log_info(g_logger,"FIN DEL REMANTENTES");
			}
			else{
				free(string_posiciones);
			}
			log_info(g_logger,"FIN DEL IF");
		}


	}
	//TODO
	// SI AL FINALIZAR TODA LA LISTA QUEDA UN REMANENTE HAY QUE GUARDAR SOLO ESE REMANENTE

	log_info(g_logger,"aca");
	free(string_posiciones);
	log_info(g_logger,"aca");
	list_add(pokemon_metadata->blocks, blocks);
	log_info(g_logger,"aca");
	grabar_metadata_pokemon(pokemon_metadata, pokemon);
//	liberar_lista_posiciones(string_posiciones);
	log_info(g_logger,"FIN");
}


/**
 * Graba un bloque en el file system del tamanio inticado en el archivo de configuracion
 */
void grabar_bloque(int block_nro, char *block_buffer){

	char * file_block = int_to_string(block_nro);

	char* path_blocks = malloc(strlen(g_config_gc->path_blocks) + strlen(file_block) +4 );
	strcpy(path_blocks, g_config_gc->path_blocks);
	strcat(path_blocks, file_block);
	strcat(path_blocks, ".bin");

	FILE* fd = fopen(path_blocks,"w");
	fwrite(block_buffer,sizeof(char),g_config_tg->block_size,fd);
	fclose(fd);

	log_info(g_logger,"SAVE BLOCK SUCESS %s ", path_blocks);
}

/**
 * Graba la metadata del pokemon, cierra el achivoy sete OPEN=N
 */
void grabar_metadata_pokemon(t_pokemon_medatada *pokemon_metadata, char *pokemon){

	crear_dirname_pokemon(pokemon);
log_info(g_logger,"iniciando metadata_pokemon");
	char* path_metadata = malloc(sizeof(g_config_gc->path_files) + sizeof(pokemon)+12);

	strcpy(path_metadata, g_config_gc->path_files);
	strcpy(path_metadata, pokemon);
	strcpy(path_metadata, "metadata.bin");

	char *str_blocks;

	FILE* fd = fopen(path_metadata,"wb");
	fprintf(fd, "DIRECTORY=%s\n", pokemon_metadata->directory);
	fprintf(fd, "SIZE=%d\n", pokemon_metadata->size);
	fprintf(fd, "BLOCKS=[");
	int *buffer = malloc(sizeof(int));
	for(int i=0 ; i <= pokemon_metadata->blocks->elements_count;  i++ ){
		int *bloque_actual = list_get(pokemon_metadata->blocks,	i);

	//	memcpy(buffer, &(bloque_actual), sizeof(int));

		fprintf(fd, "%d", *bloque_actual);
		if( i < pokemon_metadata->blocks->elements_count )
			fprintf(fd,",");
	}
	free(buffer);
	fprintf(fd, "]\n");
	fprintf(fd, "blocks=%s\n", str_blocks);
	fprintf(fd, "OPEN=N\n");
	fclose(fd);

	log_info(g_logger,"UPDATE METADATA POKEMON SUCESS| Pokemenon %s | file %s",pokemon,path_metadata);
}

void crear_dirname_pokemon(char *pokemon){
	log_info(g_logger,"iniciando metadata_pokemon");
	char* pathPokemon = malloc(strlen(g_config_gc->path_files) + sizeof(pokemon));
	log_info(g_logger,"iniciando metadata_pokemon");
	strcpy(pathPokemon, g_config_gc->path_files);
	strcat(pathPokemon, pokemon);

	struct stat st = { 0 };
	if (stat(pathPokemon, &st) == -1) {
		log_info(g_logger, "CREATE_DIR Pokemon");
		mkdir(pathPokemon, 0774);
	}

}

void leer_metadata_tall_grass(){
	t_config* g_config;

	g_config = config_create(g_config_gc->file_metadata);
	g_config_tg = malloc(sizeof(t_config_tall_grass));

	g_config_tg->block_size = config_get_int_value(g_config,"BLOCK_SIZE");
	g_config_tg->blocks = config_get_int_value(g_config,"BLOCKS");
	g_config_tg->magic_number = config_get_int_value(g_config,"MAGIC_NUMBER");

}
/**
 * Retorna el primer bloque libre que se tiene
 * Debe tener en cosideracion los bloques que ya se tienen asignado.
 * Debe tener en consideracion si faltan o sobran bloques para pedirlos o liberarlos
 */
void  get_bloque_libre(){
	//TODO
}

/**
 * lee los bloques que se corresponden a un archivo pokemon y lo retorna en una lista de posiciones
 *
 */
t_pokemon_posiciones* leer_bloques(){
	//TODO
}

char* int_to_string(int x){
    char* buffer = malloc(sizeof(char) * sizeof(int) * 4 + 1);
    if (buffer) {
         sprintf(buffer, "%d", x);
    }
    return buffer;
}
