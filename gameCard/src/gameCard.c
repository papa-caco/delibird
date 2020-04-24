/*
 ============================================================================
 Name        : gameCard.c
 Author      : Grupo "tp-2020-1C Los Que Aprueban"
 Version     : 1.0.0
 Description :
 ============================================================================
 */
#include "utilsGc.h"
#include "servidor.h"


int main(void) {

	iniciar_log();
	int* servidor;
	int canal =1;
	int pid;


	pid= fork();

	if (pid < 0) {
			printf(stderr, "Falló\n");
			exit(-1);
	}	else if (pid == 0) {


		// Inicio de código de Proceso Hijo
		//log_info(logger,"proceso hijos socketServidor %d",*servidor);

		//suscribirse(*servidor);
		//char* mensajeRecibido = recibir_mensaje_broken(*servidor);
		//log_info(logger, "SUSCRIPCION OK %s", mensajeRecibido);
		//free(mensajeRecibido);
	//	close(conexion);

		int conexion = crear_conexion();//Crea una conexion par enviar un mensaje
		if (conexion < 0) {
			return EXIT_FAILURE;
		}

		//envio mensaje al canal suscripto
		 enviar_mensaje_al_canal("nuevo pokemon",canal,conexion);
		 int idMensaje = recibir_id_mensaje(conexion);

		log_info(logger,"IdMensaje recibido %d", idMensaje);
		close(conexion);

		// Esperar término del Hijo //

		wait(NULL);
		printf("Child Complete\n");
		exit(0);
		execlp("/bin/ls","ls",NULL);
		//
		// Fin de código de Proceso Hijo
	}	else {
			// Inicio de código de Proceso Padre
		iniciar_servidor(servidor);
			//
			// Fin de código de Proceso Padre
	}

	destruir_config();
return EXIT_SUCCESS;
	// Establecemos conexion //
	return EXIT_SUCCESS;
}
