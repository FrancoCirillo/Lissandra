/*
 * logging.h
 *
 *  Created on: 3 jun. 2019
 *      Author: utnso
 */

#ifndef LOGGING_H_
#define LOGGING_H_
#include <stdlib.h>
#include <semaphore.h>
#include <commons/log.h>

void loggear_debug(t_log *logger, sem_t* mutex_log, char* valor);
void loggear_info(t_log *logger, sem_t* mutex_log, char* valor);
void loggear_error(t_log *logger, sem_t* mutex_log, char* valor);
void loggear_warning(t_log *logger, sem_t* mutex_log, char* valor);

#endif /* LOGGING_H_ */
