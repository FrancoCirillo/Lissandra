//-------serializaciones.c-------

#include "serializaciones.h"

t_paquete *instruccion_a_paquete(instr_t *instruccionAEnviar)
{

	t_paquete *paqueteAEnviar = crear_paquete(instruccionAEnviar->codigo_operacion, instruccionAEnviar->timestamp);
	int tamanioTotal = 0;
	int tamanio = 0;
	void iterator(char *valor)
	{
		tamanio = strlen(valor) + 1;
		agregar_a_paquete(paqueteAEnviar, valor, tamanio);
		tamanioTotal += tamanio;
	}
	list_iterate(instruccionAEnviar->parametros, (void *)iterator);
	return paqueteAEnviar;
}

//Para agregar los parametros al paquete
void agregar_a_paquete(t_paquete *paquete, void *valor, int tamanio)
{

	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int)); //el tamanio del valor y el valor en si

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void *serializar_paquete(t_paquete *paquete, int bytes)
{
	void *magic = calloc(1, bytes);

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

void *serializar_request(instr_t *instruccionAEnviar, int *tamanio)
{
	t_paquete *paqueteAEnviar = instruccion_a_paquete(instruccionAEnviar);
	if (paqueteAEnviar->buffer->size < 0)
	{ //Pasa si una instruccion no tiene parametros
		paqueteAEnviar->buffer->size = 0;
	}
	int bytes = paqueteAEnviar->buffer->size + sizeof(int) + sizeof(cod_op) + sizeof(mseg_t);
	void *paqueteSerializado = serializar_paquete(paqueteAEnviar, bytes);
	*tamanio = bytes;
	eliminar_paquete(paqueteAEnviar);
	return paqueteSerializado;
}

int enviar_request(instr_t *instruccionAEnviar, int socket_cliente)
{
	int tamanio=0;
	void *a_enviar = NULL;
	a_enviar = serializar_request(instruccionAEnviar, &tamanio);
	int s = send(socket_cliente, a_enviar, tamanio, MSG_DONTWAIT);

//	loggear_trace(string_from_format("Se va a destruir la lista de parametros de la instruccion"));
	list_destroy(instruccionAEnviar->parametros);

//	loggear_trace(string_from_format("Se va a hacer free(instruccionAEnviar"));
	free(instruccionAEnviar);

//	loggear_trace(string_from_format("Se va a hacer free(a_enviar)"));
	free(a_enviar);

	loggear_trace(string_from_format("Request enviado. Instruccion y parametros libreados"));
	return s;
}

int enviar_liberando_request(instr_t *instruccionAEnviar, int socket_cliente)
{
	t_list* listaABorrar = list_duplicate(instruccionAEnviar->parametros);
	int s = enviar_request(instruccionAEnviar, socket_cliente);
	list_destroy_and_destroy_elements(listaABorrar, free);
	return s;
}

//Hay que crear un buffer para los parametros
void crear_buffer(t_paquete *paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer)); //free en eliminar_paquete
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

t_paquete *crear_paquete(cod_op nuevoCodOp, mseg_t nuevoTimestamp)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));//free en eliminar_paquete
	paquete->timestamp = nuevoTimestamp;
	paquete->codigo_operacion = nuevoCodOp;
	crear_buffer(paquete);
	return paquete;
}

void eliminar_paquete(t_paquete *paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

//Deserializacion

void *recibir_buffer(int *size, int socket_cliente)
{
	void *buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);//free en recibir_paquete
	if (*size != 0)
		recv(socket_cliente, buffer, *size, MSG_WAITALL); //Pasa cuando recibimos las requests sin parametros
	return buffer;
}

void recibir_paquete(int socket_cliente, t_list * valores)
{
	int size;
	int desplazamiento = 0;
	void *buffer;
	int tamanio;

//	loggear_trace(string_from_format("Se va a recibir_buffer"));
	buffer = recibir_buffer(&size, socket_cliente);
//	loggear_trace(string_from_format("buffer recibido, copiandolo"));
	while (desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento += sizeof(int);
		char *valor = malloc(tamanio);
		memcpy(valor, buffer + desplazamiento, tamanio);
		desplazamiento += tamanio;
		list_add(valores, valor);
	}
	free(buffer);
//	loggear_trace(string_from_format("free(buffer) hecho"));

}

