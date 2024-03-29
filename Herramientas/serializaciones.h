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
#include "logging.h"
#include "tiempo.h"
#include "definicionesConexiones.h"

enum cantidadParametros{
    CANT_PARAM_SELECT = 2,
    CANT_PARAM_INSERT = 3,
    CANT_PARAM_INSERT_COMPLETO = 4,
    CANT_PARAM_CREATE = 4,
    CANT_PARAM_DESCRIBE = 0,
	CANT_PARAM_DESCRIBE_COMPLETO = 1,
    CANT_PARAM_DROP = 1,
    CANT_PARAM_JOURNAL = 0, 
    CANT_PARAM_ADD = 4,
    CANT_PARAM_METRICS = 0, 
    CANT_PARAM_RUN = 1,
	CANT_PARAM_CERRAR = 0
};

/*
 * SERIALIZACIONES
 */

/*
 * Crea un t_paquete con los campos timestamp = nuevoTimestamp y codigo_operacion = nuevoCodOp
 * Llama a crear_buffer para el campo buffer
 */
t_paquete *crear_paquete(cod_op nuevoCodOp, mseg_t nuevoTimestamp);


/*
 * Hace malloc del buffer de paquete
 */
void crear_buffer(t_paquete *paquete);


/*
 * Hace free de todos los campos del paquete
 */
void eliminar_paquete(t_paquete *paquete);


/*
 * Agrega valor de tamanio tamanio en el buffer->stream de paquete
 * Actualiza buffer->size
 *
 * En buffer->stream se guardan los valores y los tamanios de los valores de manera consecutiva
 * En buffer->size indica el tamanio total del buffer, esto es en tamanio de los valores + n*int
 *
 * Ej:
 * 		buffer: unValor7otroValor9
 * 		size: 7+9+(2*sizeof(int))
 *
 */
void agregar_a_paquete(t_paquete *paquete, void *valor, int tamanio);


/*
 *  Adapta instr_t a t_paquete, que es el tipo que necesitamos
 */
t_paquete *instruccion_a_paquete(instr_t *instruccionAEnviar);


/*
 * Serializa un t_paquete de tamanio bytes
 *
 */
void *serializar_paquete(t_paquete *paquete, int bytes);


/*
 * Serializa una instr_t.
 * Devuelve en tamanio el tamanio serializado
 *
 * Dada la instruccion, la obtiene como paquete llamando a instruccion_a_paquete
 * Ese paquete lo serializa con serializar_paquete y lo devuelve
 *
 */
void *serializar_request(instr_t *instruccionAEnviar, int *tamanio);


/*
 * ENVIO
 */

/*
 * Envia instruccionAEnviar a travez de socket_cliente
 * Hace free(instruccionAEnviar)
 */
int enviar_request(instr_t *instruccionAEnviar, int socket_cliente);


/*
 * Envia instruccionAEnviar a travez de socket_cliente
 * Llama a enviar_request pero ademas hace free de los parametros de instr
 */
int enviar_liberando_request(instr_t *instruccionAEnviar, int socket_cliente);


/*
 * Igual que enviar_request, pero no libera nada
 * Sirve para el Kernel, que se guarda las instrucciones
 */
int enviar_request_simple(instr_t *instruccionAEnviar, int socket_cliente);

/*
 * RECEPCION
 */

/*
 * Recibe en *instruccion la instr_t del socket_cliente
 *
 * Primero recibe el ts, despues el codigo de operacion y por ultimo los parametros
 * Si hay un error al recibir el ts o la operación devuelve el el código de error
 * Devuelve 1 en exito
 */
int recibir_request(int socket_cliente, instr_t **instruccion);

/*
 * Recibe mseg_t bytes de socket_cliente
 *
 * Devuelve el resultado del recv
 */
int recibir_timestamp(int socket_cliente, mseg_t *nuevoTimestamp);

/*
 * Recibe cod_op bytes del socket cliente, el Codigo de Operacion.
 *
 * Devuelve el resultado del recv
 */
int recibir_operacion(int socket_cliente, cod_op *nuevaOperacion);


/*
 * recibe un t_buffer de socket_cliente y lo devuelve como void*
 */
void *recibir_buffer(int *size, int socket_cliente);


/*
 * Por mas que se llame recibir paquete, lo que hace es
 * recibir_buffer y lo deserializa en t_list
 */
void recibir_paquete(int socketCliente, t_list* valores);


/*
 * MANEJO DE INSTRUCCIONES
 */

/*
 * Devuelve una nueva (malloc) instr_t con los valores dados
 */
instr_t *crear_instruccion(mseg_t timestampNuevo, cod_op codInstNuevo, t_list *listaParamNueva);


/*
* Lee request y lo guarda en una nueva instr_t (malloc en crear_instruccion)
*
* En queConsola va CONSOLA_KERNEL, CONSOLA_MEMORIA o CONSOLA_FS, según desde qué consola
* se haga el input.
*
* Devuelve la instruccion creada, con la BASE_CONSOLA sumada al comando reconocido
*/
instr_t *leer_a_instruccion(char *request, int queConsola);


/*
 * Pasa comando de char* a cod_op
 */
cod_op reconocer_comando(char *comando);


/*
 * Devuelve 0 si la cantidad de parametros permitidos de comando
 * coinciden con el tamanio de listaParam
 */
int es_comando_valido(cod_op comando, t_list *listaParam);


/*
 * Imprime instruccion en stdout
 */
void imprimir_instruccion(instr_t *instruccion, void (*funcion_log)(char *texto));


/*
 * Devuelve CONSOLA_KERNEL, CONSOLA_MEMORIA o CONSOLA_FS según el cod_op de instruccion
 *
 * Usado con las otras funciones devuelve qué consola fue la que realizó el input,
 * por lo tanto sirve para saber en qué consola imprimir la instruccion
 */
cod_op quien_pidio(instr_t *instruccion);


/*
 *  Imprime un registro presente en instr_t (siempre y cuando Key y Value sean char*)
 */
void imprimir_registro(instr_t *instruccion, void (*funcion_log)(char *texto));


/*
 * Imprime el primer parametro de instruccion
 */
void loggear_exito_proceso(instr_t *miInstruccion);


/*
 * Imprime el primer parametro de instruccion, en rojo
 */
void loggear_error_proceso(instr_t *miInstruccion);

/*
 * Imprime un diccionario de conexiones
 */
void imprimir_conexiones(t_dictionary *diccionario, void (*funcion_log)(char *texto));

/*
 * Conversion de char* a uint16_t
 * Sirve para poder usar el parametro Key
 *
 */
bool str_to_uint16(char *str, uint16_t *res);

/*
 * Conversion de uint16_t a char*
 * La usamos para manejarnos siempre con char* en los parametros de las instr_t*
 */
void uint16_to_str(uint16_t key, char** keyChar);

/*
 * Devuelve el último parametro de la instruccion.
 * NULL si no tiene parametros.
 * Sirve porque en el último parámetro guardamos qué hilo del Kernel lo envió
 */

void* obtener_ultimo_parametro(instr_t* instruccion);

/*
 * Para trabajar mas comodos al leer un congfig usando config_get_array_value
 * Devuelve una lista nueva.
 */
t_list* string_array_to_list(char** stringArray);

#endif /* SERIALIZACIONES_H_ */
