//--------misConexiones.c--------

#include "misConexiones.h"

int obtener_fd_out(char *proceso)
{
	identificador *idsProceso = (identificador *)dictionary_get(conexionesActuales, proceso);
	if (idsProceso->fd_out == 0)
	{ //Es la primera vez que se le quiere enviar algo a proceso
		responderHandshake(idsProceso);
		return idsProceso->fd_out;
	}
	//	La conexion en el fd_out %d ya existia
	return idsProceso->fd_out;
}

void imprimir_donde_corresponda(cod_op codigoOperacion, instr_t *instruccion, t_list *listaParam)
{
	void *ultimoParametro = obtener_ultimo_parametro(instruccion);
	if (ultimoParametro != NULL){
		list_add(listaParam, (char*)ultimoParametro);
	}
	instr_t *miInstruccion;
	switch (quien_pidio(instruccion))
	{
	case CONSOLA_KERNEL:
		loggear_debug(debug_logger, &mutex_log, string_from_format("Enviando instruccion al Kernel"));
		miInstruccion = crear_instruccion(obtener_ts(), codigoOperacion + BASE_CONSOLA_KERNEL, listaParam);
		int conexionKernel = obtener_fd_out("Kernel");
		enviar_request(miInstruccion, conexionKernel);
		break;
	default:
		loggear_debug(debug_logger, &mutex_log, string_from_format("Loggeando resultado instruccion"));
		miInstruccion = crear_instruccion(obtener_ts(), codigoOperacion, listaParam);
		if (codigoOperacion == CODIGO_EXITO)
			loggear_exito_proceso(miInstruccion, g_logger, &mutex_log);
		if (codigoOperacion > BASE_COD_ERROR){
			loggear_error_proceso(miInstruccion, g_logger, &mutex_log);
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
	instr_t * miInstruccion = crear_instruccion(obtener_ts(), CODIGO_HANDSHAKE, listaParam);

	int fd_saliente = crear_conexion(idsConexionEntrante->ip_proceso, idsConexionEntrante->puerto, miIPMemoria, g_logger, &mutex_log);

	enviar_request(miInstruccion, fd_saliente);
	idsConexionEntrante->fd_out = fd_saliente;
}

//TODO: Cambiar para usar un IP decente (esta con argv para testing)
void enviar_datos_a_FS()
{
		t_list *listaParam = list_create();

		printf(COLOR_ANSI_VERDE "	PROCESO %s\n" COLOR_ANSI_RESET, nombreDeMemoria);
		printf("IP Memoria: %s.\n", miIPMemoria);

		list_add(listaParam, nombreDeMemoria);
		list_add(listaParam, miIPMemoria);
		list_add(listaParam, configuracion.PUERTO);
		instr_t * miInstruccion = crear_instruccion(obtener_ts(), CODIGO_HANDSHAKE, listaParam);

		int conexion_con_fs = crear_conexion(configuracion.IP_FS, configuracion.PUERTO_FS, miIPMemoria, g_logger, &mutex_log);
		enviar_request(miInstruccion, conexion_con_fs);

		idsNuevasConexiones->fd_in = 0; //Ojo
		strcpy(idsNuevasConexiones->puerto, configuracion.PUERTO_FS);
		strcpy(idsNuevasConexiones->ip_proceso, configuracion.IP_FS);
		idsNuevasConexiones->fd_out = conexion_con_fs;
		dictionary_put(conexionesActuales, "FileSystem", idsNuevasConexiones);
}

void pedir_tamanio_value(){

	puts("obteniendo fd out");
	int conexionFS = obtener_fd_out("FileSystem");
	puts("fd out obtenido");
	t_list* listaParam = list_create();
	instr_t* miInstruccion = crear_instruccion(obtener_ts(), CODIGO_VALUE, listaParam);
	puts("Instruccion creada");
	enviar_request(miInstruccion, conexionFS);
	puts("Request enviada");

}

void actualizar_tamanio_value(instr_t* instruccion){

	tamanioValue = atoi((char*) list_get(instruccion->parametros, 0));
	printf("Tamanio del value recibido: %d\n", tamanioValue);


}
