#ifndef ADMIN_H_
#define ADMIN_H_

#include <commons/log.h>
#include <commons/config.h>

extern t_log *g_logger;
extern t_config *g_config;
char nombreDeMemoria[12];

typedef struct config
{
	char *PUERTO;
	char *IP_FS;
	char *PUERTO_FS;
	char *IP_SEEDS;
	char *PUERTO_SEEDS;
	int RETARDO_MEMORIA;
	int RETARDO_FS;
	int TAMANIO_MEMORIA;
	int RETARDO_JOURNAL;
	int RETARDO_GOSSIPING;
	int MEMORY_NUMBER;
	char *RUTA_LOG;
} config;

extern config configuracion;

#endif /* ADMIN_H_ */
