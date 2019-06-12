/*MEMTABLE*/
#ifndef MEMTABLE_H_
#define MEMTABLE_H_

#include "fileSystem.h"
#include "registros.h"


t_dictionary* memtable;
t_dictionary* tablas_nro_dump;
sem_t mutex_tablas_nro_dump;

// --------FUNCIONES DE MEMTABLE--------
void levantar_tablas_directorio();
void inicializar_memtable();
void finalizar_memtable();
void limpiar_memtable();

// --------FUNCIONES DE REGISTROS--------
t_list* 	crear_lista_registros();
void 		borrar_lista_registros(void*);
void 		agregar_registro(char*, registro_t*);
void 		borrar_registro(void*);
registro_t* pasar_a_registro(instr_t*);
t_list* 	obtener_registros_mem(char*, uint16_t);

////VAN EN FILE SYSTEM!!
//registro_t* obtener_registro_mas_reciente(t_list*);
//t_list* 	obtener_registros_key(char*, uint16_t);
//registro_t* leer_binario(char*, uint16_t); //va en fileSystem

// --------FUNCIONES DE TABLAS--------
int  existe_tabla(char*);
void agregar_tabla(char*);
void eliminar_tabla_de_mem(char*);

void agregar_a_contador_dumpeo(char*);
int siguiente_nro_dump(char*);
void agregar_a_contador_dumpeo(char*);
void dumpear_tabla(char*, void*);
void dumpear(t_dictionary*);
void dumpeo();


void pasar_a_archivo(char*, t_list*, char*);
void escribir(FILE*, t_list*);
//void bajar_tabla(mem_tabla_t* t)

#endif /* MEMTABLE_H_ */
