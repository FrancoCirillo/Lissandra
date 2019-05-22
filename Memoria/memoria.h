//---------memoria.h---------

#ifndef MEMORIA_MEMORIA_H_
#define MEMORIA_MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include "../Herramientas/serializaciones.h"
#include "../Herramientas/definicionesConexiones.h"
#include "../Herramientas/utilsServidor.h"
#include "../Herramientas/utilsCliente.h"
#include "../Herramientas/tiempo.h"
#include "componentes.h"


		/* LOGGER Y CONFIG */

t_log *g_logger;
t_config *g_config;

typedef struct config
{
	char *PUERTO;
	char *IP_FS;
	char *PUERTO_FS;
	char *IP_SEEDS;
	char *PUERTO_SEEDS;
	int RETARDO_MEMORIA;
	int RETARDO_FS;
	int TAMANIO_MEMORIA;
	int RETARDO_JOURNAL;
	int RETARDO_GOSSIPING;
	int MEMORY_NUMBER;
	char *RUTA_LOG;
} config;

config configuracion;


		/* CONEXIONES */

identificador *idsNuevasConexiones;
t_dictionary *conexionesActuales;
char nombreDeMemoria[12];
char miIPMemoria[18];


		/* DECLARACION DE FUNCIONES */

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


#endif /* MEMORIA_MEMORIA_H_ */
