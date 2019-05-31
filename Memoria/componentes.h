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

/* MEMORIA PRINCIPAL */
void gran_malloc_inicial();
void *insertar_instruccion_en_memoria(instr_t* instruccion, int* nroPag);
void actualizar_pagina(void* paginaAActualizar, mseg_t nuevoTimestamp, char* nuevoValue);
registro *obtener_registro_de_instruccion(instr_t *instruccion);
int get_proximo_sector_disponible();
bool memoria_esta_full();
void ejecutar_journal();

/* TABLA DE PAGINAS */
filaTabPags *agregar_fila_tabla(t_list * tablaDePaginas, int numPag, void* pagina, bool flagMod);
t_list *nueva_tabla_de_paginas();
bool tabla_de_paginas_full(t_list* tablaDePaginas);
bool memoria_esta_full();


/* TABLA DE SEGMENTOS */
void inicializar_tabla_segmentos();
t_list * segmento_de_esa_tabla(char* tabla);


/* GETTERS */

mseg_t get_ts_pagina(void* pagina);
uint16_t get_key_pagina(void* pagina);
char* get_value_pagina(void* pagina);

registro *obtener_registro_de_pagina(void* pagina);

char* pagina_a_str(void* pagina);
char* registro_a_str(registro* registro);
void imprimir_tabla_de_paginas(t_list* tablaDePaginas);


/* ALGORITMO DE REEMPLAZO */

t_list* paginasSegunUso;
void se_utilizo(filaTabPags* filaUsada);
void se_uso(int paginaUsada);
int* pagina_lru();

filaTabPags* fila_correspondiente_a_esa_pagina(int numeroDePagina, int *indiceEnTabla, char** segmentoQueLaTiene);
filaTabPags* fila_con_el_numero(t_list *suTablaDePaginas, int numeroDePagina, int* indiceEnTabla);
filaTabPags* fila_con_la_key(t_list *suTablaDePaginas, uint16_t keyBuscada);


/* JOURNAL */
void ejecutar_instruccion_journal(instr_t *instruccion);
instr_t	 *fila_a_instr(filaTabPags* fila, cod_op codOp);
instr_t *registro_a_instr(registro* unRegistro, cod_op codOp);

#endif //COMPONENTES_H
