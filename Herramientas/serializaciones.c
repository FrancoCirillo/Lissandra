//-------serializaciones.c-------

#include "serializaciones.h"

t_paquete *instruccion_a_paquete(instr_t *instruccionAEnviar)
{

	t_paquete *paqueteAEnviar = crear_paquete(instruccionAEnviar->codigo_operacion, instruccionAEnviar->timestamp);
	int tamanioTotal = 0;
	void iterator(void *valor)
	{
		int tamanio = strlen((char *)valor) + 1;
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
	void *magic = malloc(bytes);

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
	if(paqueteAEnviar->buffer->size<0){ //Pasa si una instruccion no tiene parametros
		paqueteAEnviar->buffer->size = 0;
	}
	int bytes =  paqueteAEnviar->buffer->size + sizeof(int) + sizeof(cod_op) + sizeof(mseg_t);
	void *paqueteSerializado = serializar_paquete(paqueteAEnviar, bytes);
	*tamanio = bytes;
	return paqueteSerializado;
}

int enviar_request(instr_t *instruccionAEnviar, int socket_cliente)
{
	int tamanio;
	void *a_enviar = serializar_request(instruccionAEnviar, &tamanio);
	int s = send(socket_cliente, a_enviar, tamanio, MSG_DONTWAIT);
	free(a_enviar);
	return s;
}

//Hay que crear un buffer para los parametros
void crear_buffer(t_paquete *paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

t_paquete *crear_paquete(cod_op nuevoCodOp, mseg_t nuevoTimestamp)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));
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
	buffer = malloc(*size);
	if(*size !=0) recv(socket_cliente, buffer, *size, MSG_WAITALL); //Pasa cuando recibimos las requests sin parametros
	return buffer;
}

t_list *recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void *buffer;
	t_list *valores = list_create();
	int tamanio;
	int i = 0; //debug

	buffer = recibir_buffer(&size, socket_cliente);
	while (desplazamiento < size)
	{
		i++;
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento += sizeof(int);
		char *valor = malloc(tamanio);
		memcpy(valor, buffer + desplazamiento, tamanio);
		desplazamiento += tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
	return NULL;
}

int recibir_request(int socket_cliente, instr_t **instruccion)
{
	mseg_t nuevoTimestamp;
	cod_op nuevoCodOp;
	t_list *listaParam;

	int t = recibir_timestamp(socket_cliente, &nuevoTimestamp); //return en error
	if (t <= 0)
		return t;

	t = recibir_operacion(socket_cliente, &nuevoCodOp);
	if (t <= 0)
		return t;

	listaParam = recibir_paquete(socket_cliente); //del TP0

	(*instruccion) = malloc(sizeof(mseg_t) * sizeof(cod_op) + sizeof(listaParam));
	(*instruccion)->timestamp = nuevoTimestamp;
	(*instruccion)->codigo_operacion = nuevoCodOp;
	(*instruccion)->parametros = listaParam;

	return 1;
}

//TODO: usar logger
int recibir_timestamp(int socket_cliente, mseg_t *nuevoTimestamp)
{
	int r = recv(socket_cliente, nuevoTimestamp, sizeof(mseg_t), MSG_WAITALL);
	if (r == 0)
	{
		perror("El cliente se desconecto: ");
	}
	if (r < 0)
	{
		perror("Error en el recv: ");
	}
	return r;
}

int recibir_operacion(int socket_cliente, cod_op *nuevaOperacion)
{
	int r = recv(socket_cliente, nuevaOperacion, sizeof(cod_op), MSG_WAITALL);
	if (r == 0)
	{
		perror("El cliente se desconecto: ");
	}
	if (r < 0)
	{
		perror("Error en el recv: ");
	}
	return r;
}

