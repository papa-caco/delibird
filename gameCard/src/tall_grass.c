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



		posicion->pos_x = 23;
		posicion->pos_y = 10;
		posicion->cantidad = 5;

		list_add(lista_posiciones, posicion);
		posicion->pos_x = 2;
		posicion->pos_y = 3 ;
		posicion->cantidad = 1;
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
	int block_nro ;
	int bk = 1; //simula un nro correlativo de bloque

	char *string_posiciones;//Concatena el string a escribir en los bloques
	int tamanio_remanente = 0; //Caracteres remanentes

	t_posicion_pokemon* posicion_actual;

	log_info(g_logger,"FILESYSTEM");
	leer_metadata_tall_grass();

	//t_list* posiciones =  list_map(lista_posiciones,(void*) int_to_string);
	//PasaR la lista de posiciones de enteros a una lista de posiciones de string
	log_info(g_logger,"cant pos %d",lista_posiciones->elements_count);
	for( int i=0; i<= lista_posiciones->elements_count; i++ ){
		log_info(g_logger,"procesando pos %d | tam_remain %d | %d",i, tamanio_remanente,lista_posiciones->elements_count);
		int offset = 0;

		char *pos_x,*pos_y,*cantidad;

		posicion_actual = list_get(lista_posiciones,	i);

		pos_x = int_to_string(posicion_actual->pos_x);
		pos_y = int_to_string(posicion_actual->pos_y);
		cantidad = int_to_string(posicion_actual->cantidad);

		int tamanio_buffer = strlen(pos_x) + strlen(pos_y) + strlen(cantidad);

		if( tamanio_remanente > 0 ){
			//Se anexan los caracteres remanentes
			log_info(g_logger,"hay REMANENTE ");
			if( i == lista_posiciones->elements_count ){
				log_info(g_logger,"GRABANDO REMANENTE ");
			/*	*block_nro = get_bloque_libre(i+1,5);

				grabar_bloque(*block_nro,string_posiciones);
				continue;*/
				continue;
			}
			else{
				string_posiciones = realloc(string_posiciones,	strlen(string_posiciones) * sizeof(char) + tamanio_buffer * sizeof(char)+3 );
				strcat(string_posiciones,"\n");
				strcat(string_posiciones,pos_x);
			}
		}
		else{
			string_posiciones = malloc(tamanio_buffer * sizeof(char)+2);
			strcpy(string_posiciones,pos_x);
		}


		strcat(string_posiciones,"-");
		strcat(string_posiciones,pos_y);
		strcat(string_posiciones,"=");
		strcat(string_posiciones,cantidad);
		tamanio_buffer = strlen(string_posiciones);

		div_t total_bloques;
		total_bloques.quot = 0;
		total_bloques.rem =0;
		if( tamanio_buffer >= g_config_tg->block_size ){
			total_bloques =  div(tamanio_buffer , g_config_tg->block_size );
		}


		log_info(g_logger,"String %s| count_character %d | block_size %d | blocks_need %d | char_remain %d",
					string_posiciones, tamanio_buffer,g_config_tg->block_size,total_bloques.quot, total_bloques.rem);

		offset =0;
		if( total_bloques.quot > 0){ //Tengo al menos un bloque para grabar
			for( int bloque=1 ; bloque <= total_bloques.quot ; bloque++ ){
				char *block_buffer = malloc(sizeof(g_config_tg->block_size));

				block_nro = bk; //TODO get_bloque_libre()

				strncpy(block_buffer,  &string_posiciones[offset], g_config_tg->block_size);
				offset += g_config_tg->block_size;

				grabar_bloque(block_nro,block_buffer);

				free(block_buffer);
				list_add(blocks, &block_nro);
				bk++;
			}
			free(string_posiciones);
			if( total_bloques.rem > 0 ){ //Solo dejo el remanente que queda en el buffer de posiciones
				//log_info(g_logger,"cant_remanente %d | characters %s",total_bloques.rem,&string_posiciones[offset]);
				tamanio_remanente = total_bloques.rem;
				char *caracteres_remanentes2 = malloc(total_bloques.rem * sizeof(char));

				strncpy(caracteres_remanentes2, &string_posiciones[offset], total_bloques.rem);
				string_posiciones = malloc( total_bloques.rem * sizeof(char) );

				strncpy(string_posiciones,caracteres_remanentes2, tamanio_remanente);
				log_info(g_logger,"remains %s ",string_posiciones);
				free(caracteres_remanentes2);
			}

		}


	}
	//free(caracteres_remanentes);
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

	log_info(g_logger,"SAVE BLOCK SUCESS | contenido %s |file %d.bin",block_buffer, block_nro);
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
int  get_bloque_libre(int i, int j){
	//TODO

	int bloque;



	bloque =   i*j;

	return bloque;
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
