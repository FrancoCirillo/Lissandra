//--------compactador.h--------

#ifndef COMPACTADOR_H_
#define COMPACTADOR_H_

#include "fileSystem.h"
#include "memtable.h"
#include "Estructuras.h"
#include <commons/txt.h>
#include <commons/bitarray.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "registros.h"

void    compactactar_todos_los_tmpc();
void 	compactacion_final(char* tabla, void* registros);
void 	compactador(char* tabla);
t_list* listar_archivos(char* tabla);
int 	hay_tmp(char* tabla);
void 	agregar_por_ts( t_dictionary* dic, registro_t* reg_nuevo);
void 	agregar_registros_en_particion(t_list* particiones, char* ruta_archivo);
t_list* levantar_registros(char* ruta_archivo);
void 	liberar_todos_los_bloques(char* tabla);
void 	finalizar_compactacion(t_dictionary*, char* tabla, int num);
int  	ultimo_bloque(t_config* archivo);
void    eliminar_archivo(char* ruta_archivo);
void    borrar_tmpcs(char* tabla);

//ELiminar
void 	compactar2();
void 	compactar5();


int pasar_a_tmpc(char* tabla);

#endif /* COMPACTADOR_H_ */
