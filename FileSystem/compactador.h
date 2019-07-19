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

_Bool compactation_locker;

void 	crear_hilo_compactador(char* tabla);
void    compactar_todas_las_tablas();
void* 	compactador(void* tabla);
t_list* listar_archivos(char* tabla);
int 	hay_tmp(char* tabla);
void 	agregar_bloque_por_ts(char* bloqueCompleto, t_list* particiones);
void 	agregar_por_ts(t_dictionary* dic, registro_t* reg_nuevo);
void 	agregar_registros_de_particion(t_list* particiones, char* ruta_archivo);
t_list* levantar_registros(char* ruta_archivo);
void 	liberar_todos_los_bloques(char* tabla);
void 	finalizar_compactacion(t_dictionary*, char* tabla, int num);
int  	ultimo_bloque(t_config* archivo);
void    eliminar_archivo(char* ruta_archivo);
int 	pasar_a_tmpc(char* tabla);
void    borrar_tmpcs(char* tabla);
void 	vaciar_listas_registros(t_list* particiones);
void 	liberar_listas_registros(t_list* particiones);
void 	liberar_registro(registro_t* registro);

#endif /* COMPACTADOR_H_ */
