//-------fileSystem.h-------

#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "/home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua/Herramientas/tiempo.h"
#include "/home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua/Herramientas/definicionesConexiones.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <readline/readline.h>
#include <pthread.h>
#include <sys/stat.h>
#include <dirent.h>

//ORDEN PARAMETROS

//SELECT [NOMBRE_TABLA] [KEY]
//INSERT [NOMBRE_TABLA] [KEY] “[VALUE]” [Timestamp]     <- opcional el timestamp?
//CREATE [NOMBRE_TABLA] [TIPO_CONSISTENCIA] [NUMERO_PARTICIONES] [COMPACTION_TIME]
//DESCRIBE [NOMBRE_TABLA]    <- opcional el param.
//DROP [NOMBRE_TABLA]


#define RUTA_TABLAS "mnj/Lissandra_FS/Tablas/"
#define RUTA_BLOQUES "mnj/Lissandra_FS/Bloques/"
#define RUTA_PUNTO_MONTAJE "mnj/Lissandra_FS/"

t_log* g_logger;
t_config* g_config;

typedef long int mseg_t;

/*STRUCTS*/
typedef struct archivo_t{
	int size;
	int blocks[];
}archivo_t;

typedef struct metadata_t{
	char * consistency;
	char * partitions;
	mseg_t compactation_time;
}metadata_t;

typedef struct archivo_config_FS_t{
	int PUERTO_ESCUCHA;
	char* PUNTO_MONTAJE;
	int TAMANIO_VALUE;
	mseg_t RETARDO;
	mseg_t TIEMPO_DUMP;
}archivo_config_FS_t;
//Recordar que los ultimos 2 son modificables. Inotify()    No creo que sea necesario el struct.
//Grabar valores correctos. y poder leerlos.

archivo_config_FS_t configuracion;


typedef struct registro_t{
	//u_int16 key;			//TODO ver este tipo.
	char* value;
	mseg_t timestamp;
}registro;

typedef struct metadata_FS_t{
	int block_size;
	int blocks;
	char* magic_number;
}tabla;


							/*ARCHIVOS*/

void loggear_FS(char*);
void inicializarConfig(void);

char* leerMetadata_FS();		//TODO hacer, es un único archivo

void  crear_directorio(char *);
FILE* crear_archivo(char*, char*, char*);
void  crear_particiones(char*);
void  crear_bloque(char*);
void  crear_bloques();
void  crear_metadata(char*, char*, char*, char*);

int existe_Tabla(char * );

void archivo_inicializar(FILE *);
void metadata_inicializar(FILE*, char*, char*, char*);

							/*MANEJO INTRUCCIONES*/

void evaluar_instruccion(remitente_instr_t*);
void execute_create(instr_t*);
void execute_insert(instr_t*);
void execute_select(instr_t*);
void execute_describe(instr_t*);
void execute_drop(instr_t*);
instr_t* i_create(instr_t *);
instr_t* i_insert(instr_t *);
instr_t* i_select(instr_t *);
instr_t* i_describe(instr_t *);
instr_t* i_drop(instr_t *);

							/*MANEJO DE CADENAS*/

char* concat(char*, char*);
char* concat3( char* , char*, char*);

							/*Comunicación*/
void response(remitente_t*);

							/*Compactación*/
void* compactar(instr_t*);
void compactation(char*);


#endif /* FILE_SYSTEM_H */
