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
#include "admin.h"

		/* CONEXIONES */

identificador *idsNuevasConexiones;
t_dictionary *conexionesActuales;
char miIPMemoria[18];
char nombreDeMemoria[12];
int tamanioValue;
int tamanioRegistro;
bool* sectorOcupado;
int cantidadDeSectores;

	/* CONFIG Y LOG */

t_log *g_logger;
t_config *g_config;
config configuracion;
void inicializar_configuracion();
void iniciar_log();
void iniciar_config();

		/* DECLARACION DE FUNCIONES */

void (*callback)(instr_t *instruccion, char *remitente);

void responderHandshake(identificador *idsConexionEntrante);

char *obtener_por_clave(char *ruta, char *clave);

void ejecutar_instruccion(instr_t *instruccion, char *remitente);
void ejecutar_instruccion_select(instr_t *instruccion);
void ejecutar_instruccion_devolucion_select(instr_t *instruccion);
void ejecutar_instruccion_insert(instr_t *instruccion);
void ejecutar_instruccion_create(instr_t *instruccion);
void ejecutar_instruccion_describe(instr_t *instruccion);
void ejecutar_instruccion_drop(instr_t *instruccion);
void ejecutar_instruccion_exito(instr_t *instruccion);

	/* TESTING */

void check_inicial(int argc, char* argv[]);

#endif /* MEMORIA_MEMORIA_H_ */
