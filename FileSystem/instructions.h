//--------instructions.h--------

#ifndef INSTRUCTIONS_H_
#define INSTRUCTIONS_H_

#include "fileSystem.h"

void prueba_create(instr_t* instruccion);
int  eliminar_dir(char* tabla);
void prueba_drop(instr_t* instruccion);



/*MANEJO INTRUCCIONES*/
void 	evaluar_instruccion(instr_t* instr, char* remitente);
void 	execute_create(instr_t*, char* remitente);
t_list* execute_insert(instr_t*, cod_op* codOp);
void 	execute_select(instr_t*, char* remitente);
void 	execute_describe(instr_t*, char* remitente); //TODO hacer
void 	execute_drop(instr_t*, char* remitente); //TODO completar

void 	ejecutar_instruccion_insert(instr_t* instruccion, char* remitente);

/*EJEMPLO INTRUCCIONES*/ //para testeo
void 	ejemplo_instr_create();
void 	ejemplo_instr_insert();


#endif /* INSTRUCTIONS_H_ */
