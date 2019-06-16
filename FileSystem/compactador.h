//--------compactador.h--------

#ifndef COMPACTADOR_H_
#define COMPACTADOR_H_

#include "fileSystem.h"
#include "memtable.h"
#include <commons/txt.h>
#include <commons/bitarray.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>
#include <stdio.h>
#include "registros.h"

void compactar();
void compactador(char* tabla);
t_list* listar_archivos(char* tabla);

void actualizar_registros_de_la_particion(char* , t_list* );
void finalizar_compactacion(char* , t_list* );
t_list* comparar_registro_a_registro(t_list* , t_list* );
void filtrar_keys_repetidas(char* , t_list* );


//ELiminar
void compactar2();
void compactar5();


void pasar_a_tmpc(char* tabla);

#endif /* COMPACTADOR_H_ */
