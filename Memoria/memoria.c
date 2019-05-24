//---------memoria.c---------

#include "memoria.h"

int main(int argc, char *argv[])
{

	check_inicial(argc, argv);
	iniciar_log();
	iniciar_config();
	inicializar_configuracion();
	conexionesActuales = dictionary_create();
	idsNuevasConexiones = malloc(sizeof(identificador));

	callback = ejecutar_instruccion;


	enviar_datos_a_FS(argv);


	int listenner = iniciar_servidor(miIPMemoria, configuracion.PUERTO);

	//recibir el tamanio del Value


	memoriaPrincipal = gran_malloc_inicial();

	vigilar_conexiones_entrantes(listenner, callback, conexionesActuales, CONSOLA_MEMORIA);
	//config_destroy(g_config);

	return 0;
}

void inicializar_configuracion()
{
	puts("Configuracion:");
	char *rutaConfig = "memoria.config";
	configuracion.PUERTO = obtener_por_clave(rutaConfig, "PUERTO");
	configuracion.IP_FS = obtener_por_clave(rutaConfig, "IP_FS");
	configuracion.PUERTO_FS = obtener_por_clave(rutaConfig, "PUERTO_FS");
	configuracion.IP_SEEDS = obtener_por_clave(rutaConfig, "IP_SEEDS");
	configuracion.PUERTO_SEEDS = obtener_por_clave(rutaConfig, "PUERTO_SEEDS");
	configuracion.RETARDO_MEMORIA = atoi(obtener_por_clave(rutaConfig, "RETARDO_MEMORIA"));
	configuracion.RETARDO_FS = atoi(obtener_por_clave(rutaConfig, "RETARDO_FS"));
	configuracion.TAMANIO_MEMORIA = atoi(obtener_por_clave(rutaConfig, "TAMANIO_MEMORIA"));
	configuracion.RETARDO_JOURNAL = atoi(obtener_por_clave(rutaConfig, "RETARDO_JOURNAL"));
	configuracion.RETARDO_GOSSIPING = atoi(obtener_por_clave(rutaConfig, "RETARDO_GOSSIPING"));
	configuracion.MEMORY_NUMBER = atoi(obtener_por_clave(rutaConfig, "MEMORY_NUMBER"));
	configuracion.RUTA_LOG = obtener_por_clave(rutaConfig, "RUTA_LOG");
}


void iniciar_log()
{
	g_logger = log_create(configuracion.RUTA_LOG, nombreDeMemoria, 1, LOG_LEVEL_INFO);

}

void iniciar_config()
{
	g_config = config_create("memoria.config");
}

char *obtener_por_clave(char *ruta, char *clave)
{
	char *valor;
	valor = config_get_string_value(g_config, clave);
	printf(" %s: %s \n", clave, valor);

	return valor;
}

void ejecutar_instruccion(instr_t *instruccion, char *remitente)
{

	switch (instruccion->codigo_operacion)
	{
	case CONSOLA_MEM_SELECT:
	case CONSOLA_KRN_SELECT:
		ejecutar_instruccion_select(instruccion);
		break;
	case CONSOLA_KRN_RTA_SELECT:
	case CONSOLA_MEM_RTA_SELECT:
		ejecutar_instruccion_devolucion_select(instruccion);
		break;
	case CONSOLA_MEM_INSERT:
	case CONSOLA_KRN_INSERT:
		ejecutar_instruccion_insert(instruccion);
		break;
	case CONSOLA_MEM_CREATE:
	case CONSOLA_KRN_CREATE:
		ejecutar_instruccion_create(instruccion);
		break;
	case CONSOLA_MEM_DESCRIBE:
	case CONSOLA_KRN_DESCRIBE:
		ejecutar_instruccion_describe(instruccion);
		break;
	case CONSOLA_MEM_DROP:
	case CONSOLA_KRN_DROP:
		ejecutar_instruccion_drop(instruccion);
		break;
	case CONSOLA_MEM_JOURNAL:
	case CONSOLA_KRN_JOURNAL:
		ejecutar_instruccion_journal(instruccion);
		break;
	case CONSOLA_MEM_EXITO:
	case CONSOLA_KRN_EXITO:
		ejecutar_instruccion_exito(instruccion);
		break;

	default:
		puts("El comando no pertenece a la Memoria");
		break;
	}
}

