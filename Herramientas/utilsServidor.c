//-------utilsServidor.c-------

#include"utilsServidor.h"

/*
 * socket() y bind()
 */

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
	int aceptar;
	setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &aceptar, sizeof(int));
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

int vigilar_conexiones_entrantes(int listener, void (*ejecutar_requestRecibido)(instr_t* instruccionAEjecutar, int conexionReceptor), int conexionReceptor){

	//Gracias a la guia de Beej:
	fd_set master;    // lista 'master' de file descriptors
	fd_set read_fds;  // lista temporal de fds para el select()
	int fdmax;        // mayor file descriptor (para recorrerlos todos con el for)

	int newfd;        // nuevos fds a los que se les hizo accept()
	struct sockaddr_storage remoteaddr; // direccion del cliente
	socklen_t addrlen;

	char bufferLeido[100];
	int i;

	FD_ZERO(&master); //los vaciamos
	FD_ZERO(&read_fds);

	FD_SET(listener, &master); // agregamos a los fds que vigila select()
	FD_SET(0, &master); // 0 es el fd de la consola

	// mantener cual es el fd mas grande (lo pide el seelct())
	fdmax = listener; // por ahora es este

	while(1) {
		read_fds = master;
		printf("\n" COLOR_ANSI_MAGENTA ">" COLOR_ANSI_RESET);
		fflush(stdout);
		int resultado = select(fdmax + 1, &read_fds, NULL, NULL, NULL);
		if (resultado == -1) {
			perror("select");
		} else if (resultado > 0) {
			//recorrer las conexiones existentes buscando data para leer, empezando desde el fd 0
			for (i = 0; i <= fdmax; i++) {
				if (FD_ISSET(i, &read_fds)) { // tenemos una nueva conexion entrante
					if (i == listener) {
						// manejo de conexiones nuevas:
						addrlen = sizeof remoteaddr;
						newfd = accept(listener, (struct sockaddr *) &remoteaddr, &addrlen);
						if (newfd == -1) {
							perror("accept");
						} else {
							printf( "\nConexion al file desctiptor '%d' aceptada!, el accept() creo el nuevo fd '%d'.\n", i, newfd);
							FD_SET(newfd, &master); // se agrega al set master
							fdmax = (fdmax < newfd) ? newfd : fdmax; // mantener cual es el fd mas grande
							imprimir_quien_se_conecto(remoteaddr);
							printf("en el socket '%d'\n", newfd);
							}
					}
					else if(i == 0){
						fgets(bufferLeido, 100, stdin);
						instr_t * request_recibida = leer_a_instruccion(bufferLeido);
						puts("Recibi la siguiente instruccion desde la consola: ");
						print_instruccion(request_recibida);
						ejecutar_requestRecibido(request_recibida, conexionReceptor);
					}

					else { // Ya se había hecho accept en el fd
							 //recibir los mensajes
							instr_t * instrcuccion_recibida;
							int recibo = recibir_request(i, &instrcuccion_recibida);
							if (recibo == 0) {
								printf(COLOR_ANSI_ROJO "El cliente se desconecto" COLOR_ANSI_RESET "\n"); //TODO: Agregar logger
								perror("recv");
								FD_CLR(i, &master);
							} else {
								puts("Recibi la siguiente instruccion: ");
								print_instruccion(instrcuccion_recibida);
								ejecutar_requestRecibido(instrcuccion_recibida, conexionReceptor);
							}
					} // END recibir los mensajes
				} // END tenemos una nueva conexion entrante
			} // END recorriendo los fd
		}
	} // END while(1)
	return 0;
}

void imprimir_quien_se_conecto(struct sockaddr_storage remoteaddr) {
	char remoteIP[INET6_ADDRSTRLEN];
	char* ip_cliente = inet_ntop(remoteaddr.ss_family,
						&(((struct sockaddr_in *) &remoteaddr)->sin_addr),
						remoteIP,
						INET_ADDRSTRLEN);
	char* nombreCliente;
	if (strcmp(ip_cliente, IP_MEMORIA) == 0)
		nombreCliente = strdup("Memoria");
	else if (strcmp(ip_cliente, IP_FILESYSTEM) == 0)
		nombreCliente = strdup("File System");
	else if (strcmp(ip_cliente, IP_KERNEL) == 0)
		nombreCliente = strdup("Kernel");
	else
		nombreCliente = strdup("Nuevo cliente");
	printf("\nNueva conexion del cliente %s (%s) ", ip_cliente, nombreCliente);
}








