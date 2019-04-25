//-------serializaciones.c-------

#include "serializaciones.h"

void* serializar_paquete(t_paquete* paquete, int bytes) {
	void * magic = malloc(bytes);

	int desplazamiento = 0;
	memcpy(magic + desplazamiento, &(paquete->timestamp), sizeof(time_t));

	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(cod_op));

	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));

	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);

	desplazamiento += paquete->buffer->size;

	return magic;
}

//Hay que crear un buffer cada parametro
void crear_buffer(t_paquete* paquete) {
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

t_paquete* crear_paquete(cod_op nuevoCodOp, time_t nuevoTimestamp) {
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->timestamp = nuevoTimestamp;
	paquete->codigo_operacion = nuevoCodOp;
	crear_buffer(paquete);
	return paquete;
}

void eliminar_paquete(t_paquete* paquete) {
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

//Para agregar los parametros al paquete
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio) {
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}


//Deserializacion

void* recibir_buffer(int* size, int socket_cliente) {
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

t_list* recibir_paquete(int socket_cliente) {
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size){
		memcpy(&tamanio, buffer+desplazamiento, sizeof(int));
		desplazamiento += sizeof(int);

		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);

		desplazamiento += tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
	return NULL;
}

int recibir_request(int socket_cliente, instr_t** instruccion) {
	time_t nuevoTimestamp;
	cod_op nuevoCodOp;
	t_list* listaParam;

	int t = recibir_timestamp(socket_cliente, &nuevoTimestamp);    //return en error
	if(t<=0)
		return t;

	t = recibir_operacion(socket_cliente, &nuevoCodOp);
	if(t<=0)
		return t;

	listaParam = recibir_paquete(socket_cliente);      //del TP0

	//Fijarse si el "(*instruccion) -> algo" funciona

	(*instruccion) = malloc( sizeof(time_t) * sizeof(cod_op) + sizeof(listaParam) );
	(*instruccion) -> timestamp = nuevoTimestamp;
	(*instruccion) -> codigo_operacion = nuevoCodOp;
	(*instruccion) -> parametros = listaParam;

	return 0;
}

int recibir_timestamp(int socket_cliente, time_t* nuevoTimestamp) {
	return recv(socket_cliente, nuevoTimestamp, sizeof(time_t), MSG_WAITALL);
}

int recibir_operacion(int socket_cliente, cod_op* nuevaOperacion) {
	return recv(socket_cliente, nuevaOperacion, sizeof(cod_op), MSG_WAITALL);
}

t_paquete* instruccion_a_paquete(instr_t* instruccionAEnviar){

	t_paquete* paqueteAEnviar = crear_paquete(instruccionAEnviar->codigo_operacion, instruccionAEnviar->timestamp);

	void iterator(void* valor){
		int tamanio = strlen((char*)valor)+1;
		agregar_a_paquete(paqueteAEnviar, valor, tamanio);
	}

	list_iterate(instruccionAEnviar->parametros, (void*) iterator);

	return paqueteAEnviar;
}

void* serializar_request(instr_t* instruccionAEnviar, int* tamanio){

	t_paquete* paqueteAEnviar = instruccion_a_paquete(instruccionAEnviar);
	int bytes = paqueteAEnviar->buffer->size + 2*sizeof(int) + sizeof(time_t);
	void* paqueteSerializado = serializar_paquete(paqueteAEnviar, bytes);

	return paqueteSerializado;
}

int enviar_request(instr_t* instruccionAEnviar, int socket_cliente){
	int tamanio;
	void* a_enviar = serializar_request(instruccionAEnviar, &tamanio);
	int s = send(socket_cliente, a_enviar, tamanio, 0);
	free (a_enviar);
	return s;
}

void print_instruccion(instr_t* instruccion){

	void iterator(char* value){
		printf("%s\n", value);
	}

	printf("Timestamp: %lld\n", (long long)instruccion->timestamp);
	printf("CodigoInstruccion: %d\n", instruccion->codigo_operacion);
	printf("Parametros:\n");
	list_iterate(instruccion->parametros, (void*) iterator);
}






