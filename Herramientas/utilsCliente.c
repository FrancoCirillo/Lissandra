//-------utilsCliente.c-------

#include "utilsCliente.h"

//Conecta el socket_cliente con el Servidor con ip ip y puerto puerto.
int crear_conexion(char *ip, char *puerto, char *miIP, int flagReintentar)
{
	struct addrinfo hints; //Es para pasarle nuestras preferencias a getaddrinfo
	struct addrinfo *server_info;
	int socket_cliente;

	memset(&hints, 0, sizeof(hints)); //Se llena toda la estructura de 0s (por las dudas)
	//Relleno default de addrinfo (no tiene importancia):
	hints.ai_family = AF_UNSPEC;	 //No importa si es ipv4 o ipv6
	hints.ai_socktype = SOCK_STREAM; //Usamos TCP
	hints.ai_flags = AI_PASSIVE;	 //Rellena la IP por nosotros TODO:Chequear si queremos esto. Creo que igual no importa xq llenamos el primer argumento de getaddrinfo con ip

	//Rellena la estructura server_info con la info del Servidor (En realidad es un addrinfo**)
	int addr = getaddrinfo(ip, puerto, &hints, &server_info);

	//Puede pasar durante el gossiping
	if(addr!=0){
		loggear_error(string_from_format("Error al hacer getaddrinfo: %s", gai_strerror(addr)));
		freeaddrinfo(server_info);
		return addr;
	}

	//Crea el socket_cliente
	//TODO: recorrer la lista "server_info" en vez de asumir que el primero funciona (Beeje's)

	if ((socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol)) < 0)
	{
		loggear_error(string_from_format("Error al crear el socket cliente  %s\n", strerror(errno)));
		close(socket_cliente);
		return socket_cliente; //
	}

	//Para especificar mi IP como cliente
	struct sockaddr_in localaddr;
	localaddr.sin_family = AF_INET;
	localaddr.sin_addr.s_addr = inet_addr(miIP);
	localaddr.sin_port = 0; // Any local port will do
	bind(socket_cliente, (struct sockaddr *)&localaddr, sizeof(localaddr));

	//ai_addr contiene el puerto e ip del servidor
	if (connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
	{
		if(flagReintentar){
			loggear_warning(string_from_format("El proceso necesita otros servicios para funcionar.\nPor favor inicielos.\nReintentado..\n"));
			while (connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1);
		}
		else {
			freeaddrinfo(server_info);
			return -1;
		}
	}
	loggear_error(string_from_format("Cree una conexion con el socket %d", socket_cliente));
	freeaddrinfo(server_info);

	//	printf("Ya se pueden enviar instrucciones a %s\n", ip); //Debug
	//una vez conectado, se pueden enviar cosas a traves del socket_cliente
	return socket_cliente;
}
