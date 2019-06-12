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

void devolver_gossip(instr_t *instruccion, char *remitente){
	int conexionRemitente = obtener_fd_out(remitente);
	t_list* tablaGossiping = conexiones_para_gossiping();

	instr_t* miInstruccion = crear_instruccion(obtener_ts(), RECEPCION_GOSSIP, tablaGossiping);

	enviar_request(miInstruccion, conexionRemitente);

	actualizar_tabla_gossiping(instruccion);

}

void actualizar_tabla_gossiping(instr_t* instruccion){

	int i = 0;
	char* nombre;
	char* ip;
	char* puerto;

	void acutalizar_tabla(char* parametro){
		if(i % 3 == 0){
			if(!dictionary_has_key(conexionesActuales, parametro)){
				nombre = strdup(parametro);
				i++;
			}
			else{
				i+=3;
			}
		}
		if(i % 3 == 1){
			ip = strdup(parametro);
			i++;
		}
		if(i % 3 == 2){
			puerto = strdup(parametro);

			identificador identificadores = {
					.fd_out = 0,
					.fd_in = 0,
					.ip_proceso = *ip,
					.puerto = *puerto
			};

			identificador* idsConexionesActuales = malloc(sizeof(identificadores));
			memcpy(idsConexionesActuales, &identificadores, sizeof(identificadores));

			dictionary_put(conexionesActuales,nombre, idsConexionesActuales);
			i++;
		}
	}

	list_iterate(instruccion->parametros, (void*)acutalizar_tabla);
}

t_list *conexiones_para_gossiping(){


	t_list *tablaGossiping = list_create();

	void juntar_ip_y_puerto(char* nombre, identificador* ids){
		list_add(tablaGossiping, nombre);
		list_add(tablaGossiping, ids->ip_proceso);
		list_add(tablaGossiping, ids->puerto);
	}

	dictionary_iterator(conexionesActuales, (void *)juntar_ip_y_puerto);

	return tablaGossiping;

}

void ejecutar_instruccion_gossip(){
	loggear_info(g_logger, &mutex_log, string_from_format("Ejecutando instruccion Gossip"));


}


