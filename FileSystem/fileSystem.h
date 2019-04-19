#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "/home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua/Herramientas/tiempo.h"
#include "/home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua/Herramientas/definicionesConexiones.h"
#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include<commons/log.h>
#include<commons/config.h>
#include<commons/string.h>
#include<readline/readline.h>
#include<pthread.h>


void loggear(char*);
void inicializarConfig(void);
void evaluar_instruccion(remitente_instr_t*);


#define RUTA_TABLAS "/home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua/FileSystem/mnj/LISANDRA_FS/TABLAS/"
#define RUTA_BLOQUES "/home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua/FileSystem/mnj/LISANDRA_FS/BLOQUES/"
#define RUTA_PUNTO_MONTAJE "/home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua/FileSystem/mnj/LISANDRA_FS/"
#define RUTA_PUNTO_MONTAJE_2 "/home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua/FileSystem/mnj"			//TODO Ver como parametrizar esto!
#define RUTA_PUNTO_MONTAJE_3 "/home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua/FileSystem/archivo.log"


t_log* g_logger;
t_config* g_config;


typedef enum{
	CODIGO_SELECT = 3,
	ERROR_SELECT = -3,
	CODIGO_INSERT = 4,
	ERROR_INSERT = -4,
	CODIGO_CREATE = 5,
	ERROR_CREATE = -5,
	CODIGO_DESCRIBE = 6,
	ERROR_DESCRIBE = -6,
	CODIGO_DROP = 7,
	ERROR_DROP = -7
	}cod_instr;



typedef struct archivo_t{
	int size;
	int blocks[];
}archivo_t;

typedef struct metadata_t{
	char * consistency;
	char * partitions;
	time_t compactation_time;
}metadata_t;

typedef struct archivo_config_FS_t{
	int PUERTO_ESCUCHA;
	char* PUNTO_MONTAJE;
	int TAMANIO_VALUE;
	time_t RETARDO;
	time_t TIEMPO_DUMP;
}archivo_config_FS_t;
//Recordar que los ultimos 2 son modificables. Inotify()    No creo que sea necesario el struct.
//Grabar valores correctos. y poder leerlos.

archivo_config_FS_t configuracion;


typedef struct registro_t{
	//u_int16 key;			//TODO ver este tipo.
	char* value;
	time_t timestamp;
}registro;


char* leerMetadata_FS();//hacer, es un único archivo

typedef struct metadata_FS_t{
	int block_size;
	int blocks;
	char* magic_number;
}tabla;






#endif /* FILE_SYSTEM_H */
