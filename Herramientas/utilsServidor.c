//-------utilsServidor.c-------

#include "utilsServidor.h"

/*
 * socket() y bind()
 */

int iniciar_servidor(char *ip_proceso, char *puerto_a_abrir)
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
		loggear_error(string_from_format("Error al crear el socket servidor"));
		//TODO: Cerrar socket servidor?
		return -1;
	}
	int aceptar = 1;
	setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &aceptar, sizeof(int));
	if (bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen) < 0)
	{
		loggear_error( string_from_format("Error al vincular el socket servidor con el puerto"));
		close(socket_servidor);
		return -1;
	}

	if ((listen(socket_servidor, SOMAXCONN)) < 0)
	{ //TODO: queremos SO MAX CONNECTIONS?, es 128
		loggear_error(string_from_format("Error al dejar todo listo para escuchar en socket servidor"));
		//TODO: Cerrar socket servidor?
		return -1;
	}
	freeaddrinfo(servinfo);
	loggear_info(string_from_format("Listo para escuchar a mi cliente"));

	listener = socket_servidor;

	return socket_servidor;
}

int vigilar_conexiones_entrantes(
		void (*ejecutar_requestRecibido)(instr_t *instruccionAEjecutar, char *remitente),
		int queConsola)
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

	while (1)
	{
		read_fds = master;
		printf("\n" COLOR_ANSI_MAGENTA ">" COLOR_ANSI_RESET);
		fflush(stdout);
		int resultado = select(fdmax + 1, &read_fds, NULL, NULL, NULL);
		if (resultado == -1)
		{
			loggear_error(string_from_format("Error al realizar el select(): %s\n", strerror(errno)));
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
							loggear_error(string_from_format("Error al realizar el accept(): %s\n", strerror(errno)));
						}
						else
						{
							loggear_trace(string_from_format("select() iniciado"));
							//							char* ipCliente = ip_cliente(remoteaddr);
							FD_SET(newfd, &master);					 // se agrega al set master
							fdmax = (fdmax < newfd) ? newfd : fdmax; // mantener cual es el fd mas grande

							instr_t *instruccion_handshake;
							recibir_request(newfd, &instruccion_handshake);
							loggear_trace(string_from_format("recibida instruccion handshake"));
							char *quienEs = strdup((char *)list_get(instruccion_handshake->parametros, 0)); //El nombre
							loggear_debug(string_from_format("Se conecto %s\n", quienEs));

							sem_wait(&mutex_diccionario_conexiones);
							if (dictionary_get(conexionesActuales, quienEs) != NULL)
							{ //Ya lo conocia, no tenia su fd_in
								loggear_debug(string_from_format("Ya conocia a %s, no tenia su fd_in\n", quienEs));
								identificador *miIdentificador = (identificador *)dictionary_get(conexionesActuales, quienEs);
								miIdentificador->fd_in = newfd;
								dictionary_put(conexionesActuales, quienEs, miIdentificador); //TODO: Hace falta? O al cambiar lo apuntado por el puntero ya esta?
								sem_post(&mutex_diccionario_conexiones);
							}
							else
							{ //No lo conocia
								loggear_debug(string_from_format("No conocia a %s\n", quienEs));
								sem_post(&mutex_diccionario_conexiones);
								char *suIP = strdup((char *)list_get(instruccion_handshake->parametros, 1));	 //Su IP, quizás se más fácil usar ip_cliente(remoteaddr)
								char *suPuerto = strdup((char *)list_get(instruccion_handshake->parametros, 2)); //Su Puerto
								identificador *idsNuevaConexion = malloc(sizeof(identificador));
								strcpy(idsNuevaConexion->puerto, suPuerto);
								strcpy(idsNuevaConexion->ip_proceso, suIP);
								idsNuevaConexion->fd_in = newfd;
								idsNuevaConexion->fd_out = (fd_out_inicial)?fd_out_inicial:0;
								fd_out_inicial = 0;
								sem_wait(&mutex_diccionario_conexiones);
								dictionary_put(conexionesActuales, quienEs, idsNuevaConexion);
								sem_post(&mutex_diccionario_conexiones);
							}
							char* auxFd = string_from_format("%d", newfd);
//							if(dictionary_get(auxiliarConexiones, auxFd)!=NULL){
//								free(dictionary_get(auxiliarConexiones, auxFd));
//							}
							loggear_trace(string_from_format("Se va a hacer free de los parametros de la instruccion handshake"));
							list_destroy_and_destroy_elements(instruccion_handshake->parametros, free);
							free(instruccion_handshake);
							loggear_trace(string_from_format("Instruccion handshake freed"));
							dictionary_put(auxiliarConexiones, auxFd, quienEs);
							free(auxFd);
							loggear_trace(string_from_format("auxFd Freed"));
							//							imprimirConexiones(conexionesActuales); //Debug
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
						int recibo = recibir_request(i, &instrcuccion_recibida);
						char* auxFd = string_from_format("%d", i);
						char* quienLoEnvia = dictionary_get(auxiliarConexiones, auxFd);
						free(auxFd);

						if (recibo == 0)
						{
							loggear_warning(string_from_format("El cliente '%s'se desconecto\n", quienLoEnvia));
							FD_CLR(i, &master);
							sem_wait(&mutex_diccionario_conexiones);
							dictionary_remove(conexionesActuales, quienLoEnvia); //No and_destroy porque se libera solo afuera del else
							sem_post(&mutex_diccionario_conexiones);
						}

						else
						{ //Se recibio una instruccion desde otro proceso
							loggear_trace(string_from_format("Se recibio una instruccion de otro proceso"));
							ejecutar_requestRecibido(instrcuccion_recibida, quienLoEnvia);
						}

					} // END recibir los mensajes
				}	 // END tenemos una nueva conexion entrante
			}		  // END recorriendo los fd
		}
	} // END while(1)
	return 0;
}


//TODO Unused code
char *ip_cliente(struct sockaddr_storage remoteaddr)
{
	char remoteIP[INET6_ADDRSTRLEN];
	return (char *)inet_ntop(remoteaddr.ss_family, &(((struct sockaddr_in *)&remoteaddr)->sin_addr), remoteIP, INET_ADDRSTRLEN);
}
