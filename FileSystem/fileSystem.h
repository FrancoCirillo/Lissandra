#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "/home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua/Herramientas/utilsCliente.h"
#include "/home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua/Herramientas/utilsServidor.h"
#include "/home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua/Herramientas/tiempo.h"
#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include<commons/log.h>
#include<commons/config.h>
#include<commons/string.h>
#include<readline/readline.h>
#include <pthread.h>

void loggear(char*);
void inicializarConfig(void);


t_log* g_logger;
t_config* g_config;


enum codigo{
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
	};


typedef struct archivo{
	int size;
	int blocks[];    // Ver esto, no le puedo asignar un tamaño fijo al array..
}archivo;

typedef struct metadata{
	char * consistency;
	char * partitions;
	time_t compactation_time;
}metadata;

typedef struct metadata_FS{

}metadata_FS;

typedef struct archivo_config{
	int PUERTO_ESCUCHA;
	char* PUNTO_MONTAJE;
	int TAMANIO_VALUE;
	time_t RETARDO;
	time_t TIEMPO_DUMP;
}archivo_config;


archivo_config configuracion;


typedef struct memoria{
	int ip;
	int puerto;
}memoria;

typedef struct instruccion_memoria{
	char* instruccion;
	memoria memo;
}instruccion_memoria;


typedef struct registro{
	//u_int16 key;
	char* value;
	time_t timestamp;
}registro;

typedef struct tabla{
	metadata* metadata_tabla;
	int block_size;
	int blocks;
	int magic_number;
}tabla;




#endif /* FILE_SYSTEM_H */
