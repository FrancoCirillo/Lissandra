//-------tiempo.h-------

#ifndef HERRAMIENTAS_TIEMPO_H
#define HERRAMIENTAS_TIEMPO_H

#include <stdio.h>
#include <stdlib.h>
//#include <time.h>
#include <sys/time.h>

typedef unsigned long long mseg_t;
// esto tenia long int.

mseg_t obtener_ts();
mseg_t dif_timestamps(mseg_t tiempo1, mseg_t tiempo2);
mseg_t max_timestamp(mseg_t tiempo1, mseg_t tiempo2);

mseg_t get_ts();
char* mseg_to_string(mseg_t );
mseg_t string_to_mseg(char* );


#endif /* HERRAMIENTAS_TIEMPO_H */
