//--------admin.h--------

#ifndef ADMIN_H_
#define ADMIN_H_

#include <commons/log.h>
#include <commons/config.h>
#include "../Herramientas/tiempo.h"
#include "../Herramientas/definicionesConexiones.h"
#include <inttypes.h>
#include <stdbool.h>
#include <semaphore.h>

typedef struct config
{
	char *PUERTO;
	char *IP_FS;
	char *PUERTO_FS;
	t_list *IP_SEEDS;
	t_list *PUERTO_SEEDS;
	int RETARDO_MEMORIA;
	int RETARDO_FS;
	int TAMANIO_MEMORIA;
	int RETARDO_JOURNAL;
	int RETARDO_GOSSIPING;
	int MEMORY_NUMBER;
	char *RUTA_LOG;
	t_log_level LOG_LEVEL;
} config;

typedef struct registro
{
	mseg_t timestamp;
	uint16_t key;
	char* value;
} registro;

extern sem_t mutex_log;
extern sem_t mutex_journal;
extern sem_t mutex_diccionario_conexiones;
extern t_log *g_logger;
extern t_config *g_config;
extern config configuracion;
extern int tamanioValue;
extern char* puntoMontaje;
extern int tamanioRegistro;
extern bool* sectorOcupado;
extern int cantidadDeSectores;
extern t_dictionary *conexionesActuales;
extern char* miIPMemoria;
extern char* nombreDeMemoria;

extern t_dictionary *tablaDeSegmentos;

typedef struct filatabPags
{
	int numeroDePagina;
	void *ptrPagina;
	bool flagModificado;
}filaTabPags;


extern config configuracion;

#endif /* ADMIN_H_ */
