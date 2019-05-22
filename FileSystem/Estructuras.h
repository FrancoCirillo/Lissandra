/*
 * Estructuras.h
 *
 *  Created on: 16 may. 2019
 *      Author: utnso
 */

#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

#include "fileSystem.h"

							/*ARCHIVOS*/

void loggear_FS(char*);
void inicializar_configuracion(void);
void inicializar_directorios(void);
void actualizar_tiempo_retardo_config(mseg_t);   //ver si le llega mseg_t o un char*
void actualizar_tiempo_dump_config(mseg_t);		//ver si le llega mseg_t o un char*
char* obtener_por_clave(char* , char* );
int obtener_tiempo_dump_config();


void leer_metadata_FS();		//es un único archivo

int  crear_directorio(char *,char *);
FILE* crear_archivo(char*, char*, char*);
int  crear_particiones(char*, int);
void  crear_bloque(char*);
void  crear_bloques();
int  crear_metadata(instr_t*);
//leer_metadata_tabla(???) TODO definir esta funcion

int existe_tabla(char * );

int archivo_inicializar(FILE *);
void metadata_inicializar(FILE*, instr_t*);

//semaforos

//sem_t mutex_config;
//sem_t mutex_tiempo_dump;
//sem_t mutex_memtable;
//sem_t mutex_archivo;
//sem_t mutex_bitmap;
//sem_t mutex_


#endif /* ESTRUCTURAS_H_ */
