//--------componentes.h--------

#ifndef COMPONENTES_H_
#define COMPONENTES_H_

#include <stdlib.h>
#include <stdio.h>
#include "admin.h"
#include "../Herramientas/definicionesConexiones.h"
#include "../Herramientas/serializaciones.h"
void *memoriaPrincipal;

void gran_malloc_inicial();
void insertar_instruccion(instr_t* instruccion);

#endif //COMPONENTES_H
