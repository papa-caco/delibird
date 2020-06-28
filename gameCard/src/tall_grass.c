/*
 * tallGrass.c
 *
 *
 *      Author: utnso
 */

#include "tall_grass.h"

void prueba_file_system(void){

	t_list* lista_posiciones = list_create();
	t_posicion_pokemon *posicion2 = malloc(sizeof(t_posicion_pokemon));
	t_posicion_pokemon *posicion1 = malloc(sizeof(t_posicion_pokemon));
	posicion2->pos_x = 23;
	posicion2->pos_y = 10;
	posicion2->cantidad = 5;
	list_add(lista_posiciones, posicion2);

	posicion1->pos_x = 15;
	posicion1->pos_y = 3 ;
	posicion1->cantidad = 1;
	list_add(lista_posiciones, posicion1);

	file_system_pokemon( lista_posiciones,"Pikachu");

	liberar_lista_posiciones(lista_posiciones);
	log_info(g_logger,"FIN PRUEBA");
}

void prueba_leer_bloques_pokemon(){
	t_list* lista_posiciones = list_create();
	lista_posiciones = leer_bloques("Pikachu");
}



/**
 *crea el filesystem del pokeno: metadata.bin y bloque.bin
 * 1 Armar un string, del tamanio del bloque con todas las lista de posiciones que entren
 * 2 Conforme se llena string se guarda en el bloque
 */
void file_system_pokemon( t_list *lista_posiciones, char *pokemon){
	log_info(g_logger,"CREATE FILESYSTEM %s",pokemon);

	leer_metadata_tall_grass();
	t_list *blocks = list_create();
	int x = 1;
	int *nro_bloque;
	nro_bloque =&x;//TODO geg_bloque;

	char *string_posiciones = (char*) calloc(g_config_tg->block_size,sizeof(char));
	int size_pokemon =0;
	for( int i=0; i< list_size(lista_posiciones); i++ ){
		t_posicion_pokemon *posicion = malloc(sizeof(t_posicion_pokemon));
		posicion = list_get(lista_posiciones,i);

		log_info(g_logger,"PROCESS POSICION %d-%d=%d",posicion->pos_x,posicion->pos_y,posicion->cantidad);

		/*char *pos_x = string_new();//(char*) calloc(string_length(string_itoa(posicion->pos_x)),sizeof(char));
		char *pos_y = string_new();//(char*) calloc(string_length(string_itoa(posicion->pos_y)),sizeof(char));
		char *cantidad = string_new();//(char*) calloc(string_length(string_itoa(posicion->cantidad)),sizeof(char));
		*/
		char *pos_x = string_itoa(posicion->pos_x);
		char *pos_y = string_itoa(posicion->pos_y);
		char *cantidad =string_itoa(posicion->cantidad);
		string_append(&pos_x,"-");
		string_append(&pos_y,"=");
		string_append(&cantidad,"\n");

		size_pokemon += string_length(pos_x) + string_length(pos_y)+string_length(cantidad);

		string_append(&string_posiciones,pos_x);
		armar_bloque(string_posiciones,nro_bloque ,blocks);

		string_append(&string_posiciones,pos_y);
		armar_bloque(string_posiciones,nro_bloque ,blocks);

		string_append(&string_posiciones,cantidad);
		armar_bloque(string_posiciones,nro_bloque ,blocks);

	}
	//Se guarda el remanente
	for( int i= 0 ; i < string_length(string_posiciones) ; i+= g_config_tg->block_size ){
		armar_bloque(string_posiciones,nro_bloque ,blocks);
	}

	grabar_metadata_pokemon(blocks, pokemon,size_pokemon,"N");
	free(string_posiciones);
	list_destroy(blocks);
}

/**
 * recibe un string, evalua la longitud y guardar el bloque
 * retorna el remanente del string que no entra el bloque
 */
