/*MEMTABLE*/


#ifndef MEMTABLE_H_
#define MEMTABLE_H_

#include "fileSystem.h"

typedef struct mem_tabla_t{
	t_list* registros;
	char* tabla;
}mem_tabla_t;

typedef struct registro_t{
	int key;
	//uint16_t key;			//TODO ver este tipo. int es provisorio.
	char* value;
	mseg_t timestamp;
}registro_t;

t_list* memtable;

void inicializar_memtable();
void finalizar_memtable();

void dumpear(t_list* );
void dumpeo();

void mem_limpiar();
void mem_agregar_tabla(char*);
void mem_agregar_reg(instr_t*);

void pasar_a_archivo(char*,t_list*, char*);
void bajar_tabla(mem_tabla_t* );
void escribir(FILE*,t_list*);

int obtener_num_sig_dumpeo(char* );


#endif /* MEMTABLE_H_ */
