/*
 * tiempo.h
 *
 *  Created on: 14 abr. 2019
 *      Author: utnso
 */


#ifndef HERRAMIENTAS_TIEMPO_H
#define HERRAMIENTAS_TIEMPO_H

#include<stdio.h>
#include<stdlib.h>
#include <time.h>

char* obtener_tiempo_cadena();
time_t obtener_tiempo();
double segundos_entre(time_t tiempo1,time_t tiempo2);

#endif /* HERRAMIENTAS_TIEMPO_H */
