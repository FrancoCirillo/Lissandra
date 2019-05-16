/*
 * Estructuras.h
 *
 *  Created on: 16 may. 2019
 *      Author: utnso
 */

#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

#include "fileSystem.h"
#include <pthread.h>
							/*ARCHIVOS*/

void loggear_FS(char*);
void inicializarConfig(void);
char* leer_config(char* );

char* leerMetadata_FS();		//es un único archivo

void  crear_directorio(char *);
FILE* crear_archivo(char*, char*, char*);
void  crear_particiones(char*);
void  crear_bloque(char*);
void  crear_bloques();
void  crear_metadata(char*, char*, char*, char*);

int existe_Tabla(char * );

void archivo_inicializar(FILE *);
void metadata_inicializar(FILE*, char*, char*, char*);

//semaforos

sem_t mutex_config;
sem_t mutex_tiempo_dump;
sem_t mutex_memtable;
sem_t mutex_archivo;
sem_t mutex_bitmap;
sem_t mutex_


#endif /* ESTRUCTURAS_H_ */
