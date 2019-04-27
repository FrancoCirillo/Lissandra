//-------tiempo.h-------

#ifndef HERRAMIENTAS_TIEMPO_H
#define HERRAMIENTAS_TIEMPO_H

#include<stdio.h>
#include<stdlib.h>
#include <time.h>
#include <sys/time.h>

char* obtener_tiempo_cadena();
time_t obtener_tiempo();
double segundos_entre(time_t tiempo1,time_t tiempo2);
int get_timestamp();
int dif_timestamps(int t1, int t2);
int max_timestamp(int t1, int t2);

#endif /* HERRAMIENTAS_TIEMPO_H */
