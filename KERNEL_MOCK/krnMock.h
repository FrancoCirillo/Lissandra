/*
 * krnMock.h
 *
 *  Created on: 17 may. 2019
 *      Author: utnso
 */

#ifndef KRNMOCK_H_
#define KRNMOCK_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <sys/stat.h>
#include "../Herramientas/definicionesConexiones.h"
#include "../Herramientas/utilsServidor.h"
#include "../Herramientas/utilsCliente.h"
#include "../Herramientas/tiempo.h"

//Cambiar para testear con las memorias que se quiera
#define RECEPTOR_SELECT 3
#define RECEPTOR_INSERT 8
#define RECEPTOR_CREATE 8
#define RECEPTOR_DESCRIBE 8
#define RECEPTOR_DROP 9
#define RECEPTOR_JOURNAL 3

t_log *g_logger;
t_config *g_config;
//Testing
int conexion_con_memoria_3;
int conexion_con_memoria_8;
int conexion_con_memoria_9;

typedef long int mseg_t;

typedef struct config
{
	char *PUERTO_MEMORIA;
	char *MEMORIA_3_IP;
	char *MEMORIA_8_IP;
	char *MEMORIA_9_IP;
	char *RUTA_LOG;
} config;

config configuracion;
t_dictionary *conexionesActuales;

void (*callback)(instr_t *instruccion, char *remitente);

void responderHandshake(identificador *idsConexionEntrante);

void inicializar_configuracion();
void loggear(char *valor);
char *obtener_por_clave(char *ruta, char *clave);
void ejecutar_instruccion(instr_t *instruccion, char *remitente);
void ejecutar_instruccion_select(instr_t *instruccion);
void ejecutar_instruccion_devolucion_select(instr_t *instruccion);
void ejecutar_instruccion_insert(instr_t *instruccion);
void ejecutar_instruccion_create(instr_t *instruccion);
void ejecutar_instruccion_describe(instr_t *instruccion);
void ejecutar_instruccion_drop(instr_t *instruccion);
void ejecutar_instruccion_journal(instr_t *instruccion);
void ejecutar_instruccion_exito(instr_t *instruccion);
void imprimir_donde_corresponda(cod_op codigoOperacion, instr_t *instruccion, t_list *listaParam);
void responderHandshake(identificador *idsConexionEntrante);
int obtener_fd_out(char *proceso);

#endif /* KRNMOCK_H_ */