void armar_bloque(char *string_bloque ,int *nro_bloque, t_list *blocks){

	if( (string_length(string_bloque) ) >= g_config_tg->block_size ){
		char *grabar = string_substring(string_bloque,0,g_config_tg->block_size);

		grabar_bloque(*nro_bloque,grabar );
		int i = (*nro_bloque);
		list_add(blocks,&i);//TODO PEDIR BLOQUE VALIDO
		(*nro_bloque)++;

		char *substring = string_new();//(char*)calloc( (string_length(string_bloque) - g_config_tg->block_size),sizeof(char) );
		substring = string_substring(string_bloque,g_config_tg->block_size,
									string_length(string_bloque) - g_config_tg->block_size);

		free(string_bloque);
		string_bloque =(char*)calloc( g_config_tg->block_size ,sizeof(char) );
		string_append(&string_bloque,substring);
	}

}

/**
 * Graba un bloque en el file system
 */
void grabar_bloque(int block_nro, char *block_buffer){

	char * file_block = string_itoa(block_nro);

	char* path_blocks = string_new();//malloc(strlen(g_config_gc->dirname_blocks) + strlen(file_block) +4 );
	string_append(&path_blocks, g_config_gc->dirname_blocks);
	string_append(&path_blocks, file_block);
	string_append(&path_blocks, ".bin");

	FILE* fd = fopen(path_blocks,"w");
	fwrite(block_buffer,sizeof(char),g_config_tg->block_size,fd);
	fclose(fd);

	log_info(g_logger,"SAVE BLOCK | CONTENT %s | FILE %d.bin",block_buffer, block_nro);
}


/**
 * Graba la metadata del pokemon(metadata.bin)
 */
void grabar_metadata_pokemon(t_list *blocks, char *pokemon, int size_pokemon, char *open){
	log_info(g_logger,"iniciando metadata_pokemon");
	crear_dirname_pokemon(pokemon);

	char* dirname_metatada_pokemon = string_new();
	string_append(&dirname_metatada_pokemon,g_config_gc->dirname_files);
	string_append(&dirname_metatada_pokemon,pokemon);
	string_append(&dirname_metatada_pokemon,"/Metadata.bin");

	FILE* fd = fopen(dirname_metatada_pokemon,"wb");
	fprintf(fd, "DIRECTORY=N\n");
	fprintf(fd, "SIZE=%d\n", size_pokemon);
	fprintf(fd, "BLOCKS=[");

	for(int i=0 ; i < list_size(blocks);  i++ ){

		int *bloque_actual = list_get(blocks,i);
		//log_info(g_logger,"valor bloque %d",*bloque_actual);
		fprintf(fd, "%d", *bloque_actual);
		if( i < (list_size(blocks) -1) )
			fprintf(fd,",");
	}

	fprintf(fd, "]\n");
	fprintf(fd, "OPEN=N\n");
	fclose(fd);

	log_info(g_logger,"SAVE METADATA  %s",dirname_metatada_pokemon);
}

