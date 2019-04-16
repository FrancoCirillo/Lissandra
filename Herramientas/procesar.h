/*
 * procesar.h
 *
 *  Created on: 15 abr. 2019
 *      Author: utnso
 */

#ifndef PROCESAR_H_
#define PROCESAR_H_


#include<stdio.h>
#include<stdlib.h>
#include <pthread.h>


typedef struct inbox{
	instr_t* instruccion;
	instr_t* sig;
}inbox;

typedef struct instr_t{
	time_t timestamp;
	int codigoInstruccion;
	char* param1;
	char* param2;
	char* param3;
	char* param4;
}instr_t;
typedef struct remitente_t{
	char* ip;
	char* puerto;
}remitente_t;
typedef struct remitente_instr_t{
	instr_t *instruccion;
	remitente_t *remitente;
}remitente_instr_t;


#endif /* PROCESAR_H_ */