int recibir_request(int socket_cliente, instr_t **instruccion)
{
	mseg_t nuevoTimestamp;
	cod_op nuevoCodOp;
	int t = recibir_timestamp(socket_cliente, &nuevoTimestamp); //return en error
	if (t <= 0)
		return t;
	t = recibir_operacion(socket_cliente, &nuevoCodOp);
	if (t <= 0)
		return t;
	t_list *listaParam = list_create();

//	loggear_trace(string_from_format("Se va a recibir el buffer!"));
	recibir_paquete(socket_cliente, listaParam); //del TP0
	*instruccion = malloc(sizeof(instr_t));
	(*instruccion)->timestamp = nuevoTimestamp;
	(*instruccion)->codigo_operacion = nuevoCodOp;
	(*instruccion)->parametros = listaParam;
	loggear_trace(string_from_format("Request recibido"));
	return 1;
}

int recibir_timestamp(int socket_cliente, mseg_t *nuevoTimestamp)
{
	int r = recv(socket_cliente, nuevoTimestamp, sizeof(mseg_t), MSG_WAITALL);
	if (r < 0)
	{
		loggear_error(string_from_format("Error en el recv: %s\n", strerror(errno)));
	}
	return r;
}

int recibir_operacion(int socket_cliente, cod_op *nuevaOperacion)
{
	int r = recv(socket_cliente, nuevaOperacion, sizeof(cod_op), MSG_WAITALL);

	if (r == 0)
		loggear_error(string_from_format("El cliente se desconecto en el medio de una recepción."));

	if (r < 0)
		loggear_error(string_from_format("Error en el recv: %s\n", strerror(errno)));

	return r;
}

instr_t *leer_a_instruccion(char *request, int queConsola)
{
	char* requestCopy = string_from_format("%s", request);
	char* actual = strtok(requestCopy, " \n");

	if (actual == NULL){
		free(requestCopy);
		return NULL;
	}

	char* comando = strdup(actual);
	string_to_upper(comando);
	cod_op comandoReconocido = reconocer_comando(comando);

	if(comandoReconocido == INPUT_ERROR){
		free(requestCopy);
		free(comando);
		puts("Input invalido");
		return NULL;
	}

	char* valor; //Va a almacenar parametros
	t_list *listaParam = list_create();
	mseg_t timestampRequest = obtener_ts();

	actual = strtok(NULL, " \n"); //El primer parametro

	//Evaluacion de parametros:
	for (int i = 1; actual != NULL; i++)
	{
		if(comandoReconocido == CODIGO_RUN){
			//string_to_upper no debe afectar al nombre de archivo del RUN
			valor = strdup(actual);
			list_add(listaParam, valor);
		}
		else{
			valor = strdup(actual);
			string_to_upper(valor);
			list_add(listaParam, valor);

			if (i == 2 && comandoReconocido == CODIGO_INSERT)
			{
				actual = strtok(NULL, "\"\n"); //El Value en el INSERT

				if (actual != NULL) //Si es NULL la cantidad de parametros es incorrecta
				{
					valor = strdup(actual);
					list_add(listaParam, valor);
					actual = strtok(NULL, " \n");
					if (actual != NULL) //Si no es NULL se introdujo el timestamp opcional
					{
						valor = strdup(actual); //Timestamp
						timestampRequest = string_a_mseg(valor);
					}
					else{
						free(actual);
					}
				}
				else{ //No se introdujo el value
					free(actual);
					break;
				}
			}
		}
		actual = strtok(NULL, " \n"); //(Si es el insert, el 2do parametro)
	}

	free(requestCopy);
	free(comando);

	if (es_comando_valido(comandoReconocido, listaParam))
	{
		instr_t* instruccionLista;
		switch (queConsola)
		{
		case CONSOLA_KERNEL:
			instruccionLista = crear_instruccion(timestampRequest, comandoReconocido + BASE_CONSOLA_KERNEL, listaParam);
			return instruccionLista;
			break;
		case CONSOLA_MEMORIA:
			instruccionLista = crear_instruccion(timestampRequest, comandoReconocido + BASE_CONSOLA_MEMORIA, listaParam);
			return instruccionLista;
			break;
		case CONSOLA_FS:
			instruccionLista = crear_instruccion(timestampRequest, comandoReconocido + BASE_CONSOLA_FS, listaParam);
			return instruccionLista;
			break;
		default:
			instruccionLista = crear_instruccion(timestampRequest, comandoReconocido + BASE_CONSOLA_KERNEL, listaParam);
			return instruccionLista;
			break;
		}
	}
	else
	{
		list_destroy_and_destroy_elements(listaParam, free); //Hay que liberar todos los parametros del INSERT (Tabla y key)
		puts("Input invalido: cantidad de parametros invalida");
		return NULL;
	}
}