void crear_dirname_pokemon(char *pokemon){

	char* pathPokemon = string_new();//malloc(strlen(g_config_gc->dirname_files) + sizeof(pokemon));

	string_append(&pathPokemon, g_config_gc->dirname_files);
	string_append(&pathPokemon, pokemon);

	struct stat st = { 0 };
	if (stat(pathPokemon, &st) == -1) {
		log_info(g_logger, "CREATE_DIR Pokemon %s",pathPokemon);
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
 * 1 cargar todos los bloques en un string de posiciones
 * 2 pasar del string de posiciones a un lista con la estructura de posiciones
 * 3 fin
 *
 */
t_list * leer_bloques( char *pokemon){
	log_info(g_logger,"READ FILE %s",pokemon);

	t_list *lista_posiciones = list_create();
	t_pokemon_medatada *pokemon_metadata =leer_metadata_pokemon(pokemon);
	char *string_posiciones = get_contenido_bloques(pokemon_metadata);

	lista_posiciones = obtener_posiciones(string_posiciones);


	log_info(g_logger,"POSITIONS POKEMON ");
	for( int i=0; i< list_size(lista_posiciones); i++){
		t_posicion_pokemon *posicion;
		posicion = list_get(lista_posiciones,i);
		log_info(g_logger," pox_x %d | pos_y %d | cantidad %d ",posicion->pos_x, posicion->pos_y, posicion->cantidad);
	}

	log_info(g_logger,"END READ FILE POKEMON %s",pokemon);
	return lista_posiciones;
}

/**
 * Retorna una estructura de posiciones a partir de un string de posiciones
 */
t_list *obtener_posiciones(char *string_posiciones){
	t_list *lista_posiciones = list_create();

	//Pasar todo el string de posiciones una lista con la estructura de posiciones
	char **lineas = string_split(string_posiciones, "\n");
	int cant_lines = sizeof(lineas) / sizeof(lineas[0]);
	for(int i=0 ; i< cant_lines; i++ ){
		if(lineas[i] != NULL ){
			t_posicion_pokemon *posicion = string_to_posicion( lineas[i]);
			list_add(lista_posiciones,posicion);
		}
	}
	return lista_posiciones;
}

/**
 * Retorna un string con el contenido de los bloques del pokemon
 */
char *get_contenido_bloques(t_pokemon_medatada *pokemon_metadata){

	leer_metadata_tall_grass();
	char *str_posiciones = string_new();
	int leer_size = 0;
	for(int i=0 ; i<= list_size(pokemon_metadata->blocks) ; i++ ){
		//Cargar todos los bloques en un string de posiciones
		if(list_get(pokemon_metadata->blocks,i) != NULL){
			char *str_bloque = string_new();
			if( (i+1)* g_config_tg->block_size < pokemon_metadata->size   ){
				leer_size = g_config_tg->block_size;
			}
			else{
				leer_size = (i+1)* g_config_tg->block_size - pokemon_metadata->size ;
			}

			str_bloque = get_contenido_bloque( leer_size,list_get(pokemon_metadata->blocks,i) );
			string_append(&str_posiciones,str_bloque);
			free(str_bloque);
		}
	}
	log_info(g_logger,"CONTENT FILE POKEMON => %s",str_posiciones);
	return str_posiciones;
}

/**
 * Retorna el contenido de un bloque en un string
 */
char *get_contenido_bloque(int block_size,char *block){
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

/**
 * hace un split del string de la posicion y retorna la estructura de la posicion
 */
t_posicion_pokemon *string_to_posicion(char* str_posicion){
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


/**
 * Lee la metadata del pokemon y la carga en la estructura pokemon_metadata
 */
t_pokemon_medatada * leer_metadata_pokemon(char *pokemon){
	t_pokemon_medatada *pokemon_metadata;

	char* dirname_metatada_pokemon = string_new();
			//malloc(sizeof(g_config_gc->dirname_files) + string_length(pokemon) + string_length(name_metadata)+1);
	string_append(&dirname_metatada_pokemon,g_config_gc->dirname_files);
	string_append(&dirname_metatada_pokemon,pokemon);
	string_append(&dirname_metatada_pokemon,"/Metadata.bin");

	t_config* g_config;

	g_config = config_create(dirname_metatada_pokemon);

	pokemon_metadata = malloc(sizeof(t_pokemon_medatada));

	pokemon_metadata->directory = config_get_string_value(g_config, "DIRECTORY");

	pokemon_metadata->size = config_get_int_value(g_config, "SIZE");
	pokemon_metadata->open = config_get_string_value(g_config, "OPEN");
	pokemon_metadata->file_metatada_pokemon = dirname_metatada_pokemon;
	pokemon_metadata->blocks = list_create();

	char **nodeBlockTupleAsArray = string_get_string_as_array(config_get_string_value(g_config, "BLOCKS"));

	for(int bloque=0 ; bloque<  sizeof(nodeBlockTupleAsArray); bloque++ ){;
		if(nodeBlockTupleAsArray[bloque] != NULL){
			list_add(pokemon_metadata->blocks,nodeBlockTupleAsArray[bloque]);
		}
	}

	log_info(g_logger,"METADATADA POKEMON  DIRECTORY: %s | SYZE: %d | OPEN: %s | BLOCKS: %s",
			pokemon_metadata->directory, pokemon_metadata->size, pokemon_metadata->open ,
			config_get_string_value(g_config, "BLOCKS") );

	return pokemon_metadata;
}

