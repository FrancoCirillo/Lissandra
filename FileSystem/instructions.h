//--------instructions.h--------

#ifndef INSTRUCTIONS_H_
#define INSTRUCTIONS_H_

#include "fileSystem.h"

void 	evaluar_instruccion(instr_t* instr, char* remitente);
void 	execute_create(instr_t*, char* remitente);
t_list* execute_insert(instr_t*, cod_op* codOp);
void 	execute_select(instr_t*, char* remitente);
void 	execute_describe(instr_t*, char* remitente); //TODO chequear
void 	execute_drop(instr_t*, char* remitente);

void 	ejecutar_instruccion_insert(instr_t* instruccion, char* remitente);


#endif /* INSTRUCTIONS_H_ */
