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

int conectar_con_proceso(cod_proceso proceso);
int crear_conexion(char *ip, char* puerto);

#endif /* UTILS_CLIENTE_ */
