#include "logging.h"

void loggear_error(t_log *logger, sem_t* mutex_log, char* valor){
	sem_wait(mutex_log);
	log_error(logger, valor);
	sem_post(mutex_log);
	free(valor);
}

void loggear_info(t_log *logger, sem_t* mutex_log, char* valor){
	sem_wait(mutex_log);
	log_info(logger, valor);
	sem_post(mutex_log);
	free(valor);
}

void loggear_debug(t_log *logger, sem_t* mutex_log, char* valor){
	sem_wait(mutex_log);
	log_debug(logger, valor);
	sem_post(mutex_log);
	free(valor);
}

void loggear_warning(t_log *logger, sem_t* mutex_log, char* valor){
	sem_wait(mutex_log);
	log_warning(logger, valor);
	sem_post(mutex_log);
	free(valor);
}
