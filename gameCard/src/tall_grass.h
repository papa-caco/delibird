/*
 * tall_grass.h
 *
 *      Author: utnso
 */

#ifndef SRC_TALL_GRASS_H_
#define SRC_TALL_GRASS_H_
#include<commons/bitarray.h>

#include "utils_gc.h"
#define TALL_GRASS 555



//Estructura del metadata
typedef struct tall_grass {
	int block_size;
	int blocks;
	int tamano_fs;
	int magic_number;
} t_config_tall_grass;

//Estructura de la metadata del pokemon
typedef struct pokemon_metadada
{
	char *archivo_pokemon;
	bool directory;
	bool open;
	int size;
	t_list* blocks;
} t_pokemon_medatada;


//Lista de todas las posiciones de los pokemons
typedef struct pokemon_posiciones{
	t_list *posiciones; //guarda las posicione t_posicion_pokemon
	uint32_t cantidad_elementos;
	uint32_t bytes;
}t_pokemon_posiciones;

typedef struct bitmap_tall_grass{
	t_bitarray *bitmap; //guarda las posicione t_posicion_pokemon
	int nro_bloque;
	int total_blocks;
	int used_blocks;
	void* bits_mmap_file;
}t_bitmap_fs;

//Variables globales

t_config_tall_grass *g_config_tg;

t_bitmap_fs *g_bitmap_bloques;

//Funciones

void prueba_file_system(char* pokemon, int cant_posiciones);

void file_system_pokemon(char *pokemon, int cant_posiciones);

t_list * armar_guardar_data_bloques_file_pokemon(char *string_posiciones);

void grabar_bloque(int block_nro, char *block_buffer);

void grabar_metadata_pokemon(t_list *blocks, char *pokemon, int size_pokemon, char *open);

void crear_dirname_pokemon(char *pokemon);

void leer_metadata_tall_grass(t_log *logger);

t_config *obtengo_info_metadata(char *pokemon);

t_list *obtengo_lista_bloques(char *string_bloques);

int valor_magic_number(char *string_fijo);

void inicializar_bitmap_tallgrass(t_log *logger);

void leer_bloques( char *pokemon);

t_list *obtener_posiciones(char *string_posiciones, int long_string);

char *get_contenido_bloques(t_pokemon_medatada *pokemon_metadata);

char *get_contenido_bloque(int block_size,char *block);

t_posicion_pokemon *string_to_posicion(char* str_posicion);

t_pokemon_medatada * leer_metadata_pokemon(char *pokemon);

void prueba_leer_bloques_pokemon(char* pokemon);

void *abrir_archivo_bitmap(char *path, int size, t_log *logger);

void leer_contador_bloques(void);

void incremento_contador_bloques(void);

bool si_no(char *valor);

char *print_si_no(bool valor);

#endif /* SRC_TALL_GRASS_H_ */
