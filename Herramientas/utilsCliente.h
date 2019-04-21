#ifndef UTILS_CLIENTE_
#define UTILS_CLIENTE_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include"definicionesConexiones.h"
#include"serializaciones.h"
#include <netinet/in.h>
#include <arpa/inet.h>
int conectar_con_proceso(cod_proceso procesoServidor, cod_proceso procesoCliente);
int crear_conexion(char *ip, char* puerto, char* miIP);

#endif /* UTILS_CLIENTE_ */
