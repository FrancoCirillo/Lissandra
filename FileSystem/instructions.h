//--------instructions.h--------

#ifndef INSTRUCTIONS_H_
#define INSTRUCTIONS_H_

#include "fileSystem.h"
typedef struct instr_remitente {
	instr_t* instruccion;
	char* remitente;
} instr_remitente;

void 	evaluar_instruccion(instr_t* instr, char* remitente);
void 	execute_create(instr_t*, char* remitente);
t_list* execute_insert(instr_t*, cod_op* codOp);
void 	execute_select(instr_t*, char* remitente);
void 	execute_describe(instr_t*, char* remitente); //TODO chequear
void 	execute_drop(instr_t*, char* remitente);
void evaluar_instruccion2(instr_remitente* in) ;
void 	ejecutar_instruccion_insert(instr_t* instruccion, char* remitente);

void liberar_instruccion(instr_t* instruccion);
#endif /* INSTRUCTIONS_H_ */
