//-------serializaciones.c-------

#include "serializaciones.h"

t_paquete* instruccion_a_paquete(instr_t* instruccionAEnviar){

	t_paquete* paqueteAEnviar = crear_paquete(instruccionAEnviar->codigo_operacion, instruccionAEnviar->timestamp);
	int tamanioTotal=0;
	void iterator(void* valor){
		int tamanio = strlen((char*)valor)+1;
		agregar_a_paquete(paqueteAEnviar, valor, tamanio);
		tamanioTotal+=tamanio;
	}
	list_iterate(instruccionAEnviar->parametros, (void*) iterator);
	return paqueteAEnviar;
}


//Para agregar los parametros al paquete
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio) {

	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));//el tamanio del valor y el valor en si

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);

}

void* serializar_paquete(t_paquete* paquete, int bytes) {
	void * magic = malloc(bytes);

	int desplazamiento = 0;
	memcpy(magic + desplazamiento, &(paquete->timestamp), sizeof(mseg_t));
	desplazamiento += sizeof(mseg_t);

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(cod_op));
	desplazamiento += sizeof(cod_op);

	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento += paquete->buffer->size;

	return magic;
}


void* serializar_request(instr_t* instruccionAEnviar, int* tamanio){

	t_paquete* paqueteAEnviar = instruccion_a_paquete(instruccionAEnviar);

	int bytes = paqueteAEnviar->buffer->size + sizeof(int) + sizeof(cod_op)+ sizeof(mseg_t);
	void* paqueteSerializado = serializar_paquete(paqueteAEnviar, bytes);
	*tamanio = bytes;
	return paqueteSerializado;
}


int enviar_request(instr_t* instruccionAEnviar, int socket_cliente){
	int tamanio;
	void* a_enviar = serializar_request(instruccionAEnviar, &tamanio);
	int s = send(socket_cliente, a_enviar, tamanio, 0);
	free (a_enviar);
	return s;
}


//Hay que crear un buffer para los parametros
void crear_buffer(t_paquete* paquete) {
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}


t_paquete* crear_paquete(cod_op nuevoCodOp, mseg_t nuevoTimestamp) {
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




//Deserializacion

void* recibir_buffer(int* size, int socket_cliente) {
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	printf("El tamanio total del buffer recibido es %d\n", *size);
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
	int i = 0; //debug

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size){
		i++;
		memcpy(&tamanio, buffer+desplazamiento, sizeof(int));
		desplazamiento += sizeof(int);
		printf("El tamanio del parametro '%d'es de: %d\n", i, size);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		printf("El valor del primer parametro '%d' es de: %s\n", i, valor);

		desplazamiento += tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
	return NULL;
}


int recibir_request(int socket_cliente, instr_t** instruccion) {
	mseg_t nuevoTimestamp;
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

	(*instruccion) = malloc( sizeof(mseg_t) * sizeof(cod_op) + sizeof(listaParam) );
	(*instruccion) -> timestamp = nuevoTimestamp;
	(*instruccion) -> codigo_operacion = nuevoCodOp;
	(*instruccion) -> parametros = listaParam;

	return 1;
}


int recibir_timestamp(int socket_cliente, mseg_t* nuevoTimestamp) {
	int r = recv(socket_cliente, nuevoTimestamp, sizeof(mseg_t), MSG_WAITALL);
	if(r == 0){
		perror("El cliente se desconecto: ");
	}
	if(r < 0){
			perror("Error en el recv: ");
		}
	return r;
}


int recibir_operacion(int socket_cliente, cod_op* nuevaOperacion) {
	int r = recv(socket_cliente, nuevaOperacion, sizeof(cod_op), MSG_WAITALL);
	if(r == 0){
		perror("El cliente se desconecto: ");
	}
	if(r < 0){
			perror("Error en el recv: ");
		}
	return r;
}


instr_t* leer_a_instruccion(char* request){
	char* requestCopy = strdup(request);
	char *actual, *comando, *valor;
	comando = NULL;
	valor = malloc(sizeof(int));
	t_list* listaParam = list_create();

	if(strcmp(requestCopy, "CERRAR\n")==0){
		printf("\x1b[1;36mGracias por usar Lissandra FS!\n \x1b[0m cerrando...\n");
		exit(0);
	}

	actual = strtok (requestCopy, " ");
	comando = strdup(actual);
	actual = strtok (NULL, " ");

	for(int i=1; actual != NULL; i++){
		valor = strdup(actual);
		list_add(listaParam, valor);

		if(i==1 && strcmp(comando, "INSERT")==0){
			actual = strtok (NULL, "");
			valor = strdup(actual);
			list_add(listaParam, valor);
			break;
		}
		actual = strtok (NULL, " ");
	}
	free(requestCopy);

	instr_t *miInstr = crear_instruccion(obtener_ts(), reconocer_comando(comando), listaParam);

	return miInstr;
}


instr_t* crear_instruccion(mseg_t timestampNuevo, cod_op codInstNuevo, t_list* listaParamNueva){

	instr_t instruccionCreada ={
		.timestamp = timestampNuevo,
		.codigo_operacion = codInstNuevo,
		.parametros = listaParamNueva
	};

	instr_t* miInstr = malloc(sizeof(instruccionCreada));
	memcpy(miInstr, &instruccionCreada, sizeof(instruccionCreada));

	return miInstr;
}


cod_op reconocer_comando(char* comando){

	if (strcmp(comando, "SELECT")==0) {
		puts("Se detecto comando 'SELECT'\n");
		return CODIGO_SELECT;
	}
	else if (strcmp(comando, "INSERT")==0) {
		puts("Se detecto comando 'INSERT'\n");
		return CODIGO_INSERT;
	}
	else if (strcmp(comando, "CREATE")==0) {
		puts("Se detecto comando 'CREATE'\n");
		return CODIGO_CREATE;
	}
	else if (strcmp(comando, "DESCRIBE")==0) {
		puts("Se detecto comando 'DESCRIBE'\n");
		return CODIGO_DESCRIBE;
	}
	else if (strcmp(comando, "DROP")==0) {
		puts("Se detecto comando 'DROP'\n");
		return CODIGO_DROP;
	}
	else{ 	/* if (strcmp(comando, "JOURNAL")==0) */
		puts("Se detecto comando 'JOURNAL'\n");
		return CODIGO_JOURNAL;
	}
	/*
	else{
		puts("Comando invalido\n\n");
		return ERROR_INPUT;
	}
	*/
}


void print_instruccion(instr_t* instruccion){

	void iterator(char* value){
		printf("%s\n", value);
	}

	printf("Timestamp: %u \n", (unsigned int)instruccion->timestamp);
	printf("CodigoInstruccion: %d\n", instruccion->codigo_operacion);
	printf("Parametros:\n");
	list_iterate(instruccion->parametros, (void*) iterator);
	puts("\n");
}










