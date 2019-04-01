/*
 * kernel.h
 *
 *  Created on: 1 apr. 2019
 *      Author: rodrigo diaz
 */

#ifndef TP0_H_
#define TP0_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<stdio.h>
#include "utils.h"
#include<readline/readline.h>
#include <stdio.h>
enum estado(NEW,READY,EXEC,EXIT);
struct{
	estado estadoActual;
	int current;
	instruccion* listaInstrucciones;
}proceso;

struct {
	char* instr;
	int tiempoEnvio;
	int tiempoRespuesta;
}instruccion;

struct {
	int quantum;
	int codigoError;
	int gradoMultiprocesamiento;
	int codigoInsert;
	int codigoSelect;
	int ip;
	int puerto;
}config;

struct {
	int codigoRespuesta;
	char* descripcion;
	int tipoEnviado;

}response;

struct {
	int cantidadInsert;
	int cantidadSelect;
	listaInts tiemposInserts;
	listaInts tiemposSelects
}metricas;

struct {
	int ip;
	int puerto;
}memoria;

//metricas son globales
metricas m;
//lista procesos es global
proceso* listaProcesos;
//criterios de memorias son globales


//liberar free()

#endif /* kernel.h */
