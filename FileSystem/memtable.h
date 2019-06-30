//--------memtable.h--------

#ifndef MEMTABLE_H_
#define MEMTABLE_H_

#include "fileSystem.h"
#include "registros.h"

t_dictionary* memtable;
t_dictionary* tablas_nro_dump;
sem_t mutex_tablas_nro_dump;
sem_t mutex_diccionario_conexiones;

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
registro_t* obtener_registro(char*);
registro_t* pasar_a_registro(instr_t*);
t_list* 	obtener_registros_mem(char*, uint16_t);

// --------FUNCIONES DE TABLAS--------
int   existe_tabla(char*);
void  agregar_tabla(char*);
void  eliminar_tabla_de_mem(char*);
char* obtener_ruta_tabla(char*);

void inicializar_tablas_nro_dump();
void eliminar_nro_dump_de_tabla(char*);
void agregar_a_contador_dumpeo(char*);
int  siguiente_nro_dump(char*);
void dumpear_tabla(char*, void*);
void dumpear(t_dictionary*);
void dumpeo();



#endif /* MEMTABLE_H_ */
