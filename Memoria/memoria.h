#ifndef MEMORIA_MEMORIA_H_
#define MEMORIA_MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <readline/readline.h>
#include "/home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua/Herramientas/hilos.h"
#include "/home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua/Herramientas/definicionesConexiones.h"


t_log* g_logger;
t_config* g_config;

typedef struct config{
	char* PUERTO;
	char* IP_FS;
	char* PUERTO_FS;
	char* IP_SEEDS;
	char* PUERTO_SEEDS;
	int RETARDO_MEMORIA;
	int RETARDO_FS;
	int TAMANIO_MEMORIA;
	int RETARDO_JOURNAL;
	int RETARDO_GOSSIPING;
	int MEMORY_NUMBER;
	char* RUTA_LOG;
}config;

config configuracion;



void inicializarConfiguracion();
void loggear(char*);
char* getByKey(char*, char*);
/*
void leerConsola();
void reconocerRequest(char*, char*, char*, char*, char*);
instr_t *crearInstruccion(int, char*, char*, char*, char*);
void print_instruccion(instr_t*);
*/

#endif /* MEMORIA_MEMORIA_H_ */
