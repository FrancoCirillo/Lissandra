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
//INSERT [NOMBRE_TABLA] [KEY] “[VALUE]” [Timestamp]     <- opcional el timestamp?
//CREATE [NOMBRE_TABLA] [TIPO_CONSISTENCIA] [NUMERO_PARTICIONES] [COMPACTION_TIME]
//DESCRIBE [NOMBRE_TABLA]    <- opcional el param.
//DROP [NOMBRE_TABLA]

void (*callback) (instr_t* instruccion, int conexionReceptor);
void ejecutar_instruccion(instr_t* instruccion, int conexionReceptor);
void ejecutar_instruccion_select(instr_t* instruccion, int conexionReceptor);
void ejecutar_instruccion_insert(instr_t* instruccion, int conexionReceptor);
void ejecutar_instruccion_create(instr_t* instruccion, int conexionReceptor);
void ejecutar_instruccion_describe(instr_t* instruccion, int conexionReceptor);
void ejecutar_instruccion_drop(instr_t* instruccion, int conexionReceptor);
void enviar_a_quien_corresponda(cod_op codigoOperacion, instr_t* instruccion, t_list * listaParam, int conexionReceptor);

#endif /* FILE_SYSTEM_H */
