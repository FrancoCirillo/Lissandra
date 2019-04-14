/*
 * conexiones.c
 *
 *  Created on: 2 mar. 2019
 *      Author: utnso
 */

#include "utilsCliente.h"

int conectar_con_proceso(cod_proceso proceso){
	char* ip_proceso; //IP del Servidor
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

		puts("Creando conexion...");

		//conexion es el socket_cliente
		return crear_conexion(ip_proceso, PORT);
}

//Conecta el socket_cliente con el Servidor con ip ip y puerto puerto.
int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;//Es para pasarle nuestras preferencias a getaddrinfo
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints)); //Se llena toda la estructura de 0s (por las dudas)
	//Relleno default de addrinfo (no tiene importancia):
	hints.ai_family = AF_UNSPEC; //No importa si es ipv4 o ipv6
	hints.ai_socktype = SOCK_STREAM; //Usamos TCP
	hints.ai_flags = AI_PASSIVE; //Rellena la IP por nosotros TODO:Chequear si queremos esto. Creo que igual no importa xq llenamos el primer argumetno de getaddrinfo con ip


	//Rellena la estructura server_info con la info del Servidor (En realidad es un addrinfo**)
	getaddrinfo(ip, puerto, &hints, &server_info);
	//TODO: Chqeuear los errores de getaddrinfo()

	//Crea el socket_cliente
	//TODO: recorrer la lista "server_info" en vez de asumir que el primero funciona (Beeje's)
	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

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

//Envia mensaje a traves de socket_cliente
void enviar_mensaje(char* mensaje, int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	crear_buffer(paquete);
	paquete->codigo_operacion = MENSAJE; //MENSAJE o PAQUETE
	paquete->buffer->size = strlen(mensaje) + 1;//strlen no cuenta el el \0
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size); //Copia el mensaje al buffer

	int bytes = paquete->buffer->size + 2*sizeof(int);//un int por op_code codigo_operacion, otro por int size  (Creo)
	//TODO: Revisar si usar 2*sizeof(int) o usar sizeof(paquete->buffer->size) + sizeof(paquete->codigo_operacion)

	//Convierto de struct paquete a void* (Conjunto de bytes ""sin tipo"")
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

//Necesita la el tamanio total del paquete (bytes)
void* serializar_paquete(t_paquete* paquete, int bytes) //Not to be confused with serializar() :P
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	//Guardo en magic el codigo_operacion (MENSAJE o PAQUETE)
	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(paquete->codigo_operacion));
	desplazamiento+= sizeof(paquete->codigo_operacion);

	//Guardo en magic el el tamanio (size) del PAQUETE o MENSAJE (stream)
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(paquete->buffer->size));
	desplazamiento+= sizeof(paquete->buffer->size);

	//Teniendo el tamanio (size) del MENSAJE o PAQUETE (stream), lo puedo guardar en magic
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}
//TODO: solucionar mal modelado envio de mensajes (revisar enviar_mensaje/paquete, serializar, serializar_paquete)
char* serializar(instr *x){
	char* caracterSeparador=";";
	char* codigoInstruccion=malloc(64);
	// Obtengo cantidad de digitos del codigo y lo multiplico por el tamanio de un char+ tamanio de campos+tamanio de ;
	char* mensajeSerializado=malloc(sizeof(char)*(digitosDe(x->codigoInstruccion))+sizeof(x->param1)+sizeof(x->param2)+sizeof(x->param3)+sizeof(x->param4)+sizeof(char)*6);
	printf("\n-------------------\nComienza serializacion\n");

	sprintf(codigoInstruccion,"%d",x->codigoInstruccion);
	strcat(mensajeSerializado,codigoInstruccion);
	strcat(mensajeSerializado,caracterSeparador);

	strcat(mensajeSerializado,x->param1);
	strcat(mensajeSerializado,caracterSeparador);

	strcat(mensajeSerializado,x->param2);
	strcat(mensajeSerializado,caracterSeparador);

	strcat(mensajeSerializado,x->param3);
	strcat(mensajeSerializado,caracterSeparador);

	strcat(mensajeSerializado,x->param4);
	strcat(mensajeSerializado,caracterSeparador);

	printf("\n------------\nSe serializo correctamente, el resultado es: %s\n",mensajeSerializado);
	free(codigoInstruccion);
	return mensajeSerializado;
}
int digitosDe(int x){
    int count=0;
	if(x==0){
		return 1;
	}
    while(x != 0)
    {
        // n = n/10
        x /= 10;
        ++count;
    }
	return count;
}


//Inicializa un buffer y lo llena de vacio
void crear_buffer(t_paquete* paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

//Funciones de PAQUETE:

//t_paquete* crear_super_paquete(void)
//{
//	//me falta un malloc!
//	t_paquete* paquete;
//
//	//descomentar despues de arreglar
//	//paquete->codigo_operacion = PAQUETE;
//	//crear_buffer(paquete);
//	return paquete;
//}

t_paquete* crear_paquete(void)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = PAQUETE;
	crear_buffer(paquete);
	return paquete;
}

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete(t_paquete* paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}
