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

void* serializar_instruccion(instr_t* instruccion, int tamanioParam1, int tamanioParam2, int tamanioParam3, int tamanioParam4);
void agregar_a_paquete(void** paquete, void* valor, int tamanio, int* desplazamiento);
void deserializar_instruccion(void* instruccion, instr_t** instruccionFinal);
void enviar_instruccion(instr_t* instruccion, int socket_cliente);
int recibir_paquete(int socket_cliente, instr_t ** instruccion);
void eliminar_instruccion(instr_t* instruccion);

#endif /* SERIALIZACIONES_H_ */
