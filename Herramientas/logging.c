#include "logging.h"

void loggear_trace(char* valor){
	sem_wait(&mutex_log);
	log_trace(g_logger, valor);
	sem_post(&mutex_log);
	free(valor);
}

void loggear_debug(char* valor){
	sem_wait(&mutex_log);
	log_debug(g_logger, valor);
	sem_post(&mutex_log);
	free(valor);
}

void loggear_info(char* valor){
	sem_wait(&mutex_log);
	log_info(g_logger, valor);
	sem_post(&mutex_log);
	free(valor);
}


void loggear_warning(char* valor){
	sem_wait(&mutex_log);
	log_warning(g_logger, valor);
	sem_post(&mutex_log);
	free(valor);
}

void loggear_error(char* valor){
	sem_wait(&mutex_log);
	log_error(g_logger, valor);
	sem_post(&mutex_log);
	free(valor);
}
