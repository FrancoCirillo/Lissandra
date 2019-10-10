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

int obtener_fd_out_sin_semaforo(char* proceso);
int obtener_fd_out(char *proceso);
void imprimir_donde_corresponda(cod_op codigoOperacion, instr_t *instruccion, t_list *listaParam);
void responderHandshake(identificador *idsConexionEntrante);
void enviar_datos_a_FS();
void enviar_datos(char* remitente); //TODO: Unificar con enviar_datos_a_FS();
void pedir_tamanio_value();
void actualizar_tamanio_value(instr_t* instruccion);

/* GOSSIPING */
int fd_out_inicial;
void ejecutar_instruccion_gossip();
void *ejecutar_gossiping();
void devolver_gossip(instr_t* instruccion, char* remitente);
void gossipear_con_conexiones_actuales();
void gossipear_con_procesos_desconectados();
t_list *conexiones_para_gossiping();
void actualizar_tabla_gossiping(instr_t* instruccion);
bool contiene_IP_y_puerto(identificador *ids, char *ipBuscado, char *puertoBuscado);
char* nombre_para_ip_y_puerto(char *ipBuscado, char* puertoBuscado);
instr_t* mis_datos(cod_op codigoOperacion);
void imprimir_config_actual();

#endif /* MISCONEXIONES_H_ */
