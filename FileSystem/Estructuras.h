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
void loggear_FS_error(char *, instr_t* );
void inicializar_configuracion(void);
void inicializar_directorios(void);
void actualizar_tiempo_retardo_config(mseg_t);   //ver si le llega mseg_t o un char*
void actualizar_tiempo_dump_config(mseg_t);		//ver si le llega mseg_t o un char*
char* obtener_por_clave(char* , char* );
int obtener_tiempo_dump_config();


void leer_metadata_FS();		//es un único archivo
int cant_bloques_disp();

void  crear_directorio(char *,char *);
void eliminar_directorio(char*); //TODO hacer
FILE* crear_archivo(char*, char*, char*);
void crear_tmp(char*); //TODO obtener nro de tmp y luego crear_archivo(nro_tmp, tabla, ".tmp")
int  crear_particiones(instr_t*);
void  crear_bloque(char*);
void  crear_bloques();
void  crear_metadata(instr_t*);


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
