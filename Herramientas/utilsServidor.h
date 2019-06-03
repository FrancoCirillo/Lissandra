//-------utilsServidor.h-------

#ifndef UTILS_SERVIDOR
#define UTILS_SERVIDOR

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <string.h>
#include "definicionesConexiones.h"
#include <arpa/inet.h>
#include "serializaciones.h"
#include "utilsCliente.h"

#define TAMANIO_MAX_INPUT_CONSOLA 200
instr_t *request;
int server_fd;

void imprimir_quien_se_conecto(struct sockaddr_storage remoteaddr);
int iniciar_servidor(char *ip_proceso, char *puerto_a_abrir, t_log *logger);
int vigilar_conexiones_entrantes(int listener, void (*ejecutar_requestRecibido)(instr_t *instruccionAEjecutar, char *remitente), t_dictionary *conexionesConocidas, int queConsola, t_log *logger);
char *ip_cliente(struct sockaddr_storage remoteaddr);

#endif /* UTILS_SERVIDOR */
