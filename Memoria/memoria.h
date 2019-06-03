//---------memoria.h---------

#ifndef MEMORIA_MEMORIA_H_
#define MEMORIA_MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
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
#include "admin.h"
#include "componentes.h"
#include "misConexiones.h"
#include "instrucciones.h"

	/* CONFIG Y LOG */

t_log *g_logger;
t_log *debug_logger;
t_config *g_config;
config configuracion;
void inicializar_configuracion();
void iniciar_log();
char *obtener_por_clave(char *ruta, char *clave);


	/* CONEXIONES */

identificador *idsNuevasConexiones;
t_dictionary *conexionesActuales;
char miIPMemoria[18];
char nombreDeMemoria[12];
int listenner;
int tamanioValue;
int tamanioRegistro;
bool* sectorOcupado;
int cantidadDeSectores;

	/* CONEXIONES */

void inicializar_estructuras_conexiones();
void empezar_conexiones(char *argv[]);
void (*callback)(instr_t *instruccion, char *remitente);

	/* PROCESO MEMORIA */
void inicializar_estructuras_memoria();
void ejecutar_instruccion(instr_t *instruccion, char *remitente);
void inicializar_semaforos();
void iniciar_ejecutador_journal();

	/* TESTING */

void check_inicial(int argc, char* argv[]);

#endif /* MEMORIA_MEMORIA_H_ */
