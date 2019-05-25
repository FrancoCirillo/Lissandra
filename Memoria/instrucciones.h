/*
 * instrucciones.h
 *
 *  Created on: 24 may. 2019
 *      Author: utnso
 */

#ifndef INSTRUCCIONES_H_
#define INSTRUCCIONES_H_

#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include "../Herramientas/serializaciones.h"
#include "../Herramientas/definicionesConexiones.h"
#include "../Herramientas/utilsServidor.h"
#include "../Herramientas/utilsCliente.h"
#include "misConexiones.h"
#include "componentes.h"

void ejecutar_instruccion_select(instr_t *instruccion);
void ejecutar_instruccion_devolucion_select(instr_t *instruccion);
void ejecutar_instruccion_insert(instr_t *instruccion);
void ejecutar_instruccion_create(instr_t *instruccion);
void ejecutar_instruccion_describe(instr_t *instruccion);
void ejecutar_instruccion_drop(instr_t *instruccion);
void ejecutar_instruccion_exito(instr_t *instruccion);
void ejecutar_instruccion_journal(instr_t *instruccion);
void ejecutar_instruccion_error(instr_t * instruccion);

#endif /* INSTRUCCIONES_H_ */
