//-------fileSystem.c-------

#include "fileSystemMock.h"

int main()
{
	printf(COLOR_ANSI_AMARILLO "	PROCESO FILESYSTEM" COLOR_ANSI_RESET "\n");

	conexionesActuales = dictionary_create();
	callback = ejecutar_instruccion;

	inicializar_configuracion();

	int listenner = iniciar_servidor(IP_FILESYSTEM, PORT);

	vigilar_conexiones_entrantes(listenner, callback, conexionesActuales, CONSOLA_FS);

	return 0;
}

void ejecutar_instruccion(instr_t *instruccion, char *remitente)
{	puts("Ejecutando...");
	switch (instruccion->codigo_operacion)
	{
	case CONSOLA_FS_SELECT:
	case CONSOLA_MEM_SELECT:
	case CONSOLA_KRN_SELECT:
		ejecutar_instruccion_select(instruccion, remitente);
		break;
	case CONSOLA_FS_INSERT:
	case CONSOLA_MEM_INSERT:
	case CONSOLA_KRN_INSERT:
		ejecutar_instruccion_insert(instruccion, remitente);
		break;
	case CONSOLA_FS_CREATE:
	case CONSOLA_MEM_CREATE:
	case CONSOLA_KRN_CREATE:
		ejecutar_instruccion_create(instruccion, remitente);
		break;
	case CONSOLA_FS_DESCRIBE:
	case CONSOLA_MEM_DESCRIBE:
	case CONSOLA_KRN_DESCRIBE:
		ejecutar_instruccion_describe(instruccion, remitente);
		break;
	case CONSOLA_FS_DROP:
	case CONSOLA_MEM_DROP:
	case CONSOLA_KRN_DROP:
		ejecutar_instruccion_drop(instruccion, remitente);
		break;
	default:
		puts("El comando no pertenece al FileSystem");
		break;
	}
}

void ejecutar_instruccion_select(instr_t *instruccion, char *remitente)
{
	puts("Ejecutando instruccion Select");
	int tablaPreexistente = 1;
	if (tablaPreexistente)
	{
		t_list *listaParam = list_create();
		list_add(listaParam, (char *)list_get(instruccion->parametros, 0)); //Tabla
		list_add(listaParam, (char *)list_get(instruccion->parametros, 1)); //Key
		list_add(listaParam, "Buenas soy un value");						//Value
		imprimir_donde_corresponda(DEVOLUCION_SELECT, instruccion, listaParam, remitente);
	}
	else
	{
		t_list *listaParam = list_create();
		char cadena[400];
		sprintf(cadena,
				"%s No existe",
				(char *)list_get(instruccion->parametros, 0));
		list_add(listaParam, cadena);
		imprimir_donde_corresponda(ERROR_SELECT, instruccion, listaParam, remitente);
	}
}

void ejecutar_instruccion_insert(instr_t *instruccion, char *remitente)
{
	puts("Ejecutando instruccion Insert");
	sleep(1);
	t_list *listaParam = list_create();
	char cadena[400];
	sprintf(cadena,
			"Se inserto %s | %s | %s | %"PRIu64,
			(char *)list_get(instruccion->parametros, 0),
			(char *)list_get(instruccion->parametros, 1),
			(char *)list_get(instruccion->parametros, 2),
			(mseg_t)instruccion->timestamp);
	list_add(listaParam, cadena);
	imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam, remitente);
}

void ejecutar_instruccion_create(instr_t *instruccion, char *remitente)
{
	puts("Ejecutando instruccion Create");

	t_list *listaParam = list_create();
	char cadena[400];
	sprintf(cadena,
			"Se creo la tabla %s con tipo de consistencia %s, tiene %s particiones y un tiempo de compactacion de %s",
			(char *)list_get(instruccion->parametros, 0),
			(char *)list_get(instruccion->parametros, 1),
			(char *)list_get(instruccion->parametros, 2),
			(char *)list_get(instruccion->parametros, 3));
	list_add(listaParam, cadena);

	imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam, remitente);
}

void ejecutar_instruccion_describe(instr_t *instruccion, char *remitente)
{
	puts("Ejecutando instruccion Describe");
	t_list *listaParam = list_create();
	char cadena[400];
	sprintf(cadena,
			"Metadata %s :\n	Tipo de consistencia: \n	Numero de Particiones: 4\n	Tiempo entre compactaciones: 60000",
			(char *)list_get(instruccion->parametros, 0));
	list_add(listaParam, cadena);

	imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam, remitente);
}

