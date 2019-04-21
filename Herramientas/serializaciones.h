/*
 * serializaciones.h
 *
 *  Created on: 15 abr. 2019
 *      Author: utnso
 */

#ifndef SERIALIZACIONES_H_
#define SERIALIZACIONES_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include"definicionesConexiones.h"
#include<commons/log.h>
#include "tiempo.h"
#include<readline/readline.h>

void * serializar_instruccion(instr_t *instruccion, int* tamanioTotalInstruccion);
instr_t* deserializar_instruccion(void* instruccion_a_parsear);
int recibir_paquete(int socket_cliente, instr_t ** instruccion);
void eliminar_instruccion(instr_t* instruccion);
void print_instruccion(instr_t* instruccion);
void reconocerRequest(char* comando, char* param1, char* param2, char* param3, char* param4);
instr_t *crearInstruccion(int codigoRequest, char* p1, char* p2, char* p3, char* p4);
void enviar_instruccion_socket(instr_t* instruccion, int socket_cliente);

#endif /* SERIALIZACIONES_H_ */
