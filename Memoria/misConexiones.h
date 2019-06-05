//--------misConexiones.h--------

#ifndef MISCONEXIONES_H_
#define MISCONEXIONES_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include "../Herramientas/serializaciones.h"
#include "../Herramientas/definicionesConexiones.h"
#include "../Herramientas/utilsServidor.h"
#include "../Herramientas/utilsCliente.h"
#include "../Herramientas/tiempo.h"
#include "admin.h"

int obtener_fd_out(char *proceso);
void imprimir_donde_corresponda(cod_op codigoOperacion, instr_t *instruccion, t_list *listaParam);
void responderHandshake(identificador *idsConexionEntrante);
void enviar_datos_a_FS(char *argv[]);
void pedir_tamanio_value();
void actualizar_tamanio_value(instr_t* instruccion);

#endif /* MISCONEXIONES_H_ */