void ejecutar_instruccion_drop(instr_t *instruccion, char *remitente)
{
	puts("Ejecutando instruccion Drop");
//	t_list *listaParam = list_create();
//	char cadena[400];
//	sprintf(cadena, "%s%s", "Se borro la ", (char *)list_get(instruccion->parametros, 0));
//	list_add(listaParam, cadena);
//	imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam, remitente);
	char cadena[500];
	t_list *listaParam = list_create();
	sprintf(cadena, "No se pudo borrar la tabla del FS");
	list_add(listaParam, cadena);
	imprimir_donde_corresponda(ERROR_DROP, instruccion, listaParam, remitente);
}

void imprimir_donde_corresponda(cod_op codigoOperacion, instr_t *instruccion, t_list *listaParam, char *remitente)
{
	instr_t *miInstruccion;
	switch (quien_pidio(instruccion))
	{

	case CONSOLA_KERNEL:
		miInstruccion = crear_instruccion(obtener_ts(), codigoOperacion + BASE_CONSOLA_KERNEL, listaParam);
		int conexionReceptor1 = obtener_fd_out(remitente);
		enviar_request(miInstruccion, conexionReceptor1);
		break;
	case CONSOLA_MEMORIA:
		miInstruccion = crear_instruccion(obtener_ts(), codigoOperacion + BASE_CONSOLA_MEMORIA, listaParam);
		int conexionReceptor2 = obtener_fd_out(remitente);
		enviar_request(miInstruccion, conexionReceptor2);
		break;
	default:
		miInstruccion = crear_instruccion(obtener_ts(), codigoOperacion, listaParam);
		//Se pidio desde la consola de FS
		if (codigoOperacion == DEVOLUCION_SELECT)
		{
			imprimir_registro(miInstruccion);
		}
		if (codigoOperacion == CODIGO_EXITO)
		{
			loggear_exito(miInstruccion);
		}
		if (codigoOperacion > BASE_COD_ERROR)
		{
			loggear_error(miInstruccion);
		}
		break;
	}
}

void inicializar_configuracion()
{
	puts("Configuracion:");
	char *rutaConfig = "fsMock.config";
	puts("Config accedido.");
	configuracion.PUERTO_ESCUCHA = obtener_por_clave(rutaConfig, "PUERTO_ESCUCHA");
	configuracion.TAMANIO_VALUE = atoi(obtener_por_clave(rutaConfig, "TAMANIO_VALUE"));
	configuracion.TIEMPO_DUMP = atoi(obtener_por_clave(rutaConfig, "TIEMPO_DUMP"));
	configuracion.RUTA_LOG = obtener_por_clave(rutaConfig, "RUTA_LOG");
}

void loggear(char *valor)
{
	g_logger = log_create(configuracion.RUTA_LOG, "fs", 1, LOG_LEVEL_INFO);
	log_info(g_logger, valor);
	log_destroy(g_logger);
}

char *obtener_por_clave(char *ruta, char *clave)
{
	char *valor;
	g_config = config_create(ruta);
	valor = config_get_string_value(g_config, clave);
	printf(" %s: %s \n", clave, valor);

	return valor;
}

void responderHandshake(identificador *idsConexionEntrante)
{
	t_list *listaParam = list_create();
	list_add(listaParam, "FileSystem");
	list_add(listaParam, IP_FILESYSTEM);
	list_add(listaParam, PORT); //TODO configuracion.PUERTO
	instr_t *miInstruccion = miInstruccion = crear_instruccion(obtener_ts(), CODIGO_HANDSHAKE, listaParam);

	int fd_saliente = crear_conexion(idsConexionEntrante->ip_proceso, idsConexionEntrante->puerto, IP_FILESYSTEM);
	enviar_request(miInstruccion, fd_saliente);
	idsConexionEntrante->fd_out = fd_saliente;
}

int obtener_fd_out(char *proceso)
{
	identificador *idsProceso = (identificador *)dictionary_get(conexionesActuales, proceso);
	if (idsProceso->fd_out == 0)
	{ //Es la primera vez que se le quiere enviar algo a proceso
		responderHandshake(idsProceso);
	}
	return idsProceso->fd_out;
}
