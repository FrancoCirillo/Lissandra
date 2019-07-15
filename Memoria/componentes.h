//--------componentes.h--------

#ifndef COMPONENTES_H_
#define COMPONENTES_H_

#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "admin.h"
#include "../Herramientas/definicionesConexiones.h"
#include "../Herramientas/serializaciones.h"
#include "instrucciones.h"

void *memoriaPrincipal;
t_dictionary *tablaDeSegmentos;
sem_t mutex_diccionario_conexiones;
sem_t mutex_config;

/* MEMORIA PRINCIPAL */
void gran_malloc_inicial();
void inicializar_sectores_memoria();
void *insertar_instruccion_en_memoria(instr_t* instruccion, int* nroPag);
void actualizar_pagina(void* paginaAActualizar, mseg_t nuevoTimestamp, char* nuevoValue);
registro *obtener_registro_de_instruccion(instr_t *instruccion);
int get_proximo_sector_disponible();
bool memoria_esta_full();

/* TABLA DE PAGINAS */
filaTabPags *agregar_fila_tabla(t_list * tablaDePaginas, int numPag, void* pagina, bool flagMod);
t_list *nueva_tabla_de_paginas();
bool tabla_de_paginas_full(t_list* tablaDePaginas);
bool memoria_esta_full();


/* TABLA DE SEGMENTOS */
void inicializar_tabla_segmentos();
t_list * segmento_de_esa_tabla(char* tabla);
void eliminar_tabla(instr_t* instruccion);

/* GETTERS */

mseg_t get_ts_pagina(void* pagina);
uint16_t get_key_pagina(void* pagina);
char* get_value_pagina(void* pagina);

registro *obtener_registro_de_pagina(void* pagina);

char* pagina_a_str(void* pagina);
char* registro_a_str(registro* registro);
void loggear_tabla_de_paginas(t_list *tablaDePaginas, void (*funcion_log)(char *texto));


/* ALGORITMO DE REEMPLAZO */

t_list* paginasSegunUso;
void se_utilizo(filaTabPags* filaUsada);
void se_uso(int paginaUsada);
int* pagina_lru();
void sacar_de_lista_lru(int pagina);
void sacar_segmento_de_lru(t_list* segmentoABorrar);

filaTabPags* fila_correspondiente_a_esa_pagina(int numeroDePagina, int *indiceEnTabla, char** segmentoQueLaTiene);
filaTabPags* fila_con_el_numero(t_list *suTablaDePaginas, int numeroDePagina, int* indiceEnTabla);
filaTabPags* fila_con_la_key(t_list *suTablaDePaginas, uint16_t keyBuscada);


/* JOURNAL */
sem_t mutex_journal;
void ejecutar_instruccion_journal(instr_t *instruccion, int liberar);
void *ejecutar_journal(); //Job para el journal
instr_t	 *fila_a_instr(char* tablaAInsertar, filaTabPags* fila, cod_op codOp);
instr_t *registro_a_instr(char* tablaAInsertar, registro* unRegistro, cod_op codOp);
void limpiar_memoria();
void limpiar_memoria_principal();
void limpiar_segmentos();
void liberar_value(t_list* instruccion);
void liberar_key(t_list* listaParam);
/* SHOW */
void mostrar_paginas(instr_t* instruccion);
void imprimir_segmento(char* nombreSegmento, t_list* suTablaDePaginas);
void imprimir_segmento_basico(char* nombreSegmento, t_list* suTablaDePaginas);

#endif //COMPONENTES_H
