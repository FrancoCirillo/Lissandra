//-------utilsCliente.c-------

#include "utilsCliente.h"

int conectar_con_proceso(cod_proceso proceso, cod_proceso procesoCliente){
	char* ip_proceso; //IP del Servidor
	char* ip_cliente;
	//	char* puerto_proceso; TODO

		switch (proceso) {
		case (KERNEL):
			ip_proceso = strdup(IP_KERNEL);
			break;
		case(FILESYSTEM):
			ip_proceso = strdup(IP_FILESYSTEM);
			break;
		case(MEMORIA):
			ip_proceso = strdup(IP_MEMORIA);
			break;
		default:
	//TODO	log_warning(logger, "ERROR!");
			puts("EL CODIGO DEL PROCESO NO ES CORRECTO");
			break;
		}
		switch (procesoCliente) {
		case (KERNEL):
			ip_cliente = strdup(IP_KERNEL);
			break;
		case(FILESYSTEM):
			ip_cliente = strdup(IP_FILESYSTEM);
			break;
		case(MEMORIA):
			ip_cliente = strdup(IP_MEMORIA);
			break;
		default:
	//TODO	log_warning(logger, "ERROR!");
			puts("EL CODIGO DEL PROCESO NO ES CORRECTO");
			break;
		}

		puts("Creando conexion...");

		//conexion es el socket_cliente
		return crear_conexion(ip_proceso, PORT, ip_cliente);
}
//Conecta el socket_cliente con el Servidor con ip ip y puerto puerto.
int crear_conexion(char *ip, char* puerto, char* miIP)
{
	//TODO: recibir el logger
	struct addrinfo hints;//Es para pasarle nuestras preferencias a getaddrinfo
	struct addrinfo *server_info;
	int socket_cliente;

	memset(&hints, 0, sizeof(hints)); //Se llena toda la estructura de 0s (por las dudas)
	//Relleno default de addrinfo (no tiene importancia):
	hints.ai_family = AF_UNSPEC; //No importa si es ipv4 o ipv6
	hints.ai_socktype = SOCK_STREAM; //Usamos TCP
	hints.ai_flags = AI_PASSIVE; //Rellena la IP por nosotros TODO:Chequear si queremos esto. Creo que igual no importa xq llenamos el primer argumento de getaddrinfo con ip


	//Rellena la estructura server_info con la info del Servidor (En realidad es un addrinfo**)
	getaddrinfo(ip, puerto, &hints, &server_info);
	//TODO: Chqeuear los errores de getaddrinfo()

	//Crea el socket_cliente
	//TODO: recorrer la lista "server_info" en vez de asumir que el primero funciona (Beeje's)
	if((socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol))<0){
		printf("Error al crear el socket cliente \n");
//		log_error(logger, "Error al crear el socket cliente");
		close(socket_cliente);
		return -1;//
	}

	//Para especificar mi IP como cliente
	struct sockaddr_in localaddr;
	localaddr.sin_family = AF_INET;
	localaddr.sin_addr.s_addr = inet_addr(miIP);
	localaddr.sin_port = 0;  // Any local port will do
	bind(socket_cliente, (struct sockaddr *)&localaddr, sizeof(localaddr));

	//ai_addr contiene el puerto e ip del servidor
	while(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1){
		puts("Error: no se pudo conectar con el Servidor, reintentando...");
		sleep(2); //TODO: log de error
	}
	freeaddrinfo(server_info);

	puts("Conectado como Cliente");
	//una vez conectado, se pueden enviar cosas a traves del socket_cliente
	return socket_cliente;
}
