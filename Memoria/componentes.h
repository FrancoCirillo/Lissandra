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
t_dictionary *tablaDeSegmentos;

void gran_malloc_inicial();
void *insertar_instruccion_en_memoria(instr_t* instruccion, int* nroPag);
registro *obtener_registro_de_instruccion(instr_t *instruccion);
int get_proximo_sector_disponible();

void agregar_fila_tabla(t_list * tablaDePaginas, int numPag, void* pagina, bool flagMod);
t_list *nueva_tabla_de_paginas();

int contadorPaginas;

void inicializar_tabla_segmentos();

mseg_t get_ts_pagina(void*pagina);
uint16_t get_key_pagina(void*pagina);
char* get_value_pagina(void*pagina);
registro *obtener_registro_de_pagina(void*pagina);
char* pagina_a_str(void* pagina);
char* registro_a_str(registro* registro);
void imprimir_tabla_de_paginas(t_list *tablaDePaginas);

#endif //COMPONENTES_H
