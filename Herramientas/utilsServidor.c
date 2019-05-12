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

int vigilar_conexiones_entrantes(int listener){

	//Gracias a la guia de Beej:
	fd_set master;    // lista 'master' de file descriptors
	fd_set read_fds;  // lista temporal de fds para el select()
	int fdmax;        // mayor file descriptor (para recorrerlos todos con el for)

	int newfd;        // nuevos fds a los que se les hizo accept()
	struct sockaddr_storage remoteaddr; // direccion del cliente
	socklen_t addrlen;

	char send_buf[100];
	int i;

	FD_ZERO(&master); //los vaciamos
	FD_ZERO(&read_fds);

	FD_SET(listener, &master); // agregamos a los fds que vigila select()
	FD_SET(0, &master); // 0 es el fd de la consola

	// mantener cual es el fd mas grande (lo pide el seelct())
	fdmax = listener; // por ahora es este

	while(1) {
		read_fds = master;
		puts("\nA continuacion se hace el select, esperando data entrante...");
		printf(">");
		fflush(stdout);
		int resultado = select(fdmax + 1, &read_fds, NULL, NULL, NULL);
		if (resultado == -1) {
			perror("select");
		} else if (resultado > 0) {
			puts("Se hizo el select (se detecto un dato en los fds abiertos)");
			puts("Se recorren las conexiones existentes buscando data para leer");
			//recorrer las conexiones existentes buscando data para leer, empezando desde el fd 0
			for (i = 0; i <= fdmax; i++) {
				printf("Se busco en el fileDesctiptor %d\n", i);
				if (FD_ISSET(i, &read_fds)) { // tenemos una nueva conexion entrante
					printf("\nEncontramos data para leer, en el fileDesctiptor '%d'!\n",i);

					if (i == listener) {
						printf("El file descriptor '%d' ya existia, aceptando nuevas conexiones al mismo...\n",i);
						// manejo de conexiones nuevas:
						addrlen = sizeof remoteaddr;
						newfd = accept(listener, (struct sockaddr *) &remoteaddr, &addrlen);
						if (newfd == -1) {
							perror("accept");
						} else {
							printf( "Conexion al file desctiptor '%d' aceptada!, el accept() creo el nuevo fd '%d'.\n"
									"Se lo agrego a la lista de fds que vigila el select\n", i, newfd);
							FD_SET(newfd, &master); // se agrega al set master
							fdmax = (fdmax < newfd) ? newfd : fdmax; // mantener cual es el fd mas grande
							printf("%s en el socket '%d'", imprimir_quien_se_conecto(remoteaddr), newfd);
							}
					}

					else if(i == 0){
						puts("Nuevo mensaje en la consola");
						gets(send_buf);
						instr_t * request_recibida = leer_a_instruccion(send_buf);
						puts("Recibi la siguiente instruccion: ");
						print_instruccion(request_recibida);

					}

					else { // Ya se había hecho accept en el fd
							 //recibir los mensajes
							instr_t * instrcuccion_recibida;
							int recibo = recibir_request(i, &instrcuccion_recibida);
							if (recibo == 0) {
								printf("\x1b[31m""El cliente se desconecto" "\x1b[0m" "\n"); //TODO: Agregar logger
								perror("recv");
								FD_CLR(i, &master);
							} else {
								puts("Recibi la siguiente instruccionn: ");
								print_instruccion(instrcuccion_recibida);
								//crear_hilo(instruccion_recibida);?
							}
					} // END recibir los mensajes
				} // END tenemos una nueva conexion entrante
			} // END recorriendo los fd
		}
		puts("Fin del while(1)");
	} // END while(1)
	return 0;
}

char* imprimir_quien_se_conecto(struct sockaddr_storage remoteaddr) {
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
	printf("\nNueva conexion del cliente %s (%s)\n", ip_cliente, nombreCliente);
	return ip_cliente;
}








