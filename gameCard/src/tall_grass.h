/*
 * tall_grass.h
 *
 *      Author: utnso
 */

#ifndef SRC_TALL_GRASS_H_
#define SRC_TALL_GRASS_H_
#include<commons/bitarray.h>
#include <dirent.h>

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

void file_system_pokemon(char *pokemon, t_list *lista_posiciones);

char *serializar_lista_posiciones_pokemon(t_list *lista_posiciones, t_log *logger);

char *serializar_posicion_pokemon(t_coordenada *coordenada, uint32_t cant);

t_list * armar_guardar_data_bloques_file_pokemon(char *string_posiciones);

int grabar_bloque(char *block_buffer, size_t, t_log *logger);

void grabar_metadata_pokemon(t_list *blocks, char *pokemon, int size_pokemon, char *open,  t_log *logger);

void crear_dirname_pokemon(char *pokemon, t_log *logger);

void leer_metadata_tall_grass(t_log *logger);

char *obtengo_ruta_metadata_pokemon(char *pokemon);

t_config *obtengo_info_metadata(char *path_metadata);

t_list *obtengo_lista_bloques(char *string_bloques);

int valor_magic_number(char *string_fijo);

void inicializar_bitmap_tallgrass(t_log *logger);

t_list* leer_bloques(char *pokemon);

t_list *obtener_posiciones(char *string_posiciones, int long_string);

char *get_contenido_bloques(t_pokemon_medatada *pokemon_metadata, t_log *logger);

char *get_contenido_bloque(int block_size,char *block);

t_posicion_pokemon *string_to_posicion(char* str_posicion);

t_pokemon_medatada * leer_metadata_pokemon(char *pokemon, t_log *logger);

void prueba_leer_bloques_pokemon(char* pokemon);

void *abrir_archivo_bitmap(char *path, int size, t_log *logger);

void leer_contador_bloques(void);

bool incremento_contador_bloques(void);

int obtener_ultimo_nro_bloque(void);

bool si_no(char *valor);

char *print_si_no(bool valor);

char *get_dirname_pokemon(char *pokemon);

t_list* get_files_pokemon();

void destruir_lista_posiciones_bloques(t_list* lista_posiciones_bloques);

#endif /* SRC_TALL_GRASS_H_ */
