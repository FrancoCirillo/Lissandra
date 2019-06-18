//-------utilsServidor.h-------

#ifndef UTILS_SERVIDOR
#define UTILS_SERVIDOR

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <string.h>
#include "definicionesConexiones.h"
#include <arpa/inet.h>
#include "serializaciones.h"
#include "utilsCliente.h"
#include "logging.h"
#include "auxInotify.h"

#define TAMANIO_MAX_INPUT_CONSOLA 200

instr_t *request;
int listener;

void imprimir_quien_se_conecto(struct sockaddr_storage remoteaddr);
int iniciar_servidor(char *ip_proceso, char *puerto_a_abrir);
int vigilar_conexiones_entrantes(void (*ejecutar_requestRecibido)(instr_t *instruccionAEjecutar, char *remitente), int queConsola);
char *ip_cliente(struct sockaddr_storage remoteaddr);

#endif /* UTILS_SERVIDOR */
