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

char* substr(const char *src, int m, int n);
int deserializar(char* mensaje,instr_t* outPut);

t_log* logger;
instr_t* request;
int server_fd;

void* recibir_buffer(int*, int);
int escuchar_en(int server_fd);
int iniciar_servidor(char* ip_proceso, char* puerto_a_abrir);
int esperar_cliente(int socket_servidor, struct in_addr* ip_cliente);
t_list* recibir_paquete(int);
void recibir_mensaje(int socket_cliente, struct in_addr * ip_cliente);
int recibir_operacion(int);

#endif /* UTILS_SERVIDOR */
