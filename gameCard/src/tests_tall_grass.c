/*
 * tests_tall_grass.c
 *
 *  Created on: 19 jul. 2020
 *      Author: utnso
 */

#include "tall_grass.h"

int leer_bloques(char *pokemon, t_log *logger);

void prueba_file_system(char* pokemon, int cant_posiciones);

void prueba_leer_bloques_pokemon(char* pokemon);

void file_system_pokemon(char *pokemon, int cant_posiciones);

//-------------------------------------------------------------------//


int leer_bloques(char *pokemon, t_log *logger)
{	//lee los bloques que se corresponden a un archivo pokemon y lo retorna en una lista de posiciones
	pthread_mutex_lock(&g_mutex_envio); //lee los bloques que se corresponden a un archivo pokemon
	log_info(logger,"READ FILE %s",pokemon); // y lo retorna en una lista de posiciones
	int data_size = 0, id_msj = 0;
	t_pokemon_medatada *pokemon_metadata = leer_metadata_pokemon(pokemon, logger);
	if (pokemon_metadata != NULL) {
	char *string_posiciones = get_contenido_bloques(pokemon_metadata);
	t_list *lista_posiciones = obtener_posiciones(string_posiciones, pokemon_metadata->size);
	pthread_mutex_unlock(&g_mutex_envio);
	t_list *coordenadas = list_create();
	for( int i = 0; i< list_size(lista_posiciones); i ++){
		t_posicion_pokemon *posicion = (t_posicion_pokemon*) list_get(lista_posiciones,i);
		id_msj = enviar_appeared_pokemon_broker(pokemon, posicion->cantidad, posicion->pos_x, posicion->pos_y, logger);
		if (nro_par(posicion->pos_x)) {
			id_msj = enviar_caught_pokemon_broker(posicion->pos_x, OK, logger);
		} else {
			id_msj = enviar_caught_pokemon_broker(posicion->pos_x, FAIL, logger);
		}
		t_coordenada *coordenada = malloc(sizeof(t_coordenada));
		coordenada->pos_x = posicion->pos_x;
		coordenada->pos_y = posicion->pos_y;
		void *coord = (t_coordenada*) coordenada;
		list_add(coordenadas, coord);
	}
	pthread_mutex_lock(&g_mutex_envio);
	log_trace(logger,"END READ FILE POKEMON %s", pokemon);
	pthread_mutex_unlock(&g_mutex_envio);
	data_size = pokemon_metadata->size;
	list_destroy_and_destroy_elements(pokemon_metadata->blocks,(void*) free);
	free(pokemon_metadata);
	free(string_posiciones);
	list_destroy_and_destroy_elements(lista_posiciones,(void*) free);
	return data_size;
	} else {
		return -1;
	}
}

void prueba_file_system(char* pokemon, int cant_posiciones)
{
	pthread_mutex_lock(&g_mutex_envio);
	file_system_pokemon(pokemon, cant_posiciones);
	log_info(g_logger,"FIN PRUEBA GUARDAR ARCHIVO\n");
	pthread_mutex_unlock(&g_mutex_envio);
}

void prueba_leer_bloques_pokemon(char* pokemon)
{
	 leer_bloques(pokemon, g_logger);
}


/**
 *crea el filesystem del pokeno: metadata.bin y bloque.bin
 * 1 Armar un string, del tamanio del bloque con todas las lista de posiciones que entren
 * 2 Conforme se llena string se guarda en el bloque
 */

void file_system_pokemon(char *pokemon, int cant_posiciones)
{  // -->>Esta vuela: las posiciones las tiene que cargar/actualizar de a una <<--
	log_trace(g_logger,"CREATE FILESYSTEM %s",pokemon);
	int lng_str_posiciones = 0;
	char *string_posiciones;
	t_list* lista_posiciones = list_create(), *blocks;
	for (int i = 0; i < cant_posiciones; i ++) {
		t_posicion_pokemon *posicion = malloc(sizeof(t_posicion_pokemon));
		posicion->pos_x = 15 + i * 3;
		posicion->pos_y = 20 * i;
		posicion->cantidad = (i +1) * 100;
		void *posi = (t_posicion_pokemon*) posicion;
		list_add(lista_posiciones, posicion);
	}
	string_posiciones = serializar_lista_posiciones_pokemon(lista_posiciones, g_logger);
	lng_str_posiciones = string_length(string_posiciones);
	blocks = armar_guardar_data_bloques_file_pokemon(string_posiciones);
	grabar_metadata_pokemon(blocks, pokemon, lng_str_posiciones,"N", g_logger);
	list_destroy(blocks);
	list_destroy_and_destroy_elements(lista_posiciones,(void*) free);
}
