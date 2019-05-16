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
#include <string.h>
#include "definicionesConexiones.h"
#include <arpa/inet.h>
#include "serializaciones.h"

t_log* logger;
instr_t* request;
int server_fd;

void imprimir_quien_se_conecto(struct sockaddr_storage remoteaddr);
int iniciar_servidor(char* ip_proceso, char* puerto_a_abrir);
int vigilar_conexiones_entrantes(int listener, void (*ejecutar_requestRecibido)(instr_t* instruccionAEjecutar, int conexionReceptor), int conexionReceptor, int queConsola);

#endif /* UTILS_SERVIDOR */