//-------fileSystem.h-------

#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "/home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua/Herramientas/tiempo.h"
#include "/home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua/Herramientas/definicionesConexiones.h"
#include "/home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua/Herramientas/utilsCliente.h"
#include "/home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua/Herramientas/utilsServidor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <sys/stat.h>

//ORDEN PARAMETROS

//SELECT [NOMBRE_TABLA] [KEY]
//INSERT [NOMBRE_TABLA] [KEY] “[VALUE]” [Timestamp]
//CREATE [NOMBRE_TABLA] [TIPO_CONSISTENCIA] [NUMERO_PARTICIONES] [COMPACTION_TIME]
//DESCRIBE [NOMBRE_TABLA]
//DROP [NOMBRE_TABLA]

t_log* g_logger;
t_config* g_config;
t_dictionary * conexionesActuales;

typedef struct config{
	char* PUERTO_ESCUCHA;
	char TAMANIO_VALUE;
	char TIEMPO_DUMP;
	char* RUTA_LOG;
}config;

config configuracion;

void loggear(char* valor);
void inicializar_configuracion();
char* obtener_por_clave(char* ruta, char* clave);

void (*callback) (instr_t* instruccion, int fdEntrante);

void responderHandshake(identificador* idsConexionEntrante);

void ejecutar_instruccion(instr_t* instruccion, int conexionReceptor);
void ejecutar_instruccion_select(instr_t* instruccion);
void ejecutar_instruccion_insert(instr_t* instruccion);
void ejecutar_instruccion_create(instr_t* instruccion);
void ejecutar_instruccion_describe(instr_t* instruccion);
void ejecutar_instruccion_drop(instr_t* instruccion);
void imprimir_donde_corresponda(cod_op codigoOperacion, instr_t* instruccion, t_list * listaParam);
int obtener_fd_out(char* proceso);

#endif /* FILE_SYSTEM_H */
