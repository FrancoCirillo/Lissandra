#ifndef UTILS_SERVIDOR
#define UTILS_SERVIDOR

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<string.h>
#include"definicionesConexiones.h"
#include <arpa/inet.h>
#include "serializaciones.h"

t_log* logger;
instr_t* request;
int server_fd;


int escuchar_en(int server_fd);
int iniciar_servidor(char* ip_proceso, char* puerto_a_abrir);
int esperar_cliente(int socket_servidor, struct in_addr* ip_cliente);

#endif /* UTILS_SERVIDOR */
