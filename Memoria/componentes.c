//--------componentes.c--------

#include "componentes.h"

void *gran_malloc_inicial()
{
	int tamanioMemoria = configuracion.TAMANIO_MEMORIA;
	void* memoria = malloc(tamanioMemoria);
	if (memoria==NULL) log_error(g_logger, "No se pudo reservar espacio para la memoria principal");
	log_info(g_logger, "Espacio para la memoria principal reservada");
	return memoria;
}



