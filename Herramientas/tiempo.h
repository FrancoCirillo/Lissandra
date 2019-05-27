//-------tiempo.h-------

#ifndef HERRAMIENTAS_TIEMPO_H
#define HERRAMIENTAS_TIEMPO_H

#include <stdio.h>
#include <stdlib.h>
//#include <time.h>
#include <sys/time.h>
#include <inttypes.h>

typedef uint64_t mseg_t;

mseg_t obtener_ts();
mseg_t dif_timestamps(mseg_t tiempo1, mseg_t tiempo2);
mseg_t max_timestamp(mseg_t tiempo1, mseg_t tiempo2);

char* mseg_to_string(mseg_t );
mseg_t string_to_mseg(char* );


#endif /* HERRAMIENTAS_TIEMPO_H */