cod_op reconocer_comando(char *comando)
{
	if (strcmp(comando, "SELECT") == 0)
		return CODIGO_SELECT;

	else if (strcmp(comando, "INSERT") == 0)
		return CODIGO_INSERT;

	else if (strcmp(comando, "CREATE") == 0)
		return CODIGO_CREATE;

	else if (strcmp(comando, "DESCRIBE") == 0)
		return CODIGO_DESCRIBE;

	else if (strcmp(comando, "DROP") == 0)
		return CODIGO_DROP;

	else if (strcmp(comando, "JOURNAL") == 0)
		return CODIGO_JOURNAL;

	else if (strcmp(comando, "ADD") == 0)
		return CODIGO_ADD;

	else if (strcmp(comando, "RUN") == 0)
		return CODIGO_RUN;

	else if (strcmp(comando, "METRICS") == 0)
		return CODIGO_METRICS;

	else if (strcmp(comando, "SHOW") == 0)
		return CODIGO_SHOW;

	else if (strcmp(comando, "CERRAR") == 0)
			return CODIGO_CERRAR;

	else
		return INPUT_ERROR;
}

int es_comando_valido(cod_op comando, t_list *listaParam)
{
	int cantParam = list_size(listaParam);
	switch (comando)
	{
	case CODIGO_SELECT:
		return cantParam == CANT_PARAM_SELECT;
		break;
	case CODIGO_INSERT:
		return (cantParam == CANT_PARAM_INSERT || cantParam == CANT_PARAM_INSERT_COMPLETO);
		break;
	case CODIGO_CREATE:
		return cantParam == CANT_PARAM_CREATE;
		break;
	case CODIGO_DESCRIBE:
		return (cantParam == CANT_PARAM_DESCRIBE || cantParam == CANT_PARAM_DESCRIBE_COMPLETO);
		break;
	case CODIGO_DROP:
		return cantParam == CANT_PARAM_DROP;
		break;
	case CODIGO_JOURNAL:
		return cantParam == CANT_PARAM_JOURNAL;
		break;
	case CODIGO_ADD:
		return cantParam == CANT_PARAM_ADD;
		break;
	case CODIGO_METRICS:
		return cantParam == CANT_PARAM_METRICS;
		break;
	case CODIGO_RUN:
		return cantParam == CANT_PARAM_RUN;
		break;

	case CODIGO_CERRAR:
		return cantParam == CANT_PARAM_CERRAR;

	case INPUT_ERROR:
		return -1;
		break;

	case CODIGO_SHOW: //Para testing
		return 1;

	default:
		return -1;
		break;
	}
}

instr_t *crear_instruccion(mseg_t timestampNuevo, cod_op codInstNuevo, t_list *listaParamNueva)
{

	instr_t instruccionCreada = {
		.timestamp = timestampNuevo,
		.codigo_operacion = codInstNuevo,
		.parametros = listaParamNueva};

	instr_t *miInstr = malloc(sizeof(instruccionCreada));
	memcpy(miInstr, &instruccionCreada, sizeof(instruccionCreada));

	return miInstr;
}

