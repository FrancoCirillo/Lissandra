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
void inicializar_configuracion(void);
void inicializar_directorios(void);
void actualizar_tiempo_retardo_config(char*);
void actualizar_tiempo_dump_config(char*);
char* obtener_por_clave(char* , char* );
int obtener_tiempo_dump_config();


void leer_metadata_FS();		//es un único archivo

int  crear_directorio(char *,char *);
FILE* crear_archivo(char*, char*, char*);
void  crear_particiones(char*);
void  crear_bloque(char*);
void  crear_bloques();
int  crear_metadata(instr_t*);

int existe_Tabla(char * );

void archivo_inicializar(FILE *);
void metadata_inicializar(FILE*, instr_t*);

//semaforos

//sem_t mutex_config;
//sem_t mutex_tiempo_dump;
//sem_t mutex_memtable;
//sem_t mutex_archivo;
//sem_t mutex_bitmap;
//sem_t mutex_


#endif /* ESTRUCTURAS_H_ */
