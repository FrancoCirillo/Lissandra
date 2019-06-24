//-------logging.c-------

#ifndef LOGGING_H_
#define LOGGING_H_
#include <stdlib.h>
#include <semaphore.h>
#include <commons/log.h>
#include "definicionesConexiones.h"

void loggear_trace(char* valor);
void loggear_debug(char* valor);
void loggear_info(char* valor);
void loggear_error(char* valor);
void loggear_warning(char* valor);

#endif /* LOGGING_H_ */
