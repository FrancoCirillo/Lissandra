//---------memoria.h---------

#ifndef MEMORIA_MEMORIA_H_
#define MEMORIA_MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
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
#include "misConexiones.h"
#include "instrucciones.h"
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

char *obtener_por_clave(char *ruta, char *clave);

void ejecutar_instruccion(instr_t *instruccion, char *remitente);


	/* TESTING */

void check_inicial(int argc, char* argv[]);

#endif /* MEMORIA_MEMORIA_H_ */
