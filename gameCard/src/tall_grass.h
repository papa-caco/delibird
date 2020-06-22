/*
 * tall_grass.h
 *
 *      Author: utnso
 */

#ifndef SRC_TALL_GRASS_H_
#define SRC_TALL_GRASS_H_
#include "utils_gc.h"


//Estructura del metadata
typedef struct tall_grass {
	int block_size;
	int blocks;
	int magic_number;
} t_config_tall_grass;

//Estructura de la metadata del pokemon
typedef struct pokemon_metadada{
	char *directory;
	int size;
	t_list* blocks;
	char open;
} t_pokemon_medatada;


//Lista de todas las posiciones de los pokemons
typedef struct pokemon_posiciones{
	t_list *posiciones; //guarda las posicione t_posicion_pokemon
	uint32_t cantidad_elementos;
	uint32_t bytes;
}t_pokemon_posiciones;

//Variables globales
t_config_tall_grass *g_config_tg;

//Funciones


void file_system_pokemon(t_pokemon_medatada *pokemon_metadata, t_list *lista_posiciones, char *pokemon);
void grabar_bloque(int block_nro, char *block_buffer);
void grabar_metadata_pokemon(t_pokemon_medatada *pokemon_metadata, char *pokemon);
void crear_dirname_pokemon(char *pokemon);
void leer_metadata_tall_grass();
void  get_bloque_libre();
t_pokemon_posiciones* leer_bloques();
char* int_to_string(int x);

void prueba_file_system(void);


#endif /* SRC_TALL_GRASS_H_ */