void imprimir_instruccion(instr_t *instruccion, void (*funcion_log)(char *texto))
{
	char *texto = string_new();
	string_append_with_format(&texto, "\n");

	void iterator(char *value)
	{
		string_append_with_format(&texto, "\t%s\n", value);
	}

	string_append_with_format(&texto,"Timestamp: %" PRIu64 "\n", instruccion->timestamp);
	string_append_with_format(&texto,"CodigoInstruccion: %d\n", instruccion->codigo_operacion);
	string_append_with_format(&texto,"Parametros:\n");
	list_iterate(instruccion->parametros, (void *)iterator);

	funcion_log(texto);
}

cod_op quien_pidio(instr_t *instruccion)
{
	int codigoAnalizado = instruccion->codigo_operacion;

	if (codigoAnalizado > BASE_COD_ERROR)
		codigoAnalizado -= BASE_COD_ERROR;

	if (codigoAnalizado < BASE_CONSOLA_MEMORIA)
		return CONSOLA_KERNEL;

	if (codigoAnalizado < BASE_CONSOLA_FS)
		return CONSOLA_MEMORIA;

	return CONSOLA_FS;
}

void imprimir_registro(instr_t *instruccion, void (*funcion_log)(char *texto))
{
	char *texto = string_new();
	string_append_with_format(&texto,"Key %s\n", (char *)list_get(instruccion->parametros, 1));
	string_append_with_format(&texto,"Value %s\n", (char *)list_get(instruccion->parametros, 2));
	string_append_with_format(&texto,"Timestamp: %" PRIu64 "\n", instruccion->timestamp);
	funcion_log(texto);
}

void loggear_error_proceso(instr_t *miInstruccion)
{
	loggear_warning(string_from_format("%s\n", (char *)list_get(miInstruccion->parametros, 0)));
}

void loggear_exito_proceso(instr_t *miInstruccion)
{

	void mostrar(char *parametro)
	{
		loggear_info(string_from_format(parametro));
	}

	list_iterate(miInstruccion->parametros, (void *)mostrar);
}

void imprimir_conexiones(t_dictionary *conexAc, void (*funcion_log)(char *texto))
{
	char *texto = string_new();
	void iterator(char *key, identificador *idsProceso)
	{
		string_append_with_format(&texto, "\n");
		string_append_with_format(&texto, "IP %s	|", idsProceso->ip_proceso);
		string_append_with_format(&texto, " Puerto %s |", idsProceso->puerto);
		string_append_with_format(&texto, " fd_out %d	|", idsProceso->fd_out);
		string_append_with_format(&texto, " fd_in %d	|", idsProceso->fd_in);
		string_append_with_format(&texto, " %s", key);
	}
	dictionary_iterator(conexAc, (void *)iterator);
	funcion_log(texto);
}

//TODO: cambiar
bool str_to_uint16(char *str, uint16_t *res)
{
	char *end;
	//    errno = 0;
	long val = strtol(str, &end, 10);
	//    if (errno || end == str || *end != '\0' || val < 0 || val >= 0x10000) {
	//        return false;
	//    }
	*res = (uint16_t)val;
	return true;
}

void uint16_to_str(uint16_t key, char **keyChar)
{
	sprintf((*keyChar), "%d", key);
}

void *obtener_ultimo_parametro(instr_t *instruccion)
{
	return list_get(instruccion->parametros, list_size(instruccion->parametros) - 1);
}

t_list *string_array_to_list(char **stringArray)
{
	t_list *listaStrings = list_create();
	void agregar_a_lista(char *stringAAgregar)
	{
		list_add(listaStrings, stringAAgregar);
	}

	string_iterate_lines(stringArray, (void *)agregar_a_lista);

	return listaStrings;
}

int enviar_request_simple(instr_t *instruccionAEnviar, int socket_cliente)
{
	int tamanio=0;
	void *a_enviar = NULL;
	a_enviar = serializar_request(instruccionAEnviar, &tamanio);
	int s = send(socket_cliente, a_enviar, tamanio, MSG_DONTWAIT);
	free(a_enviar);
	return s;
}