instr_t *leer_a_instruccion(char *request, int queConsola)
{
	char *requestCopy = strdup(request);
	char *actual, *comando, *valor;
	comando = NULL;
	valor = malloc(sizeof(int));
	t_list *listaParam = list_create();

	if (strcmp(requestCopy, "CERRAR\n") == 0)
	{
		printf(COLOR_ANSI_CYAN "Gracias por usar Lissandra FS!\n" COLOR_ANSI_RESET "cerrando...\n");
		exit(0);
	}

	mseg_t timestampRequest = obtener_ts();

	actual = strtok(requestCopy, " \n");
	if (actual == NULL)
		return NULL;

	comando = strdup(actual);
	actual = strtok(NULL, " \n");
	for (int i = 1; actual != NULL; i++)
	{
		valor = strdup(actual);
		list_add(listaParam, valor);

		if(i == 2 && strcmp(comando, "INSERT") == 0)
		{
			actual = strtok (NULL, "\"\n");
			if(actual != NULL) //Si es NULL la cantidad de parametros es incorrecta
			{
				valor = strdup(actual);
				list_add(listaParam, valor);
				actual = strtok (NULL, " \n");
				if(actual != NULL) //Si es NULL no se introdujo el timestamp opcional
				{
					valor = strdup(actual);
					timestampRequest = string_a_mseg(valor);
				}
			}

		}
		actual = strtok(NULL, " \n");
	}
	free(requestCopy);

	cod_op comandoReconocido = reconocer_comando(comando);
	if (es_comando_valido(comandoReconocido, listaParam) > 0)
	{
		switch (queConsola)
		{
		case CONSOLA_KERNEL:
			return crear_instruccion(timestampRequest, comandoReconocido + BASE_CONSOLA_KERNEL, listaParam);
			break;
		case CONSOLA_MEMORIA:
			return crear_instruccion(timestampRequest, comandoReconocido + BASE_CONSOLA_MEMORIA, listaParam);
			break;
		case CONSOLA_FS:
			return crear_instruccion(timestampRequest, comandoReconocido + BASE_CONSOLA_FS, listaParam);
			break;
		default:
			return crear_instruccion(timestampRequest, comandoReconocido + BASE_CONSOLA_KERNEL, listaParam);
			break;
		}
	}
	else
	{
		puts("Input invalido");
		return NULL;
	}
}

cod_op reconocer_comando(char *comando)
{
	if (strcmp(comando, "SELECT") == 0)
		return CODIGO_SELECT;

	else if (strcmp(comando, "DEVOLUCION_SELECT") == 0)
		return DEVOLUCION_SELECT;

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

	case INPUT_ERROR:
		return -1;
		break;

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

void imprimir_instruccion(instr_t *instruccion)
{

	void iterator(char *value){
		printf("%s\n", value);
	}

	printf("Timestamp: %"PRIu64"\n",instruccion->timestamp);
	printf("CodigoInstruccion: %d\n", instruccion->codigo_operacion);
	printf("Parametros:\n");
	list_iterate(instruccion->parametros, (void *)iterator);
	puts("");
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

void imprimir_registro(instr_t *instruccion)
{
	printf("Key %s\n", (char *)list_get(instruccion->parametros, 1));
	printf("Value %s\n", (char *)list_get(instruccion->parametros, 2));
	printf("Timestamp: %"PRIu64"\n", instruccion->timestamp);
}

//Todo: usar logger
void loggear_error(instr_t *miInstruccion)
{
	printf(COLOR_ANSI_ROJO "%s\n" COLOR_ANSI_RESET, (char *)list_get(miInstruccion->parametros, 0));
}
void loggear_exito(instr_t *miInstruccion)
{
	printf("%s\n", (char *)list_get(miInstruccion->parametros, 0));
}

void imprimir_conexiones(t_dictionary *conexAc)
{
	puts("Conexiones conocidas:");
	void iterator(char *key, identificador *idsProceso)
	{
		printf("Proceso: %s\n", key);
		printf("IP %s\n", idsProceso->ip_proceso);
		printf("Puerto %s\n", idsProceso->puerto);
		printf("fd_out %d\n", idsProceso->fd_out);
		printf("fd_in %d\n", idsProceso->fd_in);
	}
	dictionary_iterator(conexAc, (void *)iterator);
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

