/*
 ============================================================================
 Name        : gameCard.c
 Author      : Grupo "tp-2020-1C Los Que Aprueban"
 Version     : 1.0.0
 Description :
 ============================================================================
 */
// #include "utilsGc.h"
#include "utils_gc.h"
#include "tall_grass.h"

pthread_t thread;


int main(void) {
	iniciar_gamecard();
	inicio_server_gamecard();
}