void ejecutar_instruccion_select(instr_t *instruccion)
{
	puts("Ejecutando instruccion Select");
	int seEncontro = 0; //No cambiar hasta que se implemente conexionKERNEL
	sleep(1);			//Buscar
	if (seEncontro)
	{
		//Directo para el Kernel
		t_list *listaParam = list_create();
		char cadena[400];
		sprintf(cadena, "%s%s%s%s%s%s%s%u", "Se encontro ", (char *)list_get(instruccion->parametros, 0), " | ", (char *)list_get(instruccion->parametros, 1), " | ", (char *)list_get(instruccion->parametros, 2), " | ", (unsigned int)instruccion->timestamp);
		list_add(listaParam, cadena);

		imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam);
	}
	else
	{
		puts("La tabla no se encontro en Memoria. Consultando al FS");
		int conexionFS = obtener_fd_out("FileSystem");
		enviar_request(instruccion, conexionFS);
	}
}

void ejecutar_instruccion_devolucion_select(instr_t *instruccion)
{
	puts("Select realizado en FS, se guardo la siguiente tabla en la memoria:");
	print_instruccion(instruccion);
	t_list *listaParam = list_create();
	char cadena[400];
	sprintf(cadena, "%s%s%s%s%s%s%s%u", "Se encontro ",
										(char *)list_get(instruccion->parametros, 0), " | ",
										(char *)list_get(instruccion->parametros, 1), " | ",
										(char *)list_get(instruccion->parametros, 2), " | ",
										(unsigned int)instruccion->timestamp);
	list_add(listaParam, cadena);
	imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam);
}

void ejecutar_instruccion_insert(instr_t *instruccion)
{
	puts("Ejecutando instruccion Insert");
	if(0)
	{
	int conexionFS = obtener_fd_out("FileSystem");
	enviar_request(instruccion, conexionFS);
	}
	else
	{
		t_list *listaParam = list_create();
		char cadena[400];
		sprintf(cadena, "%s%s%s%s%s%s%s%u", "Se inserto ", (char *)list_get(instruccion->parametros, 0), " | ", (char *)list_get(instruccion->parametros, 1), " | ", (char *)list_get(instruccion->parametros, 2), " | ", (unsigned int)instruccion->timestamp);
		list_add(listaParam, cadena);
		imprimir_donde_corresponda(CODIGO_EXITO, instruccion, listaParam);

	}
}

void ejecutar_instruccion_create(instr_t *instruccion)
{
	puts("Ejecutando instruccion Create");
	int conexionFS = obtener_fd_out("FileSystem");
	enviar_request(instruccion, conexionFS);
}

void ejecutar_instruccion_describe(instr_t *instruccion)
{
	puts("Ejecutando instruccion Describe");
	puts("La tabla no se encontro en Memoria. Consultando al FS");
	int conexionFS = obtener_fd_out("FileSystem");
	puts("Se tiene el fd_out");
	if(enviar_request(instruccion, conexionFS)==-1) puts("No se envio");
	puts("Se envio al FS");
}

void ejecutar_instruccion_drop(instr_t *instruccion)
{
	puts("Ejecutando instruccion Drop");
	int conexionFS = obtener_fd_out("FileSystem");
	enviar_request(instruccion, conexionFS);
}

void ejecutar_instruccion_journal(instr_t *instruccion)
{
	puts("Ejecutando instruccion Journal");
	int conexionFS = obtener_fd_out("FileSystem");
	enviar_request(instruccion, conexionFS);
}

void ejecutar_instruccion_exito(instr_t *instruccion)
{
	imprimir_donde_corresponda(CODIGO_EXITO, instruccion, instruccion->parametros);
}

