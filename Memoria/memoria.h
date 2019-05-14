//---------memoria.h---------

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
#include "/home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua/Herramientas/serializaciones.h"
#include "/home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua/Herramientas/definicionesConexiones.h"
#include "/home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua/Herramientas/utilsServidor.h"
#include "/home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua/Herramientas/utilsCliente.h"

t_log* g_logger;
t_config* g_config;

typedef long int mseg_t;

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


void (*callback) (instr_t* instruccion, int conexionReceptor);

void inicializar_configuracion();
void loggear(char* valor);
char* obtener_por_clave(char* ruta, char* clave);

void ejecutar_instruccion(instr_t* instruccion, int conexionReceptor);
void ejecutar_instruccion_select(instr_t* instruccion, int conexionReceptor);
void ejecutar_instruccion_devolucion_select(instr_t* instruccion, int conexionReceptor);
void ejecutar_instruccion_insert(instr_t* instruccion, int conexionReceptor);
void ejecutar_instruccion_create(instr_t* instruccion, int conexionReceptor);
void ejecutar_instruccion_describe(instr_t* instruccion, int conexionReceptor);
void ejecutar_instruccion_drop(instr_t* instruccion, int conexionReceptor);
void ejecutar_instruccion_journal(instr_t* instruccion, int conexionReceptor);
void ejecutar_instruccion_exito(instr_t* instruccion, int conexionReceptor);


#endif /* MEMORIA_MEMORIA_H_ */
