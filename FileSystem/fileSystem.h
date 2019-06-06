//-------fileSystem.h-------

#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "../Herramientas/tiempo.h"
#include "../Herramientas/definicionesConexiones.h"
#include "../Herramientas/utilsCliente.h"
#include "../Herramientas/utilsServidor.h"
#include "../Herramientas/serializaciones.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <readline/readline.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <inttypes.h>
#include "Estructuras.h"
#include "compactador.h"
#include "memtable.h"

//ORDEN PARAMETROS

//SELECT [NOMBRE_TABLA] [KEY]
//INSERT [NOMBRE_TABLA] [KEY] “[VALUE]” [Timestamp]
//CREATE [NOMBRE_TABLA] [TIPO_CONSISTENCIA] [NUMERO_PARTICIONES] [COMPACTION_TIME]
//DESCRIBE [NOMBRE_TABLA]
//DROP [NOMBRE_TABLA]

#define RUTA_TABLAS "mnj/Lissandra_FS/Tablas/"
#define RUTA_BLOQUES "mnj/Lissandra_FS/Bloques/"
#define RUTA_METADATA "mnj/Lissandra_FS/Metadata/"
#define RUTA_BITMAP "mnj/Lissandra_FS/Metadata/Bitmap.bin"
#define RUTA_PUNTO_MONTAJE "mnj/Lissandra_FS/"


/*SEMAFOROS*/
sem_t mutex_tiempo_dump_config;
sem_t mutex_tiempo_retardo_config;
sem_t mutex_memtable;
sem_t mutex_log;

/*STRUCTS*/

typedef unsigned long long mseg_t;

typedef struct archivo_t {
	int size;
	int blocks[];
} archivo_t;

typedef struct metadata_t {
	char* consistency;
	char* partitions;
	mseg_t compactation_time;
} metadata_t;

typedef struct config_FS_t {
	char* puerto_escucha;
	char* punto_montaje;
	int tamanio_value;
	mseg_t retardo;
	mseg_t tiempo_dump;
} config_FS_t;

typedef struct metadata_FS_t {
	int block_size;
	int blocks;
	char* magic_number;
} metadata_FS_t;

config_FS_t config_FS;
metadata_FS_t Metadata_FS;

t_log* g_logger;
t_config* g_config;
t_config* meta_config;

void inicializar_FS();
void finalizar_FS();
void iniciar_semaforos();

/*FUNCIONES AUXILIARES*/
char* obtener_parametro(instr_t*, int);
int obtener_particion_key(char*, int);
_Bool es_registro_mas_reciente(void*, void*);
char* obtener_nombre_tabla(instr_t*);


/*FUNCIONES DE BLOQUES*/ //TODO hacer
//--------------importante:NO ME RECONOCE REGISTRO_T

//registro_t* leer_binario(char*, uint16_t);
t_list* leer_archivos_temporales(char*, uint16_t);
//void escribir_bloque(registro_t*);
void eliminar_archivos(char*);

//registro_t* obtener_registro_mas_reciente(t_list*);
//t_list* obtener_registros_key(char*, uint16_t);

/*MANEJO INTRUCCIONES*/
void evaluar_instruccion(instr_t*);
int execute_create(instr_t*);
int execute_insert(instr_t*);
int execute_select(instr_t*);
int execute_describe(instr_t*); //TODO hacer
int execute_drop(instr_t*);

/*EJEMPLO INTRUCCIONES*/ //para testeo
void ejemplo_instr_create();
void ejemplo_instr_insert();
void ejemplo_instr_select();
void ejemplo_instr_describe();
void ejemplo_instr_drop();

/*MANEJO DE CADENAS*/
char* concat(char*, char*);
char* concat3(char*, char*, char*);

/*Comunicación*/
//eliminar
//void response(remitente_t*);
void contestar(instr_t *);

#endif /* FILE_SYSTEM_H */