void imprimir_donde_corresponda(cod_op codigoOperacion, instr_t *instruccion, t_list *listaParam)
{
	instr_t *miInstruccion;
	switch (quienEnvio(instruccion))
	{
	case CONSOLA_KERNEL:
		miInstruccion = crear_instruccion(obtener_ts(), codigoOperacion + BASE_CONSOLA_KERNEL, listaParam);
		int conexionKernel = obtener_fd_out("Kernel");
		enviar_request(miInstruccion, conexionKernel);
		break;
	default:
		miInstruccion = crear_instruccion(obtener_ts(), codigoOperacion, listaParam);
		if (codigoOperacion == CODIGO_EXITO)
			loggear_exito(miInstruccion);
		if (codigoOperacion > BASE_COD_ERROR)
			loggear_error(miInstruccion);
		break;
	}
}

//hace el connect!!
void responderHandshake(identificador *idsConexionEntrante)
{
	t_list *listaParam = list_create();
	list_add(listaParam, nombreDeMemoria);
	list_add(listaParam, miIPMemoria);
	list_add(listaParam, configuracion.PUERTO);
	instr_t *miInstruccion = miInstruccion = crear_instruccion(obtener_ts(), CODIGO_HANDSHAKE, listaParam);

	int fd_saliente = crear_conexion(idsConexionEntrante->ip_proceso, idsConexionEntrante->puerto, miIPMemoria);
	enviar_request(miInstruccion, fd_saliente);
	idsConexionEntrante->fd_out = fd_saliente;
}

int obtener_fd_out(char *proceso)
{
	identificador *idsProceso = (identificador *)dictionary_get(conexionesActuales, proceso);
	if (idsProceso->fd_out == 0)
	{ //Es la primera vez que se le quiere enviar algo a proceso
//		puts("Es la primera vez que se le quiere enviar algo a proceso\n"); //Para debug
		responderHandshake(idsProceso);
		return idsProceso->fd_out;
	}
	//	La conexion en el fd_out %d ya existia
//	puts("La conexion en el fd_out ya existia"); //Para debug
	return idsProceso->fd_out;
}

void check_inicial(int argc, char* argv[])
{

	if (argc < 2 || strcmp(argv[1], "4") == 0 || strcmp(argv[1], "2") == 0)
	{
		puts("Uso: MEMORIA <NUMERERO-DE-MEMORIA>");
		puts("<NUMERERO-DE-MEMORIA> Funcionales por el momento: 3, 8, 9");
		puts("Tampoco se puede elegir el numero 4 porque es el IP que (Por el momento, testing) usa el Kernel");
		puts("Tampoco se puede elegir el numero 2 porque es el IP que (Por el momento, testing) usa el FS");

		exit(0);
	}
	sprintf(nombreDeMemoria, "Memoria_%s", argv[1]);

}

//TODO: Cambiar para usar un IP decente (esta con argv para testing)
void enviar_datos_a_FS(char *argv[])
{
		t_list *listaParam = list_create();

		printf(COLOR_ANSI_VERDE "	PROCESO %s\n" COLOR_ANSI_RESET, nombreDeMemoria);

		sprintf(miIPMemoria, "127.0.0.%s", argv[1]);
		printf("IP Memoria: %s.\n", miIPMemoria);

		list_add(listaParam, nombreDeMemoria);
		list_add(listaParam, miIPMemoria);
		list_add(listaParam, configuracion.PUERTO);
		instr_t *miInstruccion = miInstruccion = crear_instruccion(obtener_ts(), CODIGO_HANDSHAKE, listaParam);

		int conexion_con_fs = crear_conexion(configuracion.IP_FS, configuracion.PUERTO_FS, miIPMemoria);
		enviar_request(miInstruccion, conexion_con_fs);

		idsNuevasConexiones->fd_in = 0; //Ojo
		strcpy(idsNuevasConexiones->puerto, configuracion.PUERTO_FS);
		strcpy(idsNuevasConexiones->ip_proceso, configuracion.IP_FS);
		idsNuevasConexiones->fd_out = conexion_con_fs;
		dictionary_put(conexionesActuales, "FileSystem", idsNuevasConexiones);
}

