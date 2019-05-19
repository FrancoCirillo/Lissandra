/*
 * krnMock.h
 *
 *  Created on: 17 may. 2019
 *      Author: utnso
 */

#ifndef KRNMOCK_H_
#define KRNMOCK_H_

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

t_log* g_logger;
t_config* g_config;

typedef long int mseg_t;

typedef struct config{
	char* PUERTO_MEMORIA;
	char* MEMORIA_IP;
	char* RUTA_LOG;
}config;

config configuracion;
identificador* idsNuevasConexiones;
t_dictionary * conexionesActuales;

void (*callback) (instr_t* instruccion, char* remitente);

void responderHandshake(identificador* idsConexionEntrante);

void inicializar_configuracion();
void loggear(char* valor);
char* obtener_por_clave(char* ruta, char* clave);
void ejecutar_instruccion(instr_t* instruccion, char* remitente);
void ejecutar_instruccion_select(instr_t* instruccion);
void ejecutar_instruccion_devolucion_select(instr_t* instruccion);
void ejecutar_instruccion_insert(instr_t* instruccion);
void ejecutar_instruccion_create(instr_t* instruccion);
void ejecutar_instruccion_describe(instr_t* instruccion);
void ejecutar_instruccion_drop(instr_t* instruccion);
void ejecutar_instruccion_journal(instr_t* instruccion);
void ejecutar_instruccion_exito(instr_t* instruccion);
void imprimir_donde_corresponda(cod_op codigoOperacion, instr_t* instruccion, t_list * listaParam);
void responderHandshake(identificador* idsConexionEntrante);
int obtener_fd_out(char* proceso);


#endif /* KRNMOCK_H_ */
