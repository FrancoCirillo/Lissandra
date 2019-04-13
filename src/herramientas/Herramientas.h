/*
 * Herramientas.h
 *
 *  Created on: 7 abr. 2019
 *      Author: rodrigo diaz
 */

#ifndef HERRAMIENTAS_HERRAMIENTAS_H_
#define HERRAMIENTAS_HERRAMIENTAS_H_

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
#include "utils.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct instr{
	int codigoInstruccion;
	char* param1;
	char* param2;
	char* param3;
	char* param4;
}instr;


char* substr(const char *src, int m, int n);
char* serializar(instr *x);
int deserializar(char* mensaje,instr *outPut);
char* obtener_tiempo_cadena();
time_t obtener_tiempo();
double segundos_entre(time_t tiempo1,time_t tiempo2);
#endif /* HERRAMIENTAS_HERRAMIENTAS_H_ */
