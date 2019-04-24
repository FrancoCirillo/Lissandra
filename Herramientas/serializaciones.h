//-------serializaciones.h-------

#ifndef SERIALIZACIONES_H_
#define SERIALIZACIONES_H_

#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <commons/log.h>
#include <readline/readline.h>
#include "tiempo.h"
#include "definicionesConexiones.h"


void* serializar_paquete(t_paquete*, int);
void crear_buffer(t_paquete*);
t_paquete* crear_paquete(int, time_t);
void eliminar_paquete(t_paquete*);
void agregar_a_paquete(t_paquete*, void*, int);
void* recibir_buffer(int*, int);
t_list* recibir_paquete(int);
int recibir_request(int, instr_t**);
int recibir_timestamp(int, time_t*);
int recibir_operacion(int, cod_op*);
void print_instruccion(instr_t* instruccion);


/*
void * serializar_instruccion(instr_t *instruccion, int* tamanioTotalInstruccion);
instr_t* deserializar_instruccion(void* instruccion_a_parsear);
t_list* recibir_paquete(int socket_cliente);
void eliminar_instruccion(instr_t* instruccion);
void print_instruccion(instr_t* instruccion);
void reconocerRequest(char* comando, char* param1, char* param2, char* param3, char* param4);
instr_t *crearInstruccion(int codigoRequest, char* p1, char* p2, char* p3, char* p4);
void enviar_instruccion_socket(instr_t* instruccion, int socket_cliente);
*/

#endif /* SERIALIZACIONES_H_ */
