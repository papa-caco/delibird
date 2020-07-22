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
typedef struct pokemon_posiciones
{
	t_list *posiciones; //guarda las posicione t_posicion_pokemon
	uint32_t cantidad_elementos;
	uint32_t bytes;
}t_pokemon_posiciones;

typedef struct bitmap_tall_grass
{
	t_bitarray *bitmap;
	int nro_bloque;
	int total_blocks;
	int used_blocks;
	void* bits_mmap_file;
}t_bitmap_fs;

typedef struct archivo_pokemon
{
	char *pokemon;
	t_pokemon_medatada *metadata;
	t_list* posiciones;
} t_archivo_pokemon;

typedef struct operaciones_tallgrass
{
	uint32_t id_correlativo;
	char *pokemon;
	t_posicion_pokemon *posicion;
	t_tipo_mensaje id_cola;
}t_operacion_tallgrass;


//-------------------------Variables globales-----------------------------------------//

t_config_tall_grass *g_config_tg;

t_bitmap_fs *g_bitmap_bloques;

t_list *g_archivos_abiertos;

t_list *g_reintentos;

pthread_mutex_t g_mutex_tallgrass;

pthread_mutex_t g_mutex_reintentos;

pthread_mutex_t g_mutex_cnt_blocks;

pthread_mutex_t g_mutex_open_files_list;

//------------------------------Funciones-----------------------------------------------//

char *serializar_lista_posiciones_pokemon(t_list *lista_posiciones, t_log *logger);

char *serializar_posicion_pokemon(t_coordenada *coordenada, uint32_t cant);

t_list * armar_guardar_data_bloques_file_pokemon(char *string_posiciones);

int grabar_bloque(char *block_buffer, size_t, t_log *logger);

void eliminar_bloques_archivo(t_archivo_pokemon *archivo);

void eliminar_bloque(char *bloque);

char *obtengo_path_bloque(char * file_block);

void grabar_metadata_pokemon(t_list *blocks, char *pokemon, int size_pokemon, char *open,  t_log *logger);

char *generar_string_bloques_metadata(t_list * blocks);

void crear_dirname_pokemon(char *pokemon, t_log *logger);

void leer_metadata_tall_grass(t_log *logger);

char *obtengo_ruta_metadata_pokemon(char *pokemon);

t_list *obtengo_lista_bloques(char *string_bloques);

int valor_magic_number(char *string_fijo);

void inicializar_bitmap_tallgrass(t_log *logger);

int leer_bloques(char *pokemon, t_log *logger);

int crear_archivo_pokemon(char *pokemon, t_posicion_pokemon *posicion, t_log *logger);

t_archivo_pokemon *abrir_archivo_pokemon(char *pokemon, t_log *logger);

void cerrar_archivo_pokemon(t_archivo_pokemon *archivo, t_log *logger);

t_list *obtener_coordenadas_archivo_pokemon(t_archivo_pokemon *archivo, t_log *g_logger);

t_list *obtener_posiciones(char *string_posiciones, int long_string);

bool existe_posicion_en_archivo(t_archivo_pokemon *archivo, t_posicion_pokemon *posicion);

int agregar_nueva_posicion_en_archivo(t_archivo_pokemon *archivo, t_posicion_pokemon *posicion, t_log *logger);

int modificar_posicion_en_archivo(t_archivo_pokemon *archivo, t_posicion_pokemon *posicion, t_log *logger);

int eliminar_posicion_en_archivo(t_archivo_pokemon *archivo, t_posicion_pokemon *posicion, t_log *logger);

int actualizar_posiciones_archivo(t_archivo_pokemon *archivo, t_posicion_pokemon *posicion, t_log *logger);

char *get_contenido_bloques(t_pokemon_medatada *pokemon_metadata);

char *get_contenido_bloque(int block_size,char *block);

t_posicion_pokemon *string_to_posicion(char* str_posicion);

t_pokemon_medatada *leer_metadata_pokemon(char *pokemon,  t_log *logger);

void modificar_metadata_pokemon(char *pokemon, char *clave, char *valor);

bool existe_archivo_pokemon(char *pokemon);

bool esta_abierto_archivo_pokemon(char *pokemon);

void quitar_de_lista_archivos_abiertos(char *pokemon);

void *abrir_archivo_bitmap(char *path, int size, t_log *logger);

void leer_contador_bloques(void);

bool incremento_contador_bloques(void);

void liberar_nro_bloque_bitmap(int block_nro);

int obtener_ultimo_nro_bloque(void);

bool si_no(char *valor);

char *print_si_no(bool valor);

#endif /* SRC_TALL_GRASS_H_ */
