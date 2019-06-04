/*
 * Estructuras.h
 *
 *  Created on: 16 may. 2019
 *      Author: utnso
 */

#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

#include "fileSystem.h"
#include <commons/txt.h>
#include <commons/bitarray.h>
#include <math.h>

							/*ARCHIVOS*/

void loggear_FS(char*);
void loggear_FS_error(char *, instr_t* );
void inicializar_configuracion(void);
void inicializar_directorios(void);
void actualizar_tiempo_retardo_config(mseg_t);   //ver si le llega mseg_t o un char*
void actualizar_tiempo_dump_config(mseg_t);		//ver si le llega mseg_t o un char*
char* obtener_por_clave(char* , char* );
int obtener_tiempo_dump_config();

//METADATA TABLAS
void metadata_inicializar(FILE*, instr_t*);
void  crear_metadata(instr_t*);
t_config* obtener_metadata(char*);
int obtener_part_metadata(char*);
char* obtener_consistencia_metadata(char*);
int obtener_tiempo_compactacion_metadata(char*);

//BITARRAY
t_bitarray* bitarray;
void crear_bitarray();
void eliminar_bitarray();
int bloque_esta_ocupado(int nro_bloque);
int siguiente_bloque_disponible();
void ocupar_bloque(int);
void liberar_bloque(int);


void leer_metadata_FS();		//es un único archivo
int cant_bloques_disp();

void  crear_directorio(char *,char *);
void eliminar_directorio(char*); //TODO hacer
FILE* crear_archivo(char*, char*, char*);
void crear_tmp(char*); //TODO obtener nro de tmp y luego crear_archivo(nro_tmp, tabla, ".tmp")
int  crear_particiones(instr_t*);
void  crear_bloque(char*);
void  crear_bloques();


//BLOQUES
//char* formatear_registro(registro_t*);
char* obtener_ruta_bloque(int);
//void escribir_registro_bloque(registro_t*, char*); ---> NO ME RECONOCE REGISTRO_T


void archivo_inicializar(FILE *);


//semaforos

//sem_t mutex_config;
//sem_t mutex_tiempo_dump;
//sem_t mutex_memtable;
//sem_t mutex_archivo;
//sem_t mutex_bitmap;
//sem_t mutex_


#endif /* ESTRUCTURAS_H_ */
