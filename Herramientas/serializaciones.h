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
#include <commons/collections/dictionary.h>
#include "tiempo.h"
#include "definicionesConexiones.h"

void *serializar_paquete(t_paquete *, int);
void crear_buffer(t_paquete *);
t_paquete *crear_paquete(cod_op nuevoCodOp, mseg_t nuevoTimestamp);
void eliminar_paquete(t_paquete *);
void agregar_a_paquete(t_paquete *, void *, int);
void *recibir_buffer(int *, int);
t_list *recibir_paquete(int);
int recibir_request(int, instr_t **);
int recibir_timestamp(int, time_t *);
int recibir_operacion(int, cod_op *);
t_paquete *instruccion_a_paquete(instr_t *instruccionAEnviar);
void *serializar_request(instr_t *instruccionAEnviar, int *tamanio);
int enviar_request(instr_t *instruccionAEnviar, int socket_cliente);
instr_t *crear_instruccion(time_t timestampNuevo, cod_op codInstNuevo, t_list *listaParamNueva);
instr_t *leer_a_instruccion(char *request, int enviadoPorConsola);
cod_op reconocer_comando(char *comando);
void print_instruccion(instr_t *instruccion);
cod_op quienEnvio(instr_t *instruccion);
void imprimir_registro(instr_t *instruccion);
void loggear_exito(instr_t *miInstruccion);
void loggear_error(instr_t *miInstruccion);

void imprimirConexiones(t_dictionary *);

#endif /* SERIALIZACIONES_H_ */
