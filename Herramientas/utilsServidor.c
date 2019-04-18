/*
 * conexiones.c
 *
 *  Created on: 3 mar. 2019
 *      Author: utnso
 */

#include"utilsServidor.h"

int iniciar_servidor(char* ip_proceso, char* puerto_a_abrir) {
	logger = log_create("log.log", "Servidor", 1, LOG_LEVEL_DEBUG); //TODO: recibirlo por parametro (Cada proceso tiene su logger)
	int socket_servidor;

	struct addrinfo hints, *servinfo; //Estructuras solo para llenar servinfo

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; //No importa si es ipv4 o ipv6
	hints.ai_socktype = SOCK_STREAM; //Usamos TCP
	hints.ai_flags = AI_PASSIVE;//Rellena la IP por nosotros

	//Aca se llenan los datos de servinfo:
	getaddrinfo(ip_proceso, puerto_a_abrir, &hints, &servinfo); //TODO: chequear los errores de getaddrinfo

	if ((socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) <0){
		log_error(logger, "Error al crear el socket servidor");
		//TODO: Cerrar socket servidor?
		return -1;
	}

	if (bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen) < 0) {
		log_error(logger, "Error al vincular el socket servidor con el puerto");
		close(socket_servidor);
		return -1;
		}

	if ((listen(socket_servidor, SOMAXCONN))<0){ //TODO: queremos SO MAX CONNECTIONS?
		log_error(logger, "Error al dejar todo listo para escuchar en socket servidor");
		//TODO: Cerrar socket servidor?
		return -1;
	}
	freeaddrinfo(servinfo);

	log_trace(logger, "Listo para escuchar a mi cliente");

	return socket_servidor;
}

int escuchar_en(int server_fd) {
	instr_t * instruccion_recibida;
	struct in_addr *ip_cliente;
	ip_cliente = malloc(sizeof(struct in_addr));//TODO: HACER free()!!!
	int cliente_fd = esperar_cliente(server_fd, ip_cliente); //accept(), en ip_cliente se aloja la respuesta
	while (recibir_paquete(cliente_fd, &instruccion_recibida)==0);
	return 0;
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
