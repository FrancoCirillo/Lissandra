/*
 * conexiones.h
 *
 *  Created on: 3 mar. 2019
 *      Author: utnso
 */

#ifndef UTILS_SERVIDOR
#define UTILS_SERVIDOR

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<string.h>
#include"definicionesConexiones.h"

#define IP "127.0.0.1"
#define PUERTO "4444"

typedef struct instr{
	int codigoInstruccion;
	char* param1;
	char* param2;
	char* param3;
	char* param4;
}instr;
char* substr(const char *src, int m, int n);
int deserializar(char* mensaje,instr* outPut);

t_log* logger;

void* recibir_buffer(int*, int);

int iniciar_servidor(void);
int esperar_cliente(int);
t_list* recibir_paquete(int);
void recibir_mensaje(int);
int recibir_operacion(int);

#endif /* UTILS_SERVIDOR */
