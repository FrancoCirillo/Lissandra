//--------misConexiones.c--------

#include "misConexiones.h"

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

void imprimir_donde_corresponda(cod_op codigoOperacion, instr_t *instruccion, t_list *listaParam)
{
	list_add(listaParam, obtener_ultimo_parametro(instruccion));
	instr_t *miInstruccion;
	switch (quien_pidio(instruccion))
	{
	case CONSOLA_KERNEL:
		miInstruccion = crear_instruccion(obtener_ts(), codigoOperacion + BASE_CONSOLA_KERNEL, listaParam);
		int conexionKernel = obtener_fd_out("Kernel");
		enviar_request(miInstruccion, conexionKernel);
		break;
	default:
		miInstruccion = crear_instruccion(obtener_ts(), codigoOperacion, listaParam);
		if (codigoOperacion == CODIGO_EXITO)
			loggear_exito(miInstruccion, g_logger);
		if (codigoOperacion > BASE_COD_ERROR){
			loggear_error(miInstruccion, g_logger);
		}
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
