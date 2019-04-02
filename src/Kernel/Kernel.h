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
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<stdio.h>
//#include "utils.h"
#include<readline/readline.h>
#include <stdio.h>
#include <string.h>

typedef enum {NEW,READY,EXEC,EXIT}estado;

typedef struct instruccion{
	char* instr;
	clock_t tiempoEnvio;
	clock_t tiempoRespuesta;
}instruccion;
typedef struct listaInstruccion{
	instruccion instruccion;
	instruccion* sig;
}listaInstruccion;
typedef struct proceso{
	estado estadoActual;
	int current;
	listaInstruccion *listaInstrucciones;
}proceso;


typedef struct listadoProcesos{
	proceso p;
	struct listadoProcesos *sig;
}listadoProcesos;

typedef struct config{
	int quantum;
	int codigoError;
	int gradoMultiprocesamiento;
	int codigoInsert;
	int codigoSelect;
	char* ip;
	char* puerto;
}config;

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

//metricas son globales
metricas m;
//lista procesos es global
listadoProcesos* listaProcesos;
//criterios de memorias son globales

//configuracion es global
config configuracion;

//liberar free()
proceso* popProceso();
void escucharYEncolarProcesos();
void informarMetricas();
void leerProcesosDesdeConsola();
int realizar_proceso(proceso *unProceso );
memoria obtenerMemoria(instruccion instr);
instruccion* obtenerInstruccion(proceso *unProceso);
response* enviar_mensaje(instruccion instr);
void metricarInsert(clock_t tiempoRespuesta);
void loggear(char* mensaje);
char* getByKey(char* ruta, char* buscado);
void informarMetricas();
void inicializarMemorias()
#endif /* kernel.h */
