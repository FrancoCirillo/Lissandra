//-------utilsServidor.c-------

#include "utilsServidor.h"

/*
 * socket() y bind()
 */

int iniciar_servidor(char *ip_proceso, char *puerto_a_abrir, t_log *logger, sem_t *mutex_log)
{
	int socket_servidor;

	struct addrinfo hints, *servinfo; //Estructuras solo para llenar servinfo

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;	 //No importa si es ipv4 o ipv6
	hints.ai_socktype = SOCK_STREAM; //Usamos TCP
	hints.ai_flags = AI_PASSIVE;	 //Rellena la IP por nosotros

	//Aca se llenan los datos de servinfo:
	getaddrinfo(ip_proceso, puerto_a_abrir, &hints, &servinfo); //TODO: chequear los errores de getaddrinfo

	if ((socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) < 0)
	{
		loggear_error(logger, mutex_log, string_from_format("Error al crear el socket servidor"));
		//TODO: Cerrar socket servidor?
		return -1;
	}
	int aceptar;
	setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &aceptar, sizeof(int));
	if (bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen) < 0)
	{
		loggear_error(logger, mutex_log,  string_from_format("Error al vincular el socket servidor con el puerto"));
		close(socket_servidor);
		return -1;
	}

	if ((listen(socket_servidor, SOMAXCONN)) < 0)
	{ //TODO: queremos SO MAX CONNECTIONS?, es 128
		loggear_error(logger, mutex_log, string_from_format("Error al dejar todo listo para escuchar en socket servidor"));
		//TODO: Cerrar socket servidor?
		return -1;
	}
	freeaddrinfo(servinfo);

	loggear_info(logger, mutex_log, string_from_format("Listo para escuchar a mi cliente"));

	return socket_servidor;
}

int vigilar_conexiones_entrantes(
		int listener,
		void (*ejecutar_requestRecibido)(instr_t *instruccionAEjecutar, char *remitente),
		t_dictionary *conexionesConocidas,
		int queConsola,
		t_log *logger,
		sem_t *mutex_log)
{

	//Gracias a la guia de Beej:
	fd_set master;   // lista 'master' de file descriptors
	fd_set read_fds; // lista temporal de fds para el select()
	int fdmax;		 // mayor file descriptor (para recorrerlos todos con el for)

	int newfd;							// nuevos fds a los que se les hizo accept()
	struct sockaddr_storage remoteaddr; // direccion del cliente
	socklen_t addrlen;

	char bufferLeido[TAMANIO_MAX_INPUT_CONSOLA];
	int i;
	FD_ZERO(&master); //los vaciamos
	FD_ZERO(&read_fds);

	FD_SET(listener, &master); // agregamos a los fds que vigila select()
	FD_SET(0, &master);		   // 0 es el fd de la consola

	// mantener cual es el fd mas grande (lo pide el select())
	fdmax = listener; // por ahora es este
	t_dictionary *auxiliarConexiones = dictionary_create();

	while (1)
	{
		read_fds = master;
		printf("\n" COLOR_ANSI_MAGENTA ">" COLOR_ANSI_RESET);
		fflush(stdout);
		int resultado = select(fdmax + 1, &read_fds, NULL, NULL, NULL);
		if (resultado == -1)
		{
			loggear_error(logger, mutex_log, string_from_format("Error al realizar el select(): %s\n", strerror(errno)));
		}
		else if (resultado > 0)
		{
			//recorrer las conexiones existentes buscando data para leer, empezando desde el fd 0
			for (i = 0; i <= fdmax; i++)
			{
				if (FD_ISSET(i, &read_fds))
				{
					if (i == listener)
					{
						// manejo de conexiones nuevas:
						addrlen = sizeof remoteaddr;
						newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);
						if (newfd == -1)
						{
							loggear_error(logger,mutex_log, string_from_format("Error al realizar el accept(): %s\n", strerror(errno)));
						}
						else
						{
							//							char* ipCliente = ip_cliente(remoteaddr);
							FD_SET(newfd, &master);					 // se agrega al set master
							fdmax = (fdmax < newfd) ? newfd : fdmax; // mantener cual es el fd mas grande

							instr_t *instruccion_handshake;
							recibir_request(newfd, &instruccion_handshake, logger, mutex_log);

							char *quienEs = (char *)list_get(instruccion_handshake->parametros, 0); //El nombre
							loggear_debug(logger, mutex_log, string_from_format("Se conecto %s\n", quienEs));

							if (dictionary_get(conexionesConocidas, quienEs) != NULL)
							{ //Ya lo conocia, no tenia su fd_in
								identificador *miIdentificador = (identificador *)dictionary_get(conexionesConocidas, quienEs);
								miIdentificador->fd_in = newfd;
								dictionary_put(conexionesConocidas, quienEs, miIdentificador); //TODO: Hace falta? O al cambiar lo apuntado por el puntero ya esta?
							}
							else
							{
								char *suIP = (char *)list_get(instruccion_handshake->parametros, 1);	 //Su IP, quizás se más fácil usar ip_cliente(remoteaddr)
								char *suPuerto = (char *)list_get(instruccion_handshake->parametros, 2); //Su Puerto
								identificador *idsNuevaConexion = malloc(sizeof(identificador));
								strcpy(idsNuevaConexion->puerto, suPuerto);
								strcpy(idsNuevaConexion->ip_proceso, suIP);
								idsNuevaConexion->fd_in = newfd;
								idsNuevaConexion->fd_out = 0;
								dictionary_put(conexionesConocidas, quienEs, idsNuevaConexion);
							}
							char* auxFd = string_from_format("%d", newfd);
							dictionary_put(auxiliarConexiones, auxFd, quienEs);
							free(auxFd);
							//							imprimirConexiones(conexionesConocidas); //Debug
						}
					}
					else if (i == 0)
					{ //Recibido desde la consola
						fgets(bufferLeido, TAMANIO_MAX_INPUT_CONSOLA, stdin);
						instr_t *request_recibida = leer_a_instruccion(bufferLeido, queConsola);
						if (request_recibida != NULL)
							ejecutar_requestRecibido(request_recibida, 0);
					}

					else
					{// Ya se había hecho accept en el fd
						//recibir los mensajes
						instr_t *instrcuccion_recibida;
						int recibo = recibir_request(i, &instrcuccion_recibida, logger, mutex_log);
						char* auxFd = string_from_format("%d", i);
						char* quienLoEnvia = dictionary_get(auxiliarConexiones, auxFd);
						free(auxFd);

						if (recibo == 0)
						{
							loggear_warning(logger,mutex_log, string_from_format("El cliente '%s'se desconecto\n", quienLoEnvia));
							FD_CLR(i, &master);
							dictionary_remove(conexionesConocidas, quienLoEnvia); //No and_destroy porque se libera solo afuera del else
						}

						else
						{ //Se recibio una instruccion desde otro proceso
							ejecutar_requestRecibido(instrcuccion_recibida, quienLoEnvia);
						}

					} // END recibir los mensajes
				}	 // END tenemos una nueva conexion entrante
			}		  // END recorriendo los fd
		}
	} // END while(1)
	return 0;
}

char *ip_cliente(struct sockaddr_storage remoteaddr)
{
	char remoteIP[INET6_ADDRSTRLEN];
	return (char *)inet_ntop(remoteaddr.ss_family, &(((struct sockaddr_in *)&remoteaddr)->sin_addr), remoteIP, INET_ADDRSTRLEN);
}
