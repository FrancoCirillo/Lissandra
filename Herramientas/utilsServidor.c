/*
 * conexiones.c
 *
 *  Created on: 3 mar. 2019
 *      Author: utnso
 */

#include"utilsServidor.h"

int iniciar_servidor(char* ip_proceso, char* puerto_a_abrir) {
	logger = log_create("log.log", "Servidor", 1, LOG_LEVEL_DEBUG);
	int socket_servidor;

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip_proceso, puerto_a_abrir, &hints, &servinfo);

	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((socket_servidor = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1)
			continue;

		if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
			close(socket_servidor);
			continue;
		}
		break;
	}

	listen(socket_servidor, SOMAXCONN);

	freeaddrinfo(servinfo);

	log_trace(logger, "Listo para escuchar a mi cliente");

	return socket_servidor;
}

int escuchar_en(int server_fd) {
	struct in_addr *ip_cliente;
	ip_cliente = malloc(sizeof(struct in_addr));//TODO: HACER free()!!!
	int cliente_fd = esperar_cliente(server_fd, ip_cliente); //accept(), en ip_cliente se aloja la respuesta
	while (1) {
		int cod_op = recibir_operacion(cliente_fd);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_fd, ip_cliente); //guardado en request
			break;
		case PAQUETE:
//			lista = recibir_paquete(cliente_fd);
//			printf("Me llegaron los siguientes valores:\n");
//			list_iterate(lista, (void*) iterator);
			log_error(logger, "Recepcion de paquetes todavia no implementada"); //TODO
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return EXIT_FAILURE;
		default:
			log_warning(logger,
					"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	return EXIT_SUCCESS;
}

int esperar_cliente(int socket_servidor, struct in_addr * ip_cliente) {
	struct sockaddr_in dir_cliente;

	int tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	struct sockaddr_in* pV4Addr = (struct sockaddr_in*) &dir_cliente;
	*ip_cliente = pV4Addr->sin_addr; //IP del cliente (en Network Byte Order)

	char IP_string[INET_ADDRSTRLEN];
	inet_ntop( AF_INET, ip_cliente, IP_string, INET_ADDRSTRLEN); //Para poder imprimirlo

	log_info(logger, "Se conecto un cliente!");
	printf("IP cliente: %s\n", IP_string);

	return socket_cliente;
}

int recibir_operacion(int socket_cliente) {
	int cod_op;
	if (recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) != 0)
		return cod_op;
	else {
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente) {
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente, struct in_addr * ip_cliente) {
	request = malloc(sizeof(instr_t));
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	deserializar(buffer, request);//SEGUIR ACA, PASARLE IP_CLIENTE
	free(buffer);
}

int deserializar(char* mensaje, instr_t* outPut) {
	//El mensaje debe poseer el \0 final
	//formato: codigoInstruccion;param1;param2;param3;param4;
	char* aux = mensaje;
	char* extracto;
	int contador = 0;
	int posicionInicial = 0;
	int posicionFinal;
	char caracterSeparador = ';';
	puts("\n ------------------- \n Iniciando parseo");
	while (*aux && contador < 5) {
		if (*aux == caracterSeparador) {

			//Obtengo donde se detecto el ;
			posicionFinal = strlen(mensaje) - strlen(aux);

			extracto = substr(mensaje, posicionInicial, posicionFinal);
			//La proxima corto desde la posicion final del anterior +1 para cortar el ;
			posicionInicial = posicionFinal + 1;

			//printf("\n ; encontrado, recortando %s",extracto);

			if (contador == 0) {
				outPut->codigoInstruccion = atoi(extracto);
				contador++;
				//puts("Codigo encontrado");
			} else if (contador == 1) {
				outPut->param1 = extracto;
				contador++;
				//puts("Param 1 encontrado");
			} else if (contador == 2) {
				outPut->param2 = extracto;
				contador++;
				//puts("Param 2 encontrado");
			} else if (contador == 3) {
				outPut->param3 = extracto;
				contador++;
				//puts("Param 3 encontrado");
			} else if (contador == 4) {
				outPut->param4 = extracto;
				contador++;
				//puts("Param 4 encontrado");
			}
		}
		//printf("\n %c obtenido, restan %d    \n",*aux,strlen(aux));
		aux++;
	}
	printf(
			"\n Se pudo parsear correctamente!!,\n codigo de instruccion %d,\n parametro1 %s,\n parametro2 %s,\n parametro3 %s,\n parametro4 %s  ",
			outPut->codigoInstruccion, outPut->param1, outPut->param2,
			outPut->param3, outPut->param4);
	puts("\n Parseo finalizado \n ---------------");
	return 1;
}

char* substr(const char *src, int m, int n) {
	// get length of the destination string
	int len = n - m;

	// allocate (len + 1) chars for destination (+1 for extra null character)
	char *dest = (char*) malloc(sizeof(char) * (len + 1));

	// extracts characters between m'th and n'th index from source string
	// and copy them into the destination string
	for (int i = m; i < n && (*src != '\0'); i++) {
		*dest = *(src + i);
		dest++;
	}

	// null-terminate the destination string
	*dest = '\0';

	// return the destination string
	return dest - len;
}

