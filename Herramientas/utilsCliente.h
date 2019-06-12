//-------utilsCliente.h-------

#ifndef UTILS_CLIENTE_
#define UTILS_CLIENTE_

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include "definicionesConexiones.h"
#include "logging.h"
#include "serializaciones.h"
#include <netinet/in.h>
#include <arpa/inet.h>

int crear_conexion(char *ip, char *puerto, char *miIP, int flagReintentar, t_log* logger, sem_t* mutex_log);

#endif /* UTILS_CLIENTE_ */
