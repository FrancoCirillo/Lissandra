//--------componentes.h--------

#ifndef COMPONENTES_H_
#define COMPONENTES_H_

#include <stdlib.h>
#include <stdio.h>
#include "admin.h"
#include "../Herramientas/definicionesConexiones.h"
#include "../Herramientas/serializaciones.h"
#include "instrucciones.h"

void *memoriaPrincipal;

void gran_malloc_inicial();
void insertar_instruccion(instr_t* instruccion);
registro *obtener_registro(instr_t *instruccion);
int get_proximo_sector_disponible();

#endif //COMPONENTES_H
