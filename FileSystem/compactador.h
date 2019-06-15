//--------compactador.h--------

#ifndef COMPACTADOR_H_
#define COMPACTADOR_H_

#include "fileSystem.h"

void compactar();
void compactador();


void actualizar_registros_de_la_particion(char* , t_list* );
void finalizar_compactacion(char* , t_list* );
t_list* comparar_registro_a_registro(t_list* , t_list* );
void filtrar_keys_repetidas(char* , t_list* );


//ELiminar
void compactar2();
void compactar5();


void pasar_a_tmpc(char* tabla);

#endif /* COMPACTADOR_H_ */
