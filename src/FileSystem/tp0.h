/*
 * Cliente.h
 *
 *  Created on: 28 feb. 2019
 *      Author: utnso
 */

#ifndef TP0_H_
#define TP0_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>

#include "utilsCliente.h"

int enviarInstruccion(char* instruccion, cod_proceso proceso);
t_config* leer_config();
#endif /* TP0_H_ */
