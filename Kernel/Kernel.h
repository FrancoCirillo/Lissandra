/*
 * kernel.h
 *
 *  Created on: 1 apr. 2019
 *      Author: rodrigo diaz
 */

#ifndef Kernel_H_
#define Kernel_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/config.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<stdio.h>
#include<readline/readline.h>
#include <stdio.h>
#include <string.h>
#include "/home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua/Herramientas/utilsCliente.h"
#include "/home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua/Herramientas/utilsServidor.h"
#include <pthread.h>

t_log* g_logger;
t_config* g_config;

//typedef enum {NEW,READY,EXEC,EXIT}estado;

typedef struct instruccion_t{
	int codigo_instruccion;
	time_t timestamp;
	char* param1;
	char* param2;
	char* param3;
	char* param4;
	struct instruccion_t* sig;
}instruccion_t;


typedef struct proceso{
	instruccion_t* current;
	instruccion_t *instrucciones;
	struct proceso* sig;
}proceso;



typedef struct config{
	int quantum;
	int gradoMultiprocesamiento;
	char* rutaLog;
	char* ip;
	char* puerto;

}config_t;

typedef struct response{
	int codigoRespuesta;
	char* descripcion;
	int tipoEnviado;

}response;

typedef struct metricas{
	int cantidadInsert;
	int cantidadSelect;
	clock_t tiempoInsert;
	clock_t tiempoSelect;
}metricas;

typedef struct memoria{
	int ip;
	int puerto;
}memoria;

metricas m;
proceso* cola_ready=NULL;
config_t configuracion;
int total_hilos=0;



//Proceso principal
void* ejecutar_proceso(void* un_proceso);
instruccion_t* ejecutar_instruccion(instruccion_t* instruccion);
instruccion_t* enviar_i(instruccion_t* i);
void finalizar_proceso(proceso* p);
void encolar_o_finalizar_proceso(proceso* p);

//Getter y setters
instruccion_t *obtener_instruccion(proceso* p);
proceso* obtener_sig_proceso();
memoria obtenerMemoria(instruccion_t* instr);
char* obtener_por_clave(char* ruta, char* key);
void encolar_proceso(proceso *p);

//Herramientas
int hilos_disponibles();
void inicializarConfiguracion();
void loggear(char* mensaje);
void informarMetricas();

/*
proceso* popProceso();
void escucharYEncolarProcesos();

void leerProcesosDesdeConsola();
int realizar_proceso(proceso *unProceso );
instruccion* obtenerInstruccion(proceso *unProceso);
response* mandar_instruccion(instruccion *i);

char* getByKey(char* ruta, char* buscado);
void informarMetricas();
void inicializarMemorias();
void inicializarMetricas();
*/
#endif /* kernel.h */
