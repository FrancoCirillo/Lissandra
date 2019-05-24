//--------componentes.c--------

#include "componentes.h"

void gran_malloc_inicial()
{
	int tamanioMemoria = configuracion.TAMANIO_MEMORIA;
	memoriaPrincipal = malloc(tamanioMemoria);
	if (memoriaPrincipal==NULL) log_error(g_logger, "No se pudo reservar espacio para la memoria principal");
	log_info(g_logger, "Espacio para la memoria principal reservada");
}


void insertar_instruccion(instr_t* instruccion)
{
	int desplazamiento = 0;
	char* tabla = (char *)list_get(instruccion->parametros, 0);
	char*  keyChar = (char *)list_get(instruccion->parametros, 1);
	char* value = (char *)list_get(instruccion->parametros, 2);
	mseg_t timestamp = instruccion->timestamp;
	uint16_t key;
	str_to_uint16(keyChar, &key);

	memcpy(memoriaPrincipal + desplazamiento, &timestamp, sizeof(mseg_t));
	desplazamiento += sizeof(mseg_t);
	memcpy(memoriaPrincipal + desplazamiento, &key, sizeof(uint16_t));
	desplazamiento += sizeof(uint16_t);
	memcpy(memoriaPrincipal + desplazamiento, &value, tamanioValue);
}
