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
#include <pthread.h>
#include <semaphore.h>
#include <math.h>
#include <stdio.h>

							/*Globales*/
int   bloques_disponibles;
int   un_num_bloque;


							/*ARCHIVOS*/

void  loggear_FS(char*);
void  loggear_FS_error(char*, instr_t*);
void  inicializar_configuracion(void);
void  inicializar_directorios(void);
void  actualizar_tiempo_retardo_config(mseg_t);   //ver si le llega mseg_t o un char*
void  actualizar_tiempo_dump_config(mseg_t);		//ver si le llega mseg_t o un char*
char* obtener_por_clave(char* , char*);
int   obtener_tiempo_dump_config();


//METADATA TABLAS
void	  metadata_inicializar(FILE*, instr_t*);
void      crear_metadata(instr_t*);
t_config* obtener_metadata(char*);
int 	  obtener_part_metadata(char*);
char* 	  obtener_consistencia_metadata(char*);
int 	  obtener_tiempo_compactacion_metadata(char*);

//BITARRAY
int 		bloques_en_bytes();
void 		inicializar_bitmap();
t_bitarray* levantar_bitmap();
void 		actualizar_bitmap(t_bitarray*);
int 		cant_bloques_disp();
void 		eliminar_bitarray(t_bitarray*);
int 		bloque_esta_ocupado(int nro_bloque);
int 		siguiente_bloque_disponible(); //NO VALIDA SI HAY BLOQUES DISPONIBLES
void 		ocupar_bloque(int);
void 		liberar_bloque(int);


void leer_metadata_FS();		//es un único archivo


void  crear_directorio(char *,char *);
void  eliminar_directorio(char*); //TODO hacer
FILE* crear_archivo(char*, char*, char*);


//TEMPORALES Y BINARIOS
char* pasar_a_string(char**); //TODO esta funcion pasa de char** a char*
char* agregar_bloque_bloques(char*, int); //TODO le agrega el int como un char*
int   agregar_bloque_archivo(char*, int);
FILE* crear_tmp(char*, char*);
void  crear_particiones(instr_t*);
//int   tam_registro(registro_t*);
int   obtener_tam_archivo(char*);
//void  aumentar_tam_archivo(char*, registro_t*);
int   cantidad_bloques_usados(char*);
int   espacio_restante_bloque(char*);


//BLOQUES
void  crear_bloque(char*);
void  crear_bloques();
//char* formatear_registro(registro_t*);
char* obtener_ruta_bloque(int);
//void escribir_registro_bloque(registro_t*, char*, char*); ---> NO ME RECONOCE REGISTRO_T
int   puede_crear_particiones(instr_t* );
int   cant_bloques_disponibles();
void  restar_bloques_disponibles(int);
void  incremetar_bloques_disponibles(int);



int archivo_inicializar(FILE*);


#endif /* ESTRUCTURAS_H_ */
