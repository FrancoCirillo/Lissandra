/*MEMTABLE*/
#ifndef MEMTABLE_H_
#define MEMTABLE_H_

#include "fileSystem.h"

typedef struct registro_t {
//	int key;
	uint16_t key;
	char* value;
	mseg_t timestamp;
} registro_t;

t_dictionary* memtable;

void inicializar_memtable();
void finalizar_memtable();
void borrar_lista_registros(void*);
void borrar_registro(void*);
void limpiar_memtable();
int existe_tabla(char*);
void eliminar_tabla_de_mem(char*);
t_list* crear_lista_registros();
void agregar_tabla(char*);
void agregar_registro(char*, registro_t*);
t_list* obtener_registros(char*, uint16_t);
_Bool es_registro_mas_reciente(void*, void*);
registro_t* obtener_registro_mas_reciente(char*, uint16_t);

registro_t* pasar_a_registro(instr_t*); //TODO hacer!!

void dumpear(t_dictionary*);
void dumpeo();


void pasar_a_archivo(char*, t_list*, char*);
void escribir(FILE*, t_list*);
//void bajar_tabla(mem_tabla_t* t)

int obtener_num_sig_dumpeo(char*);

#endif /* MEMTABLE_H_ */
